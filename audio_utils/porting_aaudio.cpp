#include "audio_utils/porting_aaudio.h"

#ifndef DONOT_USE_PORTING_AAUDIO

struct AAudioStreamBuidlerParameters
{
    int m_sampleRate = 0;
    AAUDIO_FORMAT m_format = AAUDIO_FORMAT_PCM_FLOAT;
    int m_channelCount = 0;
    int m_sessionId = 0;
    int m_performanceMode = 0;
};

struct AAudioStream
{
    AAudioStreamBuidlerParameters m_parameters;
};

struct AAudioStreamBuilder
{
    AAudioStreamBuidlerParameters m_parameters;
};

void TestFunctionForDebuger()
{

}

aaudio_result_t AAudio_createStreamBuilder( AAudioStreamBuilder** a_out )
{
    *a_out = new AAudioStreamBuilder;
    return AAUDIO_OK;
}

void AAudioStreamBuilder_setSampleRate( AAudioStreamBuilder* a_out, int a_sampleRate )
{
    a_out->m_parameters.m_sampleRate = a_sampleRate;
}

void AAudioStreamBuilder_setFormat( AAudioStreamBuilder* a_out, AAUDIO_FORMAT a_format )
{
    a_out->m_parameters.m_format = a_format;
}

void AAudioStreamBuilder_setChannelCount( AAudioStreamBuilder* a_out, int a_channelCount )
{
    a_out->m_parameters.m_channelCount = a_channelCount;
}

void AAudioStreamBuilder_setSessionId( AAudioStreamBuilder* a_out, int a_id )
{
    a_out->m_parameters.m_sessionId = a_id;
}

void AAudioStreamBuilder_setPerformanceMode( AAudioStreamBuilder* a_out, int a_mode )
{
    a_out->m_parameters.m_performanceMode = a_mode;
}

aaudio_result_t AAudioStreamBuilder_openStream( AAudioStreamBuilder* a_out, AAudioStream** a_stream )
{
    *a_stream = new AAudioStream;

    AAudioStream* p = *a_stream;
    p->m_parameters = a_out->m_parameters;
#ifndef _MSC_VER
    SampleFormat sampleFormat = SampleFormat::FormatFloat32LE;
    switch( p->m_parameters.m_format )
    {
    case AAUDIO_FORMAT_PCM_FLOAT:
    sampleFormat = SampleFormat::FormatFloat32LE;
    break;
    default:
    break;
    }

    int channelMask = p->m_parameters.m_channelCount;
    if( channelMask > 2 )
    {
        channelMask = 2;
    }
    PcmPlayer::GetInstance().SetPcmParameters( p->m_parameters.m_sampleRate, channelMask, sampleFormat );
#endif
    return AAUDIO_OK;
}

void AAudioStreamBuilder_delete( AAudioStreamBuilder* a_out )
{
    delete a_out;
}

int AAudioStream_getBufferSizeInFrames( AAudioStream* a_out )
{
    int ret = 4096;
    return ret;
}

void AAudioStream_requestStart( AAudioStream* )
{

}

void AAudioStream_requestStop( AAudioStream* )
{

}

void AAudioStream_close( AAudioStream* )
{

}

void AAudioStream_requestPause( AAudioStream* )
{

}

void AAudioStream_requestFlush( AAudioStream* )
{

}

aaudio_result_t AAudioStream_write( AAudioStream* stream,
                                    const void* buffer, int32_t numFrames, int64_t timeoutNanoseconds )
{
    unsigned char* p = nullptr;
    p = const_cast<uint8_t*>( reinterpret_cast<const uint8_t*>( buffer ) );
#ifndef _MSC_VER
    PcmPlayer::GetInstance().PlayPcmFrames( p, numFrames );
#endif
    return AAUDIO_OK;
}

void AAudioStreamBuilder_setErrorCallback( AAudioStreamBuilder* aBuilder, ErrorCallbackType aCallback, void* aUserData )
{

}

aaudio_session_id_t AAudioStream_getSessionId( AAudioStream* stream )
{
    return AAUDIO_SESSION_ID_ALLOCATE;
}

#endif
