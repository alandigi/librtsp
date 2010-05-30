/*!@file MP3MediaSubSession.cpp
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
#include <sys/time.h>
#include "MediaSource.h"
#include "MP3FileSource.h"
#include "MP3MediaSubSession.h"
#include "jrtplib/rtpipv4address.h"#include "jrtplib/rtpsessionparams.h"#include "jrtplib/rtpudpv4transmitter.h"
#include "rtsperrors.h"
#include <assert.h>

MP3MediaSubSession::MP3MediaSubSession(string mp3file, int payloadtype, unsigned short port)
 : ServerMediaSubSession(payloadtype, port)
{
    mediaType_   = RTSP::MEDIATYPE_AUDIO;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
    mediaSource_ = new MP3FileSource(mp3file);
}

MP3MediaSubSession::MP3MediaSubSession( MediaSource *mediasrc, int payloadtype, unsigned short port )
 : ServerMediaSubSession(mediasrc, payloadtype, port)
{
    mediaType_   = RTSP::MEDIATYPE_AUDIO;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
}
MP3MediaSubSession::~MP3MediaSubSession(void)
{
    if (mediaSource_)
    {
        delete mediaSource_;
        mediaSource_ = NULL;
    }
}

string MP3MediaSubSession::SDPLines()
{
    string sdp;
    sdp += "m=" + mediaType_ + RTSP::SP
        + Utility::ToString(portNum_) + RTSP::SP
        + "RTP/AVP " + Utility::ToString(payloadType_) + RTSP::CRLF;
    sdp += "c=IN IP4 " + destinationAddr_ + "/255\r\n";
    sdp += "a=control:" + TrackName() + RTSP::CRLF;
    return sdp;
}

int MP3MediaSubSession::InitRTPSession()
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
        RTSPDEBUG("[Error] : %s", stdErrStr.c_str());
        return ERR_RTSP_CRAETE_RTP_SESSION; //unable to create the session
    }
    return ServerMediaSubSession::InitRTPSession();
}

unsigned long MP3MediaSubSession::CalcNextSendTime()
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

int MP3MediaSubSession::SendPacket()
{
    bool isFirstPkt = false;
    if ( 0 == nextSendTime_.tv_sec && 0 == nextSendTime_.tv_usec ) // the first time to send a packet
    {
        gettimeofday(&nextSendTime_, NULL);
        lastTS_       = 0;
        lastDuration_ = 0;
        isFirstPkt    = true;
    }
    char buf[ MAX_PACKET_SIZE ];
    ::memset(buf, 0, sizeof ( char ) * MAX_PACKET_SIZE);
    unsigned long ts;
    assert(mediaSource_);
    int len = mediaSource_->GetMediaData(buf + 4, MAX_PACKET_SIZE - 20, lastDuration_, ts);
    if ( 0 >= len )
    {
        return ERR_RTSP_GET_DATA;

    }
    theRTPSession_.SendPacket(buf, len + 4, payloadType_, isFirstPkt, ts - lastTS_);
    lastTS_ = ts;
    return 0;
}
