
#include "rtspCommon.h"
#include "MediaSource.h"
#include "H264FileSource.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avc.h>
#include <libavutil/mem.h>
#include <libavutil/base64.h>
#ifdef __cplusplus
}
#endif
#include <cstring>
#include <climits>
#include "util.h"
#include "rtsperrors.h"

#define MAX_EXTRADATA_SIZE ((INT_MAX - 10) / 2)
#define MAX_PSET_SIZE 1024

H264FileSource::H264FileSource(string filename)
 : FileSource(filename)
{
    frameCount_ = 0;
}

H264FileSource::~H264FileSource(void)
{
}

bool H264FileSource::Open()
{
    int ret;
    ret = av_open_input_file(&formatCtx_, filename_.c_str(), NULL, 0, NULL);
    if( ret )    {        RTSPDEBUG("Error in av_open_input_file, open \"%s\"", filename_.data());        return false; // Couldn't open file    }    ret = av_find_stream_info(formatCtx_);
    // Retrieve stream information
    if( ret < 0 )    {
        RTSPDEBUG("av_find_stream_info Failed");
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
        RTSPDEBUG("Failed in find stream");
        return false;
    }
    // Get a pointer to the codec context for the audio stream
    codecCtx_ = formatCtx_->streams[videoStreamIndex_]->codec;    //AVStream    if ( NULL == codecCtx_ )
    {
        RTSPDEBUG("Failed in get codec");
        return false;
    }
    fps_ = formatCtx_->streams[videoStreamIndex_]->r_frame_rate.num;
    if ( 0 == fps_ )
    {
        RTSPDEBUG("Failed in get fps");
        return false;
    }
    return true;
}

bool H264FileSource::Close()
{
    av_free(formatCtx_);
    return false;
}

int H264FileSource::GetMediaData( char *buf,
                                  unsigned int bufsize,
                                  unsigned long &duration,
                                  unsigned long &ts )
{
    AVPacket packet;
    int ret = av_read_frame(formatCtx_, &packet);    if (ret < 0)
    {
        return ERR_RTSP_READ_FILE;
    }    unsigned int pktsize = packet.size;    if (bufsize < pktsize )
    {
        return ERR_RTSP_NOT_ENOUGH_BUF;
    }

    ::memcpy(buf, packet.data, pktsize);
    duration = av_q2d(codecCtx_->time_base) * 1000000;
    ts = frameCount_ * 90000 / fps_;
    ++frameCount_;
    av_free_packet(&packet);
    return pktsize;

}

const string H264FileSource::GetConfig()
{
    if ( NULL == codecCtx_ )
    {
        return "";
    }
    char *psets, *p;
    const uint8_t *r;
    const char *pset_string = "; sprop-parameter-sets=";

    if ( codecCtx_->extradata_size > MAX_EXTRADATA_SIZE)
    {
        RTSPDEBUG("Too much extradata!");
        return "";
    }
    psets = (char *)av_mallocz(MAX_PSET_SIZE);
    if (psets == NULL)
    {
        RTSPDEBUG("Cannot allocate memory for the parameter sets!");
        return "";;
    }
    memcpy(psets, pset_string, strlen(pset_string));
    p = psets + strlen(pset_string);
    r = ff_avc_find_startcode(codecCtx_->extradata, codecCtx_->extradata + codecCtx_->extradata_size);
    while (r < codecCtx_->extradata + codecCtx_->extradata_size) {
        const uint8_t *r1;

        while (!*(r++));
        r1 = ff_avc_find_startcode( r, codecCtx_->extradata + codecCtx_->extradata_size );
        if ( p != (psets + strlen(pset_string)) )
        {
            *p = ',';
            p++;
        }
        if ( NULL == av_base64_encode(p, MAX_PSET_SIZE - (p - psets), r, r1 - r) )
        {
            RTSPDEBUG("Cannot Base64-encode %td %td!", MAX_PSET_SIZE - (p - psets), r1 - r);
            av_free(psets);
            return "";
        }
        p += strlen(p);
        r = r1;
    }
    return string(psets);
}
