#include <iostream>

using namespace std;
#include <librtsp.h>
#include "test.h"
#include <cassert>
#include <cstdlib>
//rtsp://192.168.88.245:9999/test
//rtsp://192.168.88.245:9999/voip
#define CHECK_RET(ret)\
if (ret < 0)\
{\
    cout << "Error " << __FILE__ << " at " << __LINE__ << '\n';\
    return -1;\
}
ServerMediaSubSession * InitMediaSubSession(DemuxSource *demuxsrc);
int main(int argv, char **argc)
{
    int ret;
    int mode = 0;
    if ( argv >= 2)
    {
        mode = atoi(argc[1]);
    }
    unsigned short port = 9999;
    av_register_all();
    RTSPServer x(port);
    cout << "Listen On Port: " << port << '\n';
   // FileDemux filedemux("h264_320_240_25.vob");
    FileDemux *filedemux;
    if ( !mode )
    {
        filedemux = new FileDemux("mpeg4_352_288_25.vob");
        cout << "MEPG4+Audio\n";
    }
    else
    {
        filedemux = new FileDemux("h264_320_240_25.vob");
        cout << "H.264+Audio\n";
    }
    filedemux->OpenStream();
    int videoindex = -1;
    int audioindex = -1;
    if ( filedemux->GetStreamNums(FileDemux::STREAM_VIDEO) > 0 )
    {
        videoindex = filedemux->GetStreamIndex(0, FileDemux::STREAM_VIDEO);
    }
    if ( filedemux->GetStreamNums(FileDemux::STREAM_AUDIO) > 0 )
    {
        audioindex = filedemux->GetStreamIndex(0, FileDemux::STREAM_AUDIO);
    }
    cout << "videoindex: " << videoindex << " " << "audioindex: " << audioindex << '\n';
    assert(videoindex >= 0);
    assert(audioindex >= 0);
    DemuxSource *videoSource = filedemux->GetDemuxSourceByStreamIndex(videoindex);
    DemuxSource *audioSource = filedemux->GetDemuxSourceByStreamIndex(audioindex);

    ServerMediaSubSession *demuxmpegaudioSession = InitMediaSubSession(audioSource);
    ServerMediaSubSession *demuxH264Session = InitMediaSubSession(videoSource);

    ret = demuxmpegaudioSession->InitRTPSession();
    CHECK_RET(ret);
    ret = demuxH264Session->InitRTPSession();
    CHECK_RET(ret);

    MP3MediaSubSession *mp3Session = new MP3MediaSubSession("test.mp3", 14, 6666);
    MP3MediaSubSession *mp3Session2 = new MP3MediaSubSession("test.mp3", 14, 6686);
    //G729DMediaSubSession *g729dSession = new G729DMediaSubSession("g729d.enc", 100, 6686);
    ret = mp3Session->InitRTPSession();
    CHECK_RET(ret);
    ret = mp3Session2->InitRTPSession();
    //ret = g729dSession->InitRTPSession();
    CHECK_RET(ret);

    MPEG4MediaSubSession *mpeg4Session = new MPEG4MediaSubSession("wwe.m4e", 96, 6668);
    H264MediaSubSession  *h264session = new H264MediaSubSession("H264.mpg", 96, 6688);
    ret = mpeg4Session->InitRTPSession();
    CHECK_RET(ret);
    ret = h264session->InitRTPSession();
    CHECK_RET(ret);

    string stremName = "test";
    string stremName2 = "voip";
    string demuxstreamName = "demux";

    ServerMediaSession *mediasession = new ServerMediaSession(stremName);
    ServerMediaSession *mediasession2 = new ServerMediaSession(stremName2);
    ServerMediaSession *demuxsession = new ServerMediaSession(demuxstreamName);

    string localIP = "192.168.88.245";
    vector<string> ipaddr;
    GetAllLocalIP(ipaddr);
    if ( ipaddr.size() > 0 )
    {
        localIP = ipaddr[0];
    }
    x.SetLoaclIPAddr(localIP);
    mediasession->SetLoaclAddr(localIP);
    mediasession2->SetLoaclAddr(localIP);
    demuxsession->SetLoaclAddr(localIP);

    mediasession->AddSubsession(mpeg4Session);
    mediasession->AddSubsession(mp3Session);

    mediasession2->AddSubsession(h264session);
    mediasession2->AddSubsession(mp3Session2);

    demuxsession->AddSubsession(demuxmpegaudioSession);
    demuxsession->AddSubsession(demuxH264Session);


    mediasession->AddDestinationAddr(string("239.168.168.168"));
    mediasession2->AddDestinationAddr(string("239.168.168.169"));
    demuxsession->AddDestinationAddr(string("239.168.168.188"));

    x.AddMediaSession(stremName, mediasession);
    x.AddMediaSession(stremName2, mediasession2);
    x.AddMediaSession(demuxstreamName, demuxsession);
    x.Start();
    sleep(3000);
    x.Stop();
    filedemux->Close();
    return 0;
}

ServerMediaSubSession * InitMediaSubSession( DemuxSource *demuxsrc )
{
    ServerMediaSubSession * amediasubsession = NULL;
    switch(demuxsrc->GetCodecID())
    {
    case CODEC_ID_H264:
        amediasubsession = new H264MediaSubSession(demuxsrc, 96, 6868);
    	break;
    case CODEC_ID_MPEG4:
        amediasubsession = new MPEG4MediaSubSession(demuxsrc, 96, 6868);
    	break;
    case CODEC_ID_MP2:
    case CODEC_ID_MP3:
        amediasubsession = new MP3MediaSubSession(demuxsrc, 14, 6866);
        break;
    default:
        break;
    }
    return amediasubsession;
}

