/*!@file MPEG4FileSource.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.24
@version 1.0
*/
#pragma once
#include "FileSource.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
}
#endif

class MPEG4FileSource :
    public FileSource
{
public:
    MPEG4FileSource(string filename);
    ~MPEG4FileSource(void);

public:
    virtual bool Open();  //!< Open the source
    virtual bool Close(); //!< Close the source
    virtual int  GetMediaData(char *buf,
                              unsigned int bufsize,
                              unsigned long &duration,
                              unsigned long &ts);
    virtual const string GetConfig();

private:
    AVFormatContext  *formatCtx_;
    AVCodecContext   *codecCtx_;
    int              videoStreamIndex_;
    int              fps_;
};
