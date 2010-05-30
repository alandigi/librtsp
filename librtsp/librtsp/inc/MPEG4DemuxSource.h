#pragma once
#include "DemuxSource.h"

class MPEG4DemuxSource :
    public DemuxSource
{
public:
    MPEG4DemuxSource(int streamindex);
    virtual ~MPEG4DemuxSource(void);

    virtual const string GetConfig();
    virtual bool         Open();  //!< Open the source
    virtual bool         Close(); //!< Close the source
    virtual int          GetMediaData( char *buf,
                                       unsigned int bufsize,
                                       unsigned long &duration,
                                       unsigned long &ts);
};
