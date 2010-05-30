
#include "MediaDemux.h"
#include "FileDemux.h"
#include "DemuxSource.h"
#include <cassert>

DemuxSource::DemuxSource(int streamindex)
 : streamIndex_(streamindex)
{
    frameCount_ = 0;
}

DemuxSource::~DemuxSource(void)
{
}

void DemuxSource::SetMediaDemux( FileDemux *demux )
{
    assert(demux);
    theMediaDemux_ = demux;
}

CodecID DemuxSource::GetCodecID()
{
    return theMediaDemux_->GetCodecIDByStreamIndex(streamIndex_);
}
