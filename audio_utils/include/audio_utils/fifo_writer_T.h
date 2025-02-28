/*
 * Copyright (C) 2017 The Android Open Source Project
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

#ifndef ANDROID_AUDIO_FIFO_WRITER32_H
#define ANDROID_AUDIO_FIFO_WRITER32_H

#include <audio_utils/fifo.h>

/**
 * Optimized FIFO writer for small multiples of fixed-sized POD such as primitives.
 *
 * Has these restrictions compared to the ordinary FIFO writer:
 *  - buffer must be aligned on an appropriate boundary for T
 *  - frame size must be sizeof(T)
 *  - capacity must be power-of-2
 *  - effective size must be equal to capacity
 *  - no support for throttling of writer by one reader, and thus no blocking writes
 *  - does not implement the provider interface
 *  - does not implement the ordinary writer interface
 *  - does not unblock a reader
 *  - return value from write methods is void
 *  - no implied store-release; must be done explicitly
 *  - may not be combined with ordinary writer
 *
 * Usage:
 *  - construct an ordinary FIFO that follows the restrictions above
 *  - construct an ordinary reader based on that FIFO
 *  - construct a writer_T using the FIFO
 *  - use a sequence of write and write1, followed by storeSingleThreaded or storeRelease to commit
 */
template <typename T>
class audio_utils_fifo_writer_T /* : public audio_utils_fifo_provider */ {

public:
    /**
     * Construct a writer_T from a FIFO.
     */
    explicit audio_utils_fifo_writer_T(audio_utils_fifo& fifo);
    /*virtual*/ ~audio_utils_fifo_writer_T();

    /**
     * Write an array of T to FIFO.
     * If count is larger than capacity, then only the initial 'capacity' frames will be written.
     * TODO Instead of a silent truncation, consider adding a size_t or ssize_t return value
     * to indicate the actual transfer count.
     */
    void write(const T *buffer, uint32_t count /* FIXME size_t in writer */);

    /**
     * Write one T value to FIFO.
     */
    void write1(const T& value)
            /*__attribute__((no_sanitize("integer")))*/     // mLocalRear ++ can wrap
    {
        mBuffer[mLocalRear++ & (mFrameCountP2 - 1)] = value;
    }

    /**
     * Commit all previous write and write1 so that they are observable by reader(s),
     * with a simple non-atomic memory write.
     */
    void storeSingleThreaded() {
        mWriterRear.storeSingleThreaded(mLocalRear);
    }

    /**
     * Commit all previous write and write1 so that they are observable by reader(s),
     * with memory order 'release'.
     */
    void storeRelease() {
        mWriterRear.storeRelease(mLocalRear);
    }

private:
    // Accessed by writer only using ordinary operations
    uint32_t    mLocalRear; // frame index of next frame slot available to write, or write index

    // These fields are copied from fifo for better performance (avoids an extra de-reference)
    const uint32_t          mFrameCountP2;
    T * const               mBuffer;
    audio_utils_fifo_index& mWriterRear;
};

using audio_utils_fifo_writer32 = audio_utils_fifo_writer_T<int32_t>;
using audio_utils_fifo_writer64 = audio_utils_fifo_writer_T<int64_t>;

#endif // ANDROID_AUDIO_FIFO_WRITER32_H
