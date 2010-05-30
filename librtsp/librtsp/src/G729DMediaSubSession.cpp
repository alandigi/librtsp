


#include "rtspCommon.h"
#include "jrtplib/rtpsession.h"
#include "util.h"
#include "MediaSource.h"
#include "G729DFileSource.h"
#include "G729DMediaSubSession.h"
#include "jrtplib/rtpipv4address.h"#include "jrtplib/rtpsessionparams.h"#include "jrtplib/rtpudpv4transmitter.h"
#include "rtsperrors.h"

G729DMediaSubSession::G729DMediaSubSession(string g729dfile, int payloadtype, unsigned short port)
 : ServerMediaSubSession(payloadtype, port)
{
    mediaType_   = RTSP::MEDIATYPE_AUDIO;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
    mediaSource_ = new G729DFileSource(g729dfile);
}

G729DMediaSubSession::G729DMediaSubSession( MediaSource *mediasrc, int payloadtype, unsigned short port )
 : ServerMediaSubSession(mediasrc, payloadtype, port)
{
    mediaType_   = RTSP::MEDIATYPE_AUDIO;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
}
G729DMediaSubSession::~G729DMediaSubSession(void)
{
    if ( mediaSource_ )
    {
        delete mediaSource_;
        mediaSource_ = NULL;
    }
}
/*
m=audio `port' RTP/AVP 103
a=rtpmap:103 G729D/8000
a=fmtp:103 annexb=yes
*/
string G729DMediaSubSession::SDPLines()
{
    string sdp;
    string pt = Utility::ToString(payloadType_);
    sdp += "m=" + mediaType_ + RTSP::SP
        + Utility::ToString(portNum_) + RTSP::SP
        + "RTP/AVP " + pt + RTSP::CRLF;
    sdp += "c=IN IP4 " + destinationAddr_ + "/255\r\n";
    sdp += "a=rtpmap:" + pt + " G729D/8000\r\n";
    sdp += "a=fmtp:" + pt + "  annexb=yes" + RTSP::CRLF;
    sdp += "a=control:" + TrackName() + RTSP::CRLF;
    return sdp;
}

int G729DMediaSubSession::InitRTPSession()
{
    //setup session parameters
    RTPSessionParams sessParams;
    sessParams.SetOwnTimestampUnit(1.0 / 8000.0); //30 video frames per second
    sessParams.SetUsePollThread(1); // background thread to call virtual callbacks - set by default, but just to be sure
    sessParams.SetMaximumPacketSize(MAX_PACKET_SIZE);
    //setup transmission parameters
    RTPUDPv4TransmissionParams transParams;
    transParams.SetPortbase(portNum_);
    //CREATE THE SESSION
    int errcode = theRTPSession_.Create(sessParams, &transParams);
    if ( errcode < 0 )
    {
        string stdErrStr = RTPGetErrorString(errcode);
        return ERR_RTSP_CRAETE_RTP_SESSION; //unable to create the session
    }

    return ServerMediaSubSession::InitRTPSession();
}

unsigned long G729DMediaSubSession::CalcNextSendTime()
{
    nextSendTime_.tv_sec  += lastDuration_ / MILLION;
    nextSendTime_.tv_usec += lastDuration_ % MILLION;
    if ( nextSendTime_.tv_usec > MILLION )
    {
        nextSendTime_.tv_usec -= MILLION;
        nextSendTime_.tv_sec  += 1;
    }
    return nextSendTime_.tv_sec * MILLION + nextSendTime_.tv_usec;
}

int G729DMediaSubSession::SendPacket()
{
    bool marker = false;
    if ( 0 == nextSendTime_.tv_sec && 0 == nextSendTime_.tv_usec ) // the first time to send a packet
    {
        gettimeofday(&nextSendTime_, NULL);
        lastTS_       = 0;
        lastDuration_ = 0;
        marker = true;
    }
    char buf[ MAX_PACKET_SIZE ];
    ::memset(buf, 0, sizeof ( char ) * MAX_PACKET_SIZE);
    unsigned long ts;
    int len = mediaSource_->GetMediaData(buf, MAX_PACKET_SIZE - 5, lastDuration_, ts);
    if ( 0 >= len )
    {
        return ERR_RTSP_GET_DATA;
    }
    theRTPSession_.SendPacket(buf, G729D_FRAME_SIZE, payloadType_, marker, ts - lastTS_);
    lastTS_ = ts;
    return 0;
}
