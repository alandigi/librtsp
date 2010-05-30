/*!@file MPEG4FileSource.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.24
@version 1.0
*/
#include "rtspCommon.h"
#include "MediaSource.h"
#include "MPEG4FileSource.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/internal.h>
#ifdef __cplusplus
}
#endif


MPEG4FileSource::MPEG4FileSource(string filename)
 : FileSource(filename)
{
}

MPEG4FileSource::~MPEG4FileSource(void)
{
}

bool MPEG4FileSource::Open()
{
    int ret;
    ret = av_open_input_file(&formatCtx_, filename_.c_str(), NULL, 0, NULL);
    if(ret)    {        return false; // Couldn't open file    }    ret = av_find_stream_info(formatCtx_);
    // Retrieve stream information
    if( ret < 0 )    {
        return false; // Couldn't find stream information    }    videoStreamIndex_ = -1;
    for (unsigned int i = 0; i < formatCtx_->nb_streams; ++i )
    {
        if ( formatCtx_->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
        {
            videoStreamIndex_ = i;
            break;
        }
    }    if ( -1 == videoStreamIndex_ )
    {
        return false;
    }
    // Get a pointer to the codec context for the audio stream
    codecCtx_ = formatCtx_->streams[videoStreamIndex_]->codec;    //AVStream    if ( NULL == codecCtx_ )
    {
        return false;
    }
    fps_ = formatCtx_->streams[videoStreamIndex_]->r_frame_rate.num;
    if ( 0 == fps_ )
    {
        return false;
    }
    return true;
}

bool MPEG4FileSource::Close()
{
    av_free(formatCtx_);
    return false;
}

int MPEG4FileSource::GetMediaData( char *buf,
                                   unsigned int bufsize,
                                   unsigned long &duration,
                                   unsigned long &ts)
{
//     Compute "fDurationInMicroseconds" now:
//     fDurationInMicroseconds
//         = (fFrameRate == 0.0 || ((int)fPictureCount) < 0) ? 0
//         : (unsigned)((fPictureCount*1000000)/fFrameRate);
    AVPacket packet;
    int ret = av_read_frame(formatCtx_, &packet);    if (ret < 0)
    {
        return -1;
    }    unsigned int pktsize = packet.size;    if (bufsize < pktsize )
    {
        return -2;
    }

    ::memcpy(buf, packet.data, pktsize);
    duration = av_q2d(codecCtx_->time_base) * 1000000;
    ts = packet.dts /80000 * 90000 / fps_;
    av_free_packet(&packet);
    return pktsize;
}

const string MPEG4FileSource::GetConfig()
{
    if (codecCtx_ && codecCtx_->extradata_size )
    {
        char *buf = new char[codecCtx_->extradata_size * 2];
        string mpeg4config;
        ff_data_to_hex(buf, codecCtx_->extradata, codecCtx_->extradata_size);
        mpeg4config.assign(buf, codecCtx_->extradata_size * 2);
        delete [] buf;
        return mpeg4config;
    }
    return "";
}
