#pragma once
#include "DemuxSource.h"

class H264DemuxSource :
    public DemuxSource
{
public:
    H264DemuxSource(int streamindex);
    virtual ~H264DemuxSource(void);

    //! @brief Open the source, you can put initialization code here
    /**
    @return: bool
    */
    virtual bool Open();


    //! @brief Close the source, you can put de-initialization code here
    /**
    @return: bool
    */
    virtual bool Close();


    //! @brief Get Source Data
    /**
    @param:  char * buf  where to store the data
    @param:  unsigned int bufsize got size
    @param:  unsigned long & duration the play duration of the data got, unit is microsecond
    @param:  unsigned long & ts timestamp
    @return: int
    */
    virtual int  GetMediaData(char *buf,
        unsigned int bufsize,
        unsigned long &duration,
        unsigned long &ts);

    virtual const string GetConfig();
};
