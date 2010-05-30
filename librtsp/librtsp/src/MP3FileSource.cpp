/*!@file MP3FileSource.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.24
@version 1.0
*/
#include "rtspCommon.h"
#include "MediaSource.h"
#include "MP3FileSource.h"


MP3FileSource::MP3FileSource(string filename)
:FileSource(filename)
{
    formatCtx_ = 0;
    codecCtx_  = 0;
}

MP3FileSource::~MP3FileSource(void)
{
}

bool MP3FileSource::Open()
{
    int ret;
    ret = av_open_input_file(&formatCtx_, filename_.c_str(), NULL, 0, NULL);
    if(ret)    {        return false; // Couldn't open file    }    ret = av_find_stream_info(formatCtx_);
    // Retrieve stream information
    if( ret < 0 )    {
        return false; // Couldn't find stream information    }    int audio_stream_index = -1;
    for (unsigned int i = 0; i < formatCtx_->nb_streams; ++i )
    {
        if ( formatCtx_->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
        {
            audio_stream_index = i;
            break;
        }
    }
    // Get a pointer to the codec context for the audio stream
    codecCtx_ = formatCtx_->streams[audio_stream_index]->codec;    if ( NULL == codecCtx_ )
    {
        return false;
    }    return true;
}

bool MP3FileSource::Close()
{
    av_free(formatCtx_);
    return true;
}

int MP3FileSource::GetMediaData( char *buf,
                                 unsigned int bufsize,
                                 unsigned long &duration, // uS
                                 unsigned long &ts)
{
    AVPacket packet;
    int ret = av_read_frame(formatCtx_, &packet);    if (ret < 0)
    {
        return -1;
    }    unsigned int pktsize = packet.size;    if (bufsize < pktsize )
    {
        return -2;
    }

    ::memcpy(buf, packet.data, pktsize);
    unsigned samplerate = codecCtx_->sample_rate;
    unsigned int freq = samplerate * (1 + IsMPEG2((char *)packet.data, pktsize));
    /*In addition, the number of samples stored in an MP3 frame is constant, at 1152 samples per frame*/
    duration = (1152 * MILLION ) / freq; //unit:micro second
    ts = packet.pts;
    av_free_packet(&packet);
    return pktsize;
}

const string MP3FileSource::GetConfig()
{
    return "";
}

int MP3FileSource::IsMPEG2( char * data, int size )
{
    if (size < 4)
    {
        return 0;
    }
    unsigned int mpegHeader = (unsigned int)(data[0] << 24)
                            | (unsigned int)(data[1] << 16)
                            | (unsigned int)(data[2] << 8)
                            | (unsigned int)(data[3]);
    //fprintf(stderr, "mpeg header:0x%08x\n", mpegHeader);
    int isMpeg2 = 0;
    if (mpegHeader & (1<<20))
    {
        isMpeg2 = (mpegHeader & (1<<19)) ? 0x0 : 0x1;
    }
    else
    {
        isMpeg2 = 1;
    }
    return isMpeg2;
}
