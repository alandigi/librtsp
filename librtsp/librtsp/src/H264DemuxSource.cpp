#include "MediaDemux.h"
#include "FileDemux.h"
#include "H264DemuxSource.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#ifdef __cplusplus
}
#endif
#include "rtsperrors.h"
#include "util.h"

H264DemuxSource::H264DemuxSource(int streamindex)
 : DemuxSource(streamindex)
{
}

H264DemuxSource::~H264DemuxSource(void)
{
}

bool H264DemuxSource::Open()
{
    return true;
}

bool H264DemuxSource::Close()
{
    return true;
}

int H264DemuxSource::GetMediaData( char *buf,
                                   unsigned int bufsize,
                                   unsigned long &duration,
                                   unsigned long &ts )
{
    AVPacket packet;
    AVCodecContext *codecCtx;
    theMediaDemux_->GetCodecCtxByStreamIndex(streamIndex_, &codecCtx);
    theMediaDemux_->GetPacketByStreamIndex(streamIndex_, packet, true);
    unsigned int pktsize = packet.size;    if (bufsize < pktsize )
    {
        return ERR_RTSP_NOT_ENOUGH_BUF;
    }

    ::memcpy(buf, packet.data, pktsize);
    //duration = av_q2d(codecCtx->time_base) * 1000000;
    int fps = 25;
    duration = (1.0 / fps) * 1000000;
    ts = frameCount_ * 90000 / fps;
    ++frameCount_;
    av_free_packet(&packet);
    return pktsize;
}

const string H264DemuxSource::GetConfig()
{
    return theMediaDemux_->GetConfigByStreamIndex(streamIndex_, FileDemux::STREAM_VIDEO);
}
