/*
 * Copyright 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #define LOG_NDEBUG 0
#define LOG_TAG "audio_utils_PowerLog"
#include <log/log.h>

#include <audio_utils/PowerLog.h>

#include <algorithm>
#include <iomanip>
#include <math.h>
#include <sstream>
#include <stdint.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <corecrt_io.h>
#endif
#include <vector>

#include <audio_utils/clock.h>
#include <audio_utils/LogPlot.h>
#include <audio_utils/power.h>

#include <cutils/cutils_export.h>

namespace android {

PowerLogBase::PowerLogBase(uint32_t sampleRate,
        uint32_t channelCount,
        audio_format_t format,
        size_t entries,
        size_t framesPerEntry)
    : mSampleRate(sampleRate)
    , mChannelCount(channelCount)
    , mFormat(format)
    , mFramesPerEntry(framesPerEntry)
    , mEntryTimeNs(framesPerEntry * 1e9 / sampleRate)
    , mMaxTimeSlipNs(std::min((int64_t)200'000'000, mEntryTimeNs))
    , mEntries(entries)
{
    (void)mFormat; // currently unused, for future use
    LOG_ALWAYS_FATAL_IF(!audio_utils_is_compute_power_format_supported(format),
            "unsupported format: %#x", format);
}

void PowerLogBase::processEnergy(size_t frames, float energy, int64_t nowNs) {
    // For big entries (i.e. 1 second+) we want to ensure we don't have new data
    // accumulating into a previous energy segment.
    if (mCurrentTime > 0
            && nowNs > mCurrentTime + mCurrentFrames * 1e9 / mSampleRate + mMaxTimeSlipNs) {
        flushEntry();
    }

    mCurrentEnergy += energy;

    // if we are in a zero run, do not advance.
    if (mCurrentEnergy == 0.f && mConsecutiveZeroes > 0) return;

    mCurrentFrames += frames;
    if (mCurrentTime == 0) {
        mCurrentTime = nowNs;
    }

    ALOGV("%s: nowNs:%lld, frames:%zu, mCurrentEnergy:%f, mCurrentFrames:%zu",
            __func__, (long long)nowNs, frames, mCurrentEnergy, mCurrentFrames);
    if (mCurrentFrames < mFramesPerEntry) return;

    flushEntry();
}

std::string PowerLogBase::dumpToString(
        const char *prefix, size_t lines, int64_t limitNs, bool logPlot) const
{
    const size_t maxColumns = 10;
    const size_t numberOfEntries = mEntries.size();
    if (lines == 0) lines = SIZE_MAX;

    // compute where to start logging
    enum {
        AT_END,
        IN_SIGNAL,
    } state = IN_SIGNAL;
    size_t count = 1;
    size_t column = 0;
    size_t nonzeros = 0;
    ssize_t offset; // TODO doesn't dump if # entries exceeds SSIZE_MAX
    for (offset = 0; offset < (ssize_t)numberOfEntries && count < lines; ++offset) {
        const size_t idx = (mIdx + numberOfEntries - offset - 1) % numberOfEntries;
                                                                                // reverse direction
        const int64_t time = mEntries[idx].first;
        const float energy = mEntries[idx].second;

        if (state == AT_END) {
            if (energy == 0.f) {
                ALOGV("two zeroes detected");
                break; // normally single zero terminated - two zeroes means no more data.
            }
            state = IN_SIGNAL;
        } else { // IN_SIGNAL
            if (energy == 0.f) {
                if (column != 0) {
                    column = 0;
                    ++count;
                }
                state = AT_END;
                continue;
            }
        }
        if (column == 0 && time < limitNs) {
            break;
        }
        ++nonzeros;
        if (++column == maxColumns) {
            column = 0;
            // TODO ideally we would peek the previous entry to see if it is 0
            // to ensure we properly put in a starting signal bracket.
            // We don't do that because it would complicate the logic here.
            ++count;
        }
    }
    if (offset > 0) {
        --offset;
    }
    // We accumulate the log info into a string, and write to the fd once.
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    // ss << std::scientific;
    if (nonzeros == 0) {
        ss << prefix << "Signal power history: (none)\n";
    } else {
        // First value is power, second value is whether value is start of
        // a new time stamp.
        std::vector<std::pair<float, bool>> plotEntries;
        const float timeResolution = mFramesPerEntry * 1000.f / mSampleRate;
        ss << prefix << "Signal power history (resolution: " << timeResolution << " ms):\n";

        size_t column = 0;
        bool first = true;
        bool start = false;
        float cumulative = 0.f;
        for (; offset >= 0; --offset) {
            const size_t idx = (mIdx + numberOfEntries - offset - 1) % numberOfEntries;
            const int64_t time = mEntries[idx].first;
            const float energy = mEntries[idx].second;

            if (energy == 0.f) {
                if (!first) {
                    ss << " ] sum(" << audio_utils_power_from_energy(cumulative) << ")";
                    // Add an entry to denote the start of a new time stamp series.
                    if (!plotEntries.empty()) {
                        // First value should be between min and max of all graph entries
                        // so that it doesn't mess with y-axis scaling.
                        plotEntries.emplace_back(plotEntries.back().first, true);
                    }
                }
                cumulative = 0.f;
                column = 0;
                start = true;
                continue;
            }
            if (column == 0) {
                // print time if at start of column
                if (!first) {
                    ss << "\n";
                }
                ss << prefix << " " << audio_utils_time_string_from_ns(time).time
                        << (start ? ": [ ": ":   ");
                first = false;
                start = false;
            }  else {
                ss << " ";
            }
            if (++column >= maxColumns) {
                column = 0;
            }

            cumulative += energy;
            // convert energy to power and print
            const float power =
                    audio_utils_power_from_energy(energy / (mChannelCount * mFramesPerEntry));
            ss << std::setw(6) << power;
            ALOGV("state: %d %lld %f", state, (long long)time, power);
            // Add an entry to the ASCII art power log graph.
            // false indicates the value doesn't have a new series time stamp.
            plotEntries.emplace_back(power, false);
        }
        if (logPlot) {
            ss << "\n" << audio_utils_log_plot(plotEntries.begin(), plotEntries.end());
        }
        ss << "\n";
    }
    return ss.str();
}

void PowerLogBase::flushEntry() {
    // We store the data as normalized energy per sample. The energy sequence is
    // zero terminated. Consecutive zero entries are ignored.
    if (mCurrentEnergy == 0.f) {
        if (mConsecutiveZeroes++ == 0) {
            mEntries[mIdx++] = std::make_pair(mCurrentTime, 0.f);
            // zero terminate the signal sequence.
        }
    } else {
        mConsecutiveZeroes = 0;
        mEntries[mIdx++] = std::make_pair(mCurrentTime, mCurrentEnergy);
        ALOGV("writing %lld %f", (long long)mCurrentTime, mCurrentEnergy);
    }
    if (mIdx >= mEntries.size()) {
        mIdx -= mEntries.size();
    }
    mCurrentTime = 0;
    mCurrentEnergy = 0;
    mCurrentFrames = 0;
}

void PowerLog::log(const void *buffer, size_t frames, int64_t nowNs) {
    if (frames == 0) return;
    std::lock_guard <std::mutex> guard(mMutex);

    const size_t bytes_per_sample = audio_bytes_per_sample(mFormat);
    while (true) {
        // limit the number of frames to process from the requirements
        // of each log base.
        size_t processFrames = mBase[0]->framesToProcess(frames);
        for (size_t i = 1; i < std::size(mBase); ++i) {
            processFrames = std::min(processFrames, mBase[i]->framesToProcess(frames));
        }
        const float energy = audio_utils_compute_energy_mono(buffer, mFormat,
                                                             processFrames * mChannelCount);
        for (const auto& base : mBase) {
            base->processEnergy(processFrames, energy, nowNs);
        }
        frames -= processFrames;
        if (frames == 0) return;
        buffer = (const uint8_t *) buffer + processFrames * mChannelCount * bytes_per_sample;
        nowNs += processFrames * NANOS_PER_SECOND / mSampleRate;
    }
}

std::string PowerLog::dumpToString(
        const char *prefix, size_t lines, int64_t limitNs, bool logPlot) const
{
    // Determine how to distribute lines among the logs.
    const size_t logs = mBase.size();
    std::vector<size_t> sublines(logs);
    size_t start = 0;

    if (lines > 0) {
        // we compute the # of lines per PowerLogBase starting from
        // largest time granularity / resolution to the finest resolution.
        //
        // The largest granularity has the fewest lines, doubling
        // as the granularity gets finer.
        // The finest 2 levels have identical number of lines.
        size_t norm = 1 << (logs - 1);
        if (logs > 2) norm += (1 << (logs - 2)) - 1;
        size_t alloc = 0;
        for (size_t i = 0; i < logs - 1; ++i) {
            const size_t l = (1 << i) * lines / norm;
            if (l == 0) {
                start = i + 1;
            } else {
                sublines[i] = l;
                alloc += l;
            }
        }
        sublines[logs - 1] = lines - alloc;
    }

    // Our PowerLogBase vector is stored from finest granularity / resolution to largest
    // granularity.  We dump the logs in reverse order (logs - 1 - "index").
    std::string s = mBase[logs - 1 - start]->dumpToString(
            prefix, sublines[start], limitNs, start == logs - 1 ? logPlot : false);
    for (size_t i = start + 1; i < logs; ++i) {
        s.append(mBase[logs - 1 - i]->dumpToString(
                prefix, sublines[i], limitNs, i == logs - 1 ? logPlot : false));
    }
    return s;
}

status_t PowerLog::dump(
        int fd, const char *prefix, size_t lines, int64_t limitNs, bool logPlot) const
{
    // Since dumpToString and write are thread safe, this function
    // is conceptually thread-safe but simultaneous calls to dump
    // by different threads to the same file descriptor may not write
    // the two logs in time order.
    const std::string s = dumpToString(prefix, lines, limitNs, logPlot);
    if (s.size() > 0 && write(fd, s.c_str(), s.size()) < 0) {
        return -errno;
    }
    return NO_ERROR;
}

} // namespace android

using namespace android;

power_log_t *power_log_create(uint32_t sample_rate,
        uint32_t channel_count, audio_format_t format, size_t entries, size_t frames_per_entry)
{
    if (!audio_utils_is_compute_power_format_supported(format)) {
        return nullptr;
    }
    return reinterpret_cast<power_log_t *>
            (new(std::nothrow)
                    PowerLog(sample_rate, channel_count, format, entries, frames_per_entry));
}

void power_log_log(power_log_t *power_log,
        const void *buffer, size_t frames, int64_t now_ns)
{
    if (power_log == nullptr) {
        return;
    }
    reinterpret_cast<PowerLog *>(power_log)->log(buffer, frames, now_ns);
}

int power_log_dump(
        power_log_t *power_log, int fd, const char *prefix, size_t lines, int64_t limit_ns)
{
    if (power_log == nullptr) {
        return BAD_VALUE;
    }
    return reinterpret_cast<PowerLog *>(power_log)->dump(fd, prefix, lines, limit_ns);
}

void power_log_destroy(power_log_t *power_log)
{
    delete reinterpret_cast<PowerLog *>(power_log);
}
