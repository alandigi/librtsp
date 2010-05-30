#include "MediaDemux.h"
#include "FileDemux.h"
#include "MPEG4DemuxSource.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
}
#endif
#include <cassert>
#include "util.h"
MPEG4DemuxSource::MPEG4DemuxSource(int streamindex)
 : DemuxSource(streamindex)
{
}

MPEG4DemuxSource::~MPEG4DemuxSource(void)
{
}

const string MPEG4DemuxSource::GetConfig()
{
    return theMediaDemux_->GetConfigByStreamIndex(streamIndex_, FileDemux::STREAM_VIDEO);
}

bool MPEG4DemuxSource::Open()
{
    return true; // default nothing to do, because the stream has already opened in FileDemux
}

bool MPEG4DemuxSource::Close()
{
    return true; // default nothing to do, because the stream will be closed in FileDemux
}

int MPEG4DemuxSource::GetMediaData( char *buf,
                                    unsigned int bufsize,
                                    unsigned long &duration,
                                    unsigned long &ts )
{
    AVPacket packet;
    AVCodecContext *codecCtx;
    theMediaDemux_->GetCodecCtxByStreamIndex(streamIndex_, &codecCtx);
    assert(codecCtx);
    theMediaDemux_->GetPacketByStreamIndex(streamIndex_, packet, true);
    unsigned int pktsize = packet.size;    if (bufsize < pktsize )
    {
        return -2;
    }

    ::memcpy(buf, packet.data, pktsize);
    duration = av_q2d(codecCtx->time_base) * 1000000;
    //int fps = codecCtx->time_base.den;

    ts = packet.dts;
    //RTSPDEBUG("fps:%d, dts:%d, ts:%d", fps,packet.dts, ts);
    av_free_packet(&packet);
    return pktsize;
}
