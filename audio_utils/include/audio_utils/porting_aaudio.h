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

/**
 * These may be used with AAudioStreamBuilder_setSessionId().
 *
 * Added in API level 28.
 */
enum
{
    /**
     * Do not allocate a session ID.
     * Effects cannot be used with this stream.
     * Default.
     *
     * Added in API level 28.
     */
    AAUDIO_SESSION_ID_NONE = -1,

    /**
     * Allocate a session ID that can be used to attach and control
     * effects using the Java AudioEffects API.
     * Note that using this may result in higher latency.
     *
     * Note that this matches the value of AudioManager.AUDIO_SESSION_ID_GENERATE.
     *
     * Added in API level 28.
     */
    AAUDIO_SESSION_ID_ALLOCATE = 0,
};
typedef int32_t aaudio_session_id_t;

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
 * Passes back the session ID associated with this stream.
 *
 * The session ID can be used to associate a stream with effects processors.
 * The effects are controlled using the Android AudioEffect Java API.
 *
 * If AAudioStreamBuilder_setSessionId() was
 * called with {@link #AAUDIO_SESSION_ID_ALLOCATE}
 * then a new session ID should be allocated once when the stream is opened.
 *
 * If AAudioStreamBuilder_setSessionId() was called with a previously allocated
 * session ID then that value should be returned.
 *
 * If AAudioStreamBuilder_setSessionId() was not called then this function should
 * return {@link #AAUDIO_SESSION_ID_NONE}.
 *
 * The sessionID for a stream should not change once the stream has been opened.
 *
 * Available since API level 28.
 *
 * @param stream reference provided by AAudioStreamBuilder_openStream()
 * @return session ID or {@link #AAUDIO_SESSION_ID_NONE}
 */
LIBAUDIOUTILS_EXPORT aaudio_session_id_t AAudioStream_getSessionId( AAudioStream* stream );

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

