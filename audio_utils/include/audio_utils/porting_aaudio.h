#pragma once
#include <cstdint>
#include <audio_utils/libaudioutils_export.h>

#ifndef DONOT_USE_PORTING_AAUDIO

enum AAUDIO_FORMAT
{
    AAUDIO_FORMAT_PCM_FLOAT
};

enum
{
    AAUDIO_SESSION_ID_ALLOCATE
};

enum
{
    AAUDIO_PERFORMANCE_MODE_LOW_LATENCY
};

enum
{
    AAUDIO_OK,
    /**
    * The audio device was disconnected. This could occur, for example, when headphones
    * are plugged in or unplugged. The stream cannot be used after the device is disconnected.
    * Applications should stop and close the stream.
    * If this error is received in an error callback then another thread should be
    * used to stop and close the stream.
    */
    AAUDIO_ERROR_DISCONNECTED,
};

struct AAudioStreamBuidlerParameters;

struct AAudioStream;

struct AAudioStreamBuilder;

using aaudio_result_t = uint32_t;

typedef void ( *ErrorCallbackType )( AAudioStream* stream, void* userdata, aaudio_result_t error );

LIBAUDIOUTILS_EXPORT aaudio_result_t AAudio_createStreamBuilder( AAudioStreamBuilder** a_out );

LIBAUDIOUTILS_EXPORT void AAudioStreamBuilder_setSampleRate( AAudioStreamBuilder*, int );

LIBAUDIOUTILS_EXPORT void AAudioStreamBuilder_setFormat( AAudioStreamBuilder*, AAUDIO_FORMAT a_format );

LIBAUDIOUTILS_EXPORT void AAudioStreamBuilder_setChannelCount( AAudioStreamBuilder*, int );

LIBAUDIOUTILS_EXPORT void AAudioStreamBuilder_setSessionId( AAudioStreamBuilder*, int );

LIBAUDIOUTILS_EXPORT void AAudioStreamBuilder_setPerformanceMode( AAudioStreamBuilder*, int );

LIBAUDIOUTILS_EXPORT aaudio_result_t AAudioStreamBuilder_openStream( AAudioStreamBuilder*, AAudioStream** );

LIBAUDIOUTILS_EXPORT void AAudioStreamBuilder_delete( AAudioStreamBuilder* );

LIBAUDIOUTILS_EXPORT int AAudioStream_getBufferSizeInFrames( AAudioStream* );

LIBAUDIOUTILS_EXPORT void AAudioStream_requestStart( AAudioStream* );

LIBAUDIOUTILS_EXPORT void AAudioStream_requestStop( AAudioStream* );

LIBAUDIOUTILS_EXPORT void AAudioStream_close( AAudioStream* );

LIBAUDIOUTILS_EXPORT void AAudioStream_requestPause( AAudioStream* );

LIBAUDIOUTILS_EXPORT void AAudioStream_requestFlush( AAudioStream* );

LIBAUDIOUTILS_EXPORT void AAudioStreamBuilder_setErrorCallback( AAudioStreamBuilder*, ErrorCallbackType, void* );

/**
 * Write data to the stream.
 *
 * The call will wait until the write is complete or until it runs out of time.
 * If timeoutNanos is zero then this call will not wait.
 *
 * Note that timeoutNanoseconds is a relative duration in wall clock time.
 * Time will not stop if the thread is asleep.
 * So it will be implemented using CLOCK_BOOTTIME.
 *
 * This call is "strong non-blocking" unless it has to wait for room in the buffer.
 *
 * If the call times out then zero or a partial frame count will be returned.
 *
 * Available since API level 26.
 *
 * @param stream A stream created using AAudioStreamBuilder_openStream().
 * @param buffer The address of the first sample.
 * @param numFrames Number of frames to write. Only complete frames will be written.
 * @param timeoutNanoseconds Maximum number of nanoseconds to wait for completion.
 * @return The number of frames actually written or a negative error.
 */
LIBAUDIOUTILS_EXPORT aaudio_result_t AAudioStream_write( AAudioStream* stream,
                                    const void* buffer, int32_t numFrames, int64_t timeoutNanoseconds );

#endif

