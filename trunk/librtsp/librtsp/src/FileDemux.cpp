
#include "rtspCommon.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#ifdef __cplusplus
}
#endif
#include "MediaDemux.h"
#include "FileDemux.h"
#include "rtsperrors.h"
#include "ffmpegutil.h"
#include "util.h"
#include "H264DemuxSource.h"
#include "MPEG4DemuxSource.h"
#include "MP2DemuxSource.h"
#include <cassert>

#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)

using std::string;


FileDemux::FileDemux( string filename )
{
    filename_ = filename;
}

FileDemux::~FileDemux(void)
{

}

int FileDemux::OpenStream()
{
    int ret;
    ret = av_open_input_file(&formatCtx_, filename_.c_str(), NULL, 0, NULL);
    if( ret )    {        RTSPDEBUG("Error in av_open_input_file, open \"%s\"", filename_.data());        return false; // Couldn't open file    }    ret = av_find_stream_info(formatCtx_);

#ifdef _DEBUG
    dump_format(formatCtx_, 0, filename_.data(), 0);
#endif
    // Retrieve stream information
    if( ret < 0 )    {
        RTSPDEBUG("av_find_stream_info Failed");
        return false; // Couldn't find stream information    }
    for (unsigned int i = 0; i < formatCtx_->nb_streams; ++i )
    {
        if ( formatCtx_->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO )
        {
            AddDemuxSource(formatCtx_->streams[i]->codec, i);
            PacketQueue queue;
            PacketQueueInit(queue);
            queue.queueType = VIDEO_QUEUE;
            packetQueues_[i] = queue;
            videoStreamIndexs_.push_back(i);

        }
        if ( formatCtx_->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO )
        {
            AddDemuxSource(formatCtx_->streams[i]->codec, i);
            PacketQueue queue;
            PacketQueueInit(queue);
            queue.queueType = AUDIO_QUEUE;
            packetQueues_[i] = queue;
            audioStreamIndexs_.push_back(i);
        }
    }
    quit_   = false;
    seekIt_ = false;

    // Starting demux thread
    int err = pthread_create(&demuxThreadID_, NULL , DemuxThread, this);
    if ( 0 != err )
    {
        RTSPDEBUG("[Error]Create Trans Data Thread ERROR");
        return ERR_RTSP_PTHREAD_CREATE;
    }
    return RTSP_OK;
}

int FileDemux::Close()
{
    av_free(formatCtx_);
    std::map<int, PacketQueue>::iterator itor;
    for ( itor = packetQueues_.begin(); itor != packetQueues_.end(); ++itor )
    {
        PacketQueueDestroy(itor->second);
    }
    return RTSP_OK;
}

int FileDemux::GetPacketByStreamIndex( int index, AVPacket &pkt, bool block )
{
    assert(index >= 0);
    if (packetQueues_.find(index) != packetQueues_.end()) //found it
    {
        PacketQueueGet(packetQueues_[index], pkt, block);
        return RTSP_OK;
    }
    return ERR_RTSP_WRONG_INDEX;
}

string FileDemux::GetConfigByStreamIndex( int index, StreamType type )
{
    assert(index >= 0);
    AVCodecContext *codexCtx = formatCtx_->streams[index]->codec;
    return GetConfigByCodecCtx(codexCtx);
}

#define CHECK_PTHREADS_RET(ret)\
if ( 0 != ret )\
{\
    return ERR_RTSP_PTHREAD_INIT;\
}

int FileDemux::PacketQueueInit( PacketQueue &queue )
{
    queue.size = 0;
    queue.packets.clear();
    int ret = pthread_mutex_init(&queue.mutex, NULL);
    CHECK_PTHREADS_RET(ret);
    ret = pthread_cond_init(&queue.cond, NULL);
    CHECK_PTHREADS_RET(ret);
    return RTSP_OK;
}


int FileDemux::PacketQueueDestroy( PacketQueue &queue )
{
    std::deque<AVPacket>::iterator itor;
    for ( itor = queue.packets.begin(); itor != queue.packets.end(); ++itor)
    {
        av_free_packet(&(*itor));
    }
    queue.packets.clear();
    int ret = pthread_mutex_destroy(&queue.mutex);
    CHECK_PTHREADS_RET(ret);
    ret = pthread_cond_destroy(&queue.cond);
    CHECK_PTHREADS_RET(ret);
    return RTSP_OK;
}

int FileDemux::PacketQueuePut( PacketQueue &queue, AVPacket *pkt )
{
    if ( av_dup_packet(pkt) < 0 )
    {
        return -1;
    }

    AVPacket pkt1;
    pkt1 = *pkt;
    // copy data, need do so?
    pkt1.data = new uint8_t[pkt->size];
    ::memcpy(pkt1.data, pkt->data, pkt->size);

    pthread_mutex_lock(&queue.mutex);
    queue.packets.push_back(pkt1);
    queue.size += pkt1.size;
    pthread_cond_signal(&queue.cond);
    //pthread_cond_broadcast(&queue.cond);
    pthread_mutex_unlock(&queue.mutex);
    return RTSP_OK;
}

int FileDemux::PacketQueueGet( PacketQueue &queue, AVPacket &pkt, bool block)
{
    int ret;

    pthread_mutex_lock(&queue.mutex);

    for ( ; ; )
    {
        if ( queue.packets.size() > 0 )
        {
            pkt = queue.packets.front();
            queue.packets.pop_front();
            queue.size -= pkt.size;
            break;
        }
        else if ( !block )
        {
            ret = 0;
            break;
        }
        else
        {
            pthread_cond_wait(&queue.cond, &queue.mutex);
        }
    }
    pthread_mutex_unlock(&queue.mutex);
    return ret;
}

void * FileDemux::DemuxThread( void * arg)
{
    FileDemux *demux = (FileDemux *)arg;
    assert(demux);
    AVPacket pkt1, *packet = &pkt1;
    int videoindex = demux->videoStreamIndexs_[0];
    int audioindex = demux->audioStreamIndexs_[0];
    assert(videoindex >= 0);
    assert(audioindex >= 0);
    for(;;)
    {
        if(demux->quit_)
        {
            break;
        }
        // seek stuff goes here
//         if( demux->seekIt_)
//         {
//             int stream_index= -1;
//             int64_t seek_target = is->seek_pos;
//
//             if(is->videoStream >= 0)
//             {
//                 stream_index = is->videoStream;
//             }
//             else
//             {
//                 if(is->audioStream >= 0)
//                 {
//                     stream_index = is->audioStream;
//                 }
//             }
//
//             if(stream_index >= 0)
//             {
//                 seek_target = av_rescale_q(seek_target, avration_base, pFormatCtx->streams[stream_index]->time_base);
//             }
//             if(!av_seek_frame(is->pFormatCtx, stream_index, seek_target, is->seek_flags))
//             {
//                 fprintf(stderr, "%s: error while seeking\n", is->pFormatCtx->filename);
//             }
//             else
//             {
//                 if(is->audioStream >= 0) {
//                     packet_queue_flush(&is->audioq);
//                     packet_queue_put(&is->audioq, &flush_pkt);
//                 }
//                 if(is->videoStream >= 0) {
//                     packet_queue_flush(&is->videoq);
//                     packet_queue_put(&is->videoq, &flush_pkt);
//                 }
//             }
//             is->seek_req = 0;
//         }

        if( demux->packetQueues_[videoindex].size > MAX_AUDIOQ_SIZE ||
            demux->packetQueues_[audioindex].size > MAX_VIDEOQ_SIZE)
        {
                usleep(1000); // 1 ms
                continue;
        }

        if(av_read_frame( demux->formatCtx_, packet) < 0)
        {
            if( 0 == url_ferror( demux->formatCtx_->pb ) )
            {
                usleep(10000); // no error; wait for user input, 10ms
                continue;
            }
            else
            {
                break;
            }
        }
        // Is this a packet from the video stream?
        if(packet->stream_index == videoindex)
        {
            demux->PacketQueuePut(demux->packetQueues_[videoindex], packet);
        }
        else if(packet->stream_index == audioindex)
        {
            demux->PacketQueuePut(demux->packetQueues_[audioindex], packet);
        }
        //else
        //{
            av_free_packet(packet);
        //}
    }
    /* all done - wait for it */
   // while (!demux->quit_)
   // {
   //     usleep(1000); // 1ms
   // }
    return (void *)0;
}


bool FileDemux::AddDemuxSource( AVCodecContext *codecCtx, int streamindex )
{
    assert(codecCtx);
    DemuxSource *demuxsrc = NULL;
    bool ret = false;
    switch( codecCtx->codec_id )
    {
    case CODEC_ID_H264:
    	{
            demuxsrc = new H264DemuxSource(streamindex);
            break;
        }
    case CODEC_ID_MPEG4:
    	{
            demuxsrc = new MPEG4DemuxSource(streamindex);
            break;
        }
    case CODEC_ID_MP3:
        {
            demuxsrc = new MP2DemuxSource(streamindex);
            break;
        }
    case CODEC_ID_MP2:
        {
            demuxsrc = new MP2DemuxSource(streamindex);
            break;
        }
    default:
        {
            demuxsrc = NULL;
            break;
        }
    }

    if ( NULL != demuxsrc )
    {
        switch( codecCtx->codec_type )
        {
        case CODEC_TYPE_VIDEO:
        case CODEC_TYPE_AUDIO:
            demuxSources_[streamindex] = demuxsrc;
            demuxsrc->SetMediaDemux(this);
            ret = true;
            break;
        default:
            break;
        }
    }
    return ret;
}

int FileDemux::GetStreamNums( StreamType type )
{
    int num = 0;
    switch(type)
    {
    case STREAM_VIDEO:
        num = videoStreamIndexs_.size();
    	break;
    case STREAM_AUDIO:
        num = audioStreamIndexs_.size();
        break;
    default:
        break;
    }
    return num;
}

int FileDemux::GetCodecCtxByStreamIndex( int index, AVCodecContext **codecCtx )
{
    assert(index >= 0);
    *codecCtx = formatCtx_->streams[index]->codec;
    return RTSP_OK;
}

CodecID FileDemux::GetCodecIDByStreamIndex( int index )
{
    assert(index >= 0);
    return formatCtx_->streams[index]->codec->codec_id;
}

DemuxSource* FileDemux::GetDemuxSourceByStreamIndex( int index )
{
    if (demuxSources_.find(index) != demuxSources_.end()) // find it
    {
        return demuxSources_[index];
    }
    return NULL;
}

//! @brief Get the `num'th streamindex with StreamType
/**
@param:  int num must be smaller than GetStreamNums(StreamType)
@param:  StreamType type
@return: int
*/
int FileDemux::GetStreamIndex( int num, StreamType type )
{
    assert(num >= 0);
    int streamindex = -1;
    switch(type)
    {
    case STREAM_VIDEO:
        streamindex = videoStreamIndexs_[num];
        break;
    case STREAM_AUDIO:
        streamindex = audioStreamIndexs_[num];
        break;
    default:
        break;
    }
    return streamindex;
}
