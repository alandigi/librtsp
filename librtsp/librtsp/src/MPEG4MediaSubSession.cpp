/*!@file MPEG4MediaSubSession.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.24
@version 1.0
*/

#include "rtspCommon.h"
#include "jrtplib/rtpsession.h"
#include "util.h"
#include "MediaSource.h"
#include "MPEG4FileSource.h"
#include "MPEG4MediaSubSession.h"
#include "jrtplib/rtpipv4address.h"#include "jrtplib/rtpsessionparams.h"#include "jrtplib/rtpudpv4transmitter.h"#include "rtsperrors.h"
#include <cassert>
MPEG4MediaSubSession::MPEG4MediaSubSession(string m4efile, int payloadtype, unsigned short port)
 : ServerMediaSubSession(payloadtype, port)
{
    mediaType_   = RTSP::MEDIATYPE_VIDEO;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
    mediaSource_ = new MPEG4FileSource(m4efile);
}

MPEG4MediaSubSession::MPEG4MediaSubSession( MediaSource *mediasrc, int payloadtype, unsigned short port )
 : ServerMediaSubSession(mediasrc, payloadtype, port)
{
    mediaType_   = RTSP::MEDIATYPE_VIDEO;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
}

MPEG4MediaSubSession::~MPEG4MediaSubSession(void)
{
    if (mediaSource_)
    {
        delete mediaSource_;
        mediaSource_ = NULL;
    }
}

//! @brief get media indepent sdp lines
/**
@return: string
*/
string MPEG4MediaSubSession::SDPLines()
{
    string sdp;
    string pt = Utility::ToString(payloadType_);
    sdp += "m=" + mediaType_ + RTSP::SP
        + Utility::ToString(portNum_) + RTSP::SP
        + "RTP/AVP " + pt + RTSP::CRLF;
    sdp += "c=IN IP4 " + destinationAddr_ + "/255\r\n";
    sdp += "a=rtpmap:" + pt + " MP4V-ES/90000\r\n";
    sdp += "a=fmtp:" + pt + " profile-level-id=3; " + "config="
        + mediaSource_->GetConfig() + RTSP::CRLF;
    sdp += "a=control:" + TrackName() + RTSP::CRLF;
    return sdp;
}

//! @brief functional description of the function
/**
@return: int
*/
int MPEG4MediaSubSession::InitRTPSession()
{
    //setup session parameters
    RTPSessionParams sessParams;
    sessParams.SetOwnTimestampUnit(1.0 / 25.0); //30 video frames per second
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

unsigned long MPEG4MediaSubSession::CalcNextSendTime()
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

int MPEG4MediaSubSession::SendPacket()
{
    if ( 0 == nextSendTime_.tv_sec && 0 == nextSendTime_.tv_usec ) // the first time to send a packet
    {
        gettimeofday(&nextSendTime_, NULL);
        lastTS_       = 0;
        lastDuration_ = 0;
    }
    char buf[ MAX_PACKET_SIZE ];
    ::memset(buf, 0, sizeof ( char ) * MAX_PACKET_SIZE);
    unsigned long ts;
    assert(mediaSource_);
    int len = mediaSource_->GetMediaData(buf, MAX_PACKET_SIZE - 5, lastDuration_, ts);
    if ( 0 >= len )
    {
        return ERR_RTSP_GET_DATA;
    }
    theRTPSession_.IncrementTimestamp(ts - lastTS_);
    //RTSPDEBUG("ts diff %d", ts - lastTS_);
    SplitAndSend(buf, len);
    lastTS_ = ts;
    return 0;
}

#define DEFAULT_SPLIT_SIZE 1460

int MPEG4MediaSubSession::SplitAndSend( char * buf, int len)
{
    char *pCur = buf;
    int sendcount = len / DEFAULT_SPLIT_SIZE + ((len % DEFAULT_SPLIT_SIZE) ? 1 : 0);
    int sendsize = DEFAULT_SPLIT_SIZE;
    bool marker =  false;
    for ( int i = 0; i < sendcount; ++i )
    {
        if ( sendcount - 1 == i ) //!< send the last packet
        {
            sendsize = len % DEFAULT_SPLIT_SIZE;
            marker = true;
        }
        theRTPSession_.SendPacket(pCur, sendsize, payloadType_, marker, 0);
        pCur += sendsize;
    }
    return 0;
}
