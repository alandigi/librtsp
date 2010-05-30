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

class H264FileSource :
    public FileSource
{
public:
    H264FileSource(string filename);
    ~H264FileSource(void);
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
    unsigned long    frameCount_;

};
