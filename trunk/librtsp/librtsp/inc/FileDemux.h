#pragma once
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <pthread.h>
struct AVPacket; // forward
struct AVCodecContext;
struct AVFormatContext;
class DemuxSource;
class MediaDemux;
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#ifdef __cplusplus
}
#endif
enum  CodecID;

class FileDemux :
    public MediaDemux
{
public:
    typedef enum _eQueueType
    {
        VIDEO_QUEUE,
        AUDIO_QUEUE,
    }QueueType;
    typedef enum _eStreamType
    {
        STREAM_VIDEO,
        STREAM_AUDIO,
        STREAM_NONE,
    }StreamType;

    typedef struct _sPacketQueue
    {
        int size;
        QueueType queueType;
        pthread_mutex_t mutex;
        pthread_cond_t  cond;
        std::deque<AVPacket> packets;
    }PacketQueue;

public:
    FileDemux(std::string filename);
    virtual ~FileDemux(void);
    int          OpenStream();
    int          GetCodecCtxByStreamIndex(int index, AVCodecContext **codecCtx);
    CodecID      GetCodecIDByStreamIndex(int index);
    int          GetPacketByStreamIndex(int index, AVPacket &pkt, bool block);
    std::string  GetConfigByStreamIndex(int index, StreamType type); // type video
    int          Close();
    int          GetStreamNums(StreamType type);
    int          GetStreamIndex(int num, StreamType type);
    DemuxSource* GetDemuxSourceByStreamIndex(int index);

private:
    int         PacketQueueInit(PacketQueue &queue);
    int         PacketQueueDestroy(PacketQueue &queue);
    int         PacketQueuePut(PacketQueue &queue, AVPacket *pkt);
    int         PacketQueueGet(PacketQueue &queue, AVPacket &pkt, bool block);
    bool        AddDemuxSource(AVCodecContext *codecCtx, int streamindex);
    static void *DemuxThread(void *);

private:
    std::map<int, PacketQueue>  packetQueues_;   //!< key stream index
    std::map<int, DemuxSource*> demuxSources_;  //!< key stream index
    std::vector<int> videoStreamIndexs_; //!< all video streams' stream index
    std::vector<int> audioStreamIndexs_; //!< all audio streams' stream index
    pthread_t       demuxThreadID_;
    std::string     filename_;
    AVFormatContext *formatCtx_;
    bool            quit_;
    bool            seekIt_;
};
