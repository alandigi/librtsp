#pragma once
#include "MediaSource.h"

class FileDemux; 
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#ifdef __cplusplus
}
#endif
class DemuxSource :
    public MediaSource
{
public:
    DemuxSource(int streamindex);
    virtual ~DemuxSource(void);

public:
    virtual const string GetConfig() = 0;
    virtual bool         Open() = 0;  //!< Open the source
    virtual bool         Close() = 0; //!< Close the source
    virtual int          GetMediaData( char *buf,
                                       unsigned int bufsize,
                                       unsigned long &duration,
                                       unsigned long &ts) = 0;

    void SetMediaDemux(FileDemux *demux);
    CodecID GetCodecID();

protected:
    int       streamIndex_;
    FileDemux *theMediaDemux_;
    int       frameCount_;
    
};
