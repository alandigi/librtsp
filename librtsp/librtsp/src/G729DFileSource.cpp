
#include "rtspCommon.h"
#include "MediaSource.h"
#include "G729DFileSource.h"
#include <cstring>
#include <climits>
#include "util.h"

G729DFileSource::G729DFileSource(string filename)
 : FileSource(filename)
 , g729dfp_(NULL)
{
    framecount_ = 0;
}

G729DFileSource::~G729DFileSource(void)
{
}

bool G729DFileSource::Open()
{
    g729dfp_ = fopen(filename_.c_str(), "rb");
    if ( NULL == g729dfp_ )
    {
        RTSPDEBUG("Error in open file %s", filename_.c_str());
        return false;
    }
    return true;
}

bool G729DFileSource::Close()
{
    if ( NULL != g729dfp_ )
    {
        fclose(g729dfp_);
    }
    return true;
}

int G729DFileSource::GetMediaData( char *buf,
                                   unsigned int bufsize,
                                   unsigned long &duration,
                                   unsigned long &ts )
{
    int ret;
    if ( bufsize < G729D_FRAME_SIZE )
    {
        return -2;
    }
    ret = fread(buf, sizeof(char), G729D_FRAME_SIZE, g729dfp_);
    if ( G729D_FRAME_SIZE != ret )
    {
        return -1;
    }
    duration = 10 * 1000; // 10 ms
    ts = framecount_ * 80; // (10 * 8000 / 1000 = 80 )
    return G729D_FRAME_SIZE;
}

const string G729DFileSource::GetConfig()
{
    return "";
}
