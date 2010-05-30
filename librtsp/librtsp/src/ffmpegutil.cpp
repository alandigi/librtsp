
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avc.h>
#include <libavutil/mem.h>
#include <libavutil/base64.h>
#include <libavformat/internal.h>
#include <libavutil/avstring.h>
#ifdef __cplusplus
}
#endif
#include "ffmpegutil.h"
#include <cassert>
#include <string>
using std::string;
#if 0
static char *extradata2config(AVCodecContext *c)
{
    char *config;

    if (c->extradata_size > MAX_EXTRADATA_SIZE)
    {
        av_log(c, AV_LOG_ERROR, "Too much extradata!\n");
        return NULL;
    }
    config = (char *)av_malloc(10 + c->extradata_size * 2);
    if (config == NULL)
    {
        av_log(c, AV_LOG_ERROR, "Cannot allocate memory for the config info.\n");
        return NULL;
    }
    memcpy(config, "; config=", 9);
    ff_data_to_hex(config + 9, c->extradata, c->extradata_size);
    config[9 + c->extradata_size * 2] = 0;

    return config;
}
#endif

static char *extradata2psets(AVCodecContext *c)
{
    char *psets, *p;
    const uint8_t *r;
    const char *pset_string = "; sprop-parameter-sets=";

    if (c->extradata_size > MAX_EXTRADATA_SIZE)
    {
        av_log(c, AV_LOG_ERROR, "Too much extradata!\n");

        return NULL;
    }

    psets = (char *)av_mallocz(MAX_PSET_SIZE);
    if (psets == NULL)
    {
        av_log(c, AV_LOG_ERROR, "Cannot allocate memory for the parameter sets.\n");
        return NULL;
    }
    memcpy(psets, pset_string, strlen(pset_string));
    p = psets + strlen(pset_string);
    r = ff_avc_find_startcode(c->extradata, c->extradata + c->extradata_size);
    while (r < c->extradata + c->extradata_size)
    {
        const uint8_t *r1;

        while (!*(r++));
        r1 = ff_avc_find_startcode(r, c->extradata + c->extradata_size);
        if (p != (psets + strlen(pset_string)))
        {
            *p = ',';
            p++;
        }
        if (av_base64_encode(p, MAX_PSET_SIZE - (p - psets), r, r1 - r) == NULL)
        {
            av_log(c, AV_LOG_ERROR, "Cannot Base64-encode %td %td!\n", MAX_PSET_SIZE - (p - psets), r1 - r);
            av_free(psets);

            return NULL;
        }
        p += strlen(p);
        r = r1;
    }

    return psets;
}

std::string GetConfigByCodecCtx( AVCodecContext *codecCtx )
{
    std::string config;
    assert(codecCtx);
    switch (codecCtx->codec_id)
    {
    case CODEC_ID_H264:
        {
            config.assign(extradata2psets(codecCtx));
            break;
        }
    case CODEC_ID_MPEG4:
        {
            if ( codecCtx->extradata_size )
            {
                char *buf = new char[codecCtx->extradata_size * 2];
                string mpeg4config;
                ff_data_to_hex(buf, codecCtx->extradata, codecCtx->extradata_size);
                mpeg4config.assign(buf, codecCtx->extradata_size * 2);
                delete [] buf;
                return mpeg4config;
            }
            break;
        }
    case CODEC_ID_AAC:
        {
            break;
        }
    case CODEC_ID_PCM_S16BE:
        {
            break;
        }
    case CODEC_ID_PCM_MULAW:
        {
            break;
        }
    case CODEC_ID_PCM_ALAW:
        {
            break;
        }
    default:
        /* Nothing special to do here... */
        break;
    }
    return config;
}
