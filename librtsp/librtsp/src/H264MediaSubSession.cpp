

#include "rtspCommon.h"
#include "jrtplib/rtpsession.h"
#include "util.h"
#include "MediaSource.h"
#include "H264FileSource.h"
#include "H264MediaSubSession.h"
#include "jrtplib/rtpipv4address.h"#include "jrtplib/rtpsessionparams.h"#include "jrtplib/rtpudpv4transmitter.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avc.h>
#ifdef __cplusplus
}
#endif
#include "rtsperrors.h"

H264MediaSubSession::H264MediaSubSession(string h264file, int payloadtype, unsigned short port)
: ServerMediaSubSession(payloadtype, port)
{
    mediaSource_ = new H264FileSource(h264file);
    mediaType_   = RTSP::MEDIATYPE_VIDEO;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
}

H264MediaSubSession::H264MediaSubSession( MediaSource *mediasrc, int payloadtype, unsigned short port )
: ServerMediaSubSession(mediasrc, payloadtype, port)
{
    mediaType_   = RTSP::MEDIATYPE_VIDEO;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
}
H264MediaSubSession::~H264MediaSubSession(void)
{
    if ( NULL != mediaSource_ )
    {
        delete mediaSource_;
        mediaSource_ = NULL;
    }
}

string H264MediaSubSession::SDPLines()
{
    string sdp;
    string pt = Utility::ToString(payloadType_);
    sdp += "m=" + mediaType_ + RTSP::SP
        + Utility::ToString(portNum_) + RTSP::SP
        + "RTP/AVP " + pt + RTSP::CRLF;
    sdp += "c=IN IP4 " + destinationAddr_ + "/255\r\n";
    sdp += "a=rtpmap:" + pt + " H264/90000\r\n";
    sdp += "a=fmtp:" + pt + " packetization-mode=1"
        + mediaSource_->GetConfig() + RTSP::CRLF;
    sdp += "a=control:" + TrackName() + RTSP::CRLF;
    return sdp;
}

int H264MediaSubSession::InitRTPSession()
{
    //setup session parameters
    RTPSessionParams sessParams;
    sessParams.SetOwnTimestampUnit(1.0 / 25.0); //25 video frames per second
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
    theRTPSession_.SetDefaultTimestampIncrement(0);

    return ServerMediaSubSession::InitRTPSession();
}

unsigned long H264MediaSubSession::CalcNextSendTime()
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

int H264MediaSubSession::SendPacket()
{
    if ( 0 == nextSendTime_.tv_sec && 0 == nextSendTime_.tv_usec ) // the first time to send a packet
    {
        gettimeofday(&nextSendTime_, NULL);
        lastTS_       = 0;
        lastDuration_ = 0;
    }
    uint8_t buf[ MAX_PACKET_SIZE ];
    ::memset(buf, 0, sizeof ( uint8_t ) * MAX_PACKET_SIZE);
    unsigned long ts;
    int len = mediaSource_->GetMediaData((char *)buf, MAX_PACKET_SIZE - 5, lastDuration_, ts);
    if ( 0 >= len )
    {
        return ERR_RTSP_GET_DATA;
    }
    RTPSendH264(buf, len, ts - lastTS_);
    //RTSPDEBUG("duration:%d, ts:%d", lastDuration_, ts - lastTS_);
    lastTS_ = ts;
    return 0;
}

void H264MediaSubSession::RTPSendH264( uint8_t * buf, int size, unsigned long tsinc )
{
    const uint8_t *r;
    theRTPSession_.IncrementTimestamp(tsinc);
    r = ff_avc_find_startcode(buf, buf + size);
    while ( r < (buf + size))
    {
        const uint8_t *r1;

        while ( !*(r++) );
        r1 = ff_avc_find_startcode(r, buf + size);
        H264SendNAL(r, r1 - r, 0, (r1 == buf + size));
        r = r1;
    }

}

void H264MediaSubSession::H264SendNAL(const uint8_t * buf, int size, unsigned long tsinc, bool marker)
{
    if (size <= MAX_PACKET_SIZE)
    {
        theRTPSession_.SendPacket(buf, size, payloadType_, marker, 0);
    }
    else
    {
        uint8_t type = buf[0] & 0x1F;
        uint8_t nri = buf[0] & 0x60;
        uint8_t *senbuf = new uint8_t[MAX_PACKET_SIZE];
        ::memset(senbuf, 0, sizeof(uint8_t)* MAX_PACKET_SIZE);

        senbuf[0] = 28;        /* FU Indicator; Type = 28 ---> FU-A */
        senbuf[0] |= nri;
        senbuf[1] =  type;
        senbuf[1] |= 1 << 7;
        buf += 1;
        size -= 1;
        while (size + 2 > MAX_PACKET_SIZE)
        {
            ::memcpy(&senbuf[2], buf, MAX_PACKET_SIZE - 2);
            theRTPSession_.SendPacket(senbuf, MAX_PACKET_SIZE, payloadType_, false, 0);
            buf += MAX_PACKET_SIZE - 2;
            size -= MAX_PACKET_SIZE - 2;
            senbuf[1] &= ~(1 << 7);
        }
        senbuf[1] |= 1 << 6;
        ::memcpy(&senbuf[2], buf, size);
        theRTPSession_.SendPacket(senbuf, size + 2, payloadType_, true, 0);
    }
}
