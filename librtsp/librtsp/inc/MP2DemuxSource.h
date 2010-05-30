#pragma once
#include "DemuxSource.h"

class MP2DemuxSource :
    public DemuxSource
{
public:
    MP2DemuxSource(int streamindex);
    virtual ~MP2DemuxSource(void);

    virtual const string GetConfig();
    virtual bool         Open();  //!< Open the source
    virtual bool         Close(); //!< Close the source
    virtual int          GetMediaData( char *buf,
                                       unsigned int bufsize,
                                       unsigned long &duration,
                                       unsigned long &ts);
};
