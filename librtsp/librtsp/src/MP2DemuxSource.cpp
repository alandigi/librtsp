
#include "MediaDemux.h"
#include "FileDemux.h"
#include "MP2DemuxSource.h"
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
#include <cassert>
#include "rtspCommon.h"
#include "util.h"

MP2DemuxSource::MP2DemuxSource(int streamindex)
 : DemuxSource(streamindex)
{
}

MP2DemuxSource::~MP2DemuxSource(void)
{
}

const string MP2DemuxSource::GetConfig()
{
    return "";
}

bool MP2DemuxSource::Open()
{
    return true;
}

bool MP2DemuxSource::Close()
{
    return true;
}

int MP2DemuxSource::GetMediaData( char *buf,
                                  unsigned int bufsize,
                                  unsigned long &duration,
                                  unsigned long &ts )
{
    AVPacket packet;
    AVCodecContext *codecCtx;
    theMediaDemux_->GetPacketByStreamIndex(streamIndex_, packet, true);
    theMediaDemux_->GetCodecCtxByStreamIndex(streamIndex_, &codecCtx);
    assert(codecCtx);
    unsigned int pktsize = packet.size;    if (bufsize < pktsize )
    {
        return -2;
    }

    ::memcpy(buf, packet.data, pktsize);
    unsigned samplerate = codecCtx->sample_rate;
    unsigned int freq = samplerate * (1/* + IsMPEG2((char *)packet.data, pktsize)*/);
    /* The number of samples stored in an MP3 frame is constant, at 1152 samples per frame */
    duration = (1152 * MILLION ) / freq; //unit:micro second

    ts = packet.pts;
    int size = packet.size;
    //RTSPDEBUG("duration:%d, ts:%d", duration, ts);
    av_free_packet(&packet);
    return size;
}
