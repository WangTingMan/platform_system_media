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

#ifndef ANDROID_AUDIO_POWER_H
#define ANDROID_AUDIO_POWER_H

#include <math.h>
#include <stdint.h>
#ifndef _MSC_VER
#include <sys/cdefs.h>
#endif
#include <system/audio.h>

#include <audio_utils/libaudioutils_export.h>

#ifdef __cplusplus
#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS extern "C" {
#endif
#else
#define __BEGIN_DECLS
#endif

#ifdef __cplusplus
#ifndef __END_DECLS
#define __END_DECLS }
#endif
#else
#define __END_DECLS
#endif

/** \cond */
__BEGIN_DECLS
/** \endcond */

/**
 * \brief Compute signal power on a scale of 0 dBFS.
 *
 *   \param buffer       buffer of samples.
 *   \param format       one of AUDIO_FORMAT_PCM_8_BIT, AUDIO_FORMAT_PCM_16_BIT,
 *                       AUDIO_FORMAT_PCM_24_BIT_PACKED, AUDIO_FORMAT_PCM_8_24_BIT,
 *                       AUDIO_FORMAT_PCM_32_BIT, AUDIO_FORMAT_PCM_FLOAT.
 *   \param samples      number of samples in buffer.  This is not audio frames;
 *                       usually the number of samples is the number of audio frames
 *                       multiplied by channel count.
 *
 * \return
 *   signal power of the samples in the buffer. It is possible to return negative infinity
 *   if the power is zero.
 */

LIBAUDIOUTILS_EXPORT float audio_utils_compute_power_mono(const void *buffer, audio_format_t format, size_t samples);

/**
 * \brief Compute signal energy (sum of squared amplitudes).
 *
 *   \param buffer       buffer of samples.
 *   \param format       one of AUDIO_FORMAT_PCM_8_BIT, AUDIO_FORMAT_PCM_16_BIT,
 *                       AUDIO_FORMAT_PCM_24_BIT_PACKED, AUDIO_FORMAT_PCM_8_24_BIT,
 *                       AUDIO_FORMAT_PCM_32_BIT, AUDIO_FORMAT_PCM_FLOAT.
 *   \param samples      number of samples in buffer.  This is not audio frames;
 *                       usually the number of samples is the number of audio frames
 *                       multiplied by channel count.
 *
 * \return
 *   signal energy of the samples in the buffer (sum of squares) where each sample is
 *   normalized to peak to peak range of 1.f.
 */

LIBAUDIOUTILS_EXPORT float audio_utils_compute_energy_mono(const void *buffer, audio_format_t format, size_t samples);

/**
 * \brief Compute for each channel signal energy (sum of squared amplitudes).
 *
 *   \param buffer       buffer of samples.
 *   \param format       one of AUDIO_FORMAT_PCM_8_BIT, AUDIO_FORMAT_PCM_16_BIT,
 *                       AUDIO_FORMAT_PCM_24_BIT_PACKED, AUDIO_FORMAT_PCM_8_24_BIT,
 *                       AUDIO_FORMAT_PCM_32_BIT, AUDIO_FORMAT_PCM_FLOAT.
 *   \param samples      number of samples in buffer.  This is not audio frames;
 *                       usually the number of samples is the number of audio frames
 *                       multiplied by channel count.
 *   \param numChannels  the number of channels for which the energy is computed.
 *   \param out          interleaved buffer containing for each channel the sample
 *                       energy. Must be initialized with zero values or pre-existing
 *                       energy to accumulate to.
 *
 * \return
 *   out array is updated by adding for each channel the signal energy of the samples
 *   in the buffer (sum of squares).
 */
LIBAUDIOUTILS_EXPORT void audio_utils_accumulate_energy(const void* buffer,
                                   audio_format_t format,
                                   size_t samples,
                                   size_t numChannels,
                                   float* out);

/**
 * \brief  Returns true if the format is supported for compute_energy_for_mono()
 *         and compute_power_for_mono().
 * \param  format        format under consideration.
 * \return true if supported.
 */
LIBAUDIOUTILS_EXPORT bool audio_utils_is_compute_power_format_supported(audio_format_t format);

/**
 * \brief  Returns the signal power from amplitude.
 * \param  amplitude the signal amplitude. A negative amplitude is treated
 *                   the same as a positive amplitude.
 * \return signal power in dB. It is possible to return negative infinity
 *         if the input is zero.
 */
static inline float audio_utils_power_from_amplitude(float amplitude)
{
    return 20.f * log10f(fabsf(amplitude));
}

/**
 * \brief  Returns the signal power from energy.
 * \param  energy the signal energy. This should be non-negative.
 * \return signal power in dB. It is possible to return NaN if the input is
 *         negative, or negative infinity if the input is zero.
 */
static inline float audio_utils_power_from_energy(float energy)
{
    return 10.f * log10f(energy);
}

/** \cond */
__END_DECLS
/** \endcond */

#endif // !ANDROID_AUDIO_POWER_H
