/*!@file ServerMediaSubsession.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "rtspCommon.h"
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "util.h"
#include "jrtplib/rtpipv4address.h"
#include "jrtplib/rtpsessionparams.h"
#include "jrtplib/rtpsession.h"
#include "MediaSource.h"
#include "ServerMediaSubsession.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "rtsperrors.h"
#include <assert.h>

ServerMediaSubSession::ServerMediaSubSession(int payloadtype, unsigned short port)
 : bExit_(false)
 , addedCount_(0)
{
    Init(payloadtype, port);
}

ServerMediaSubSession::ServerMediaSubSession( MediaSource *mediasrc, int payloadtype, unsigned short port )
 : bExit_(false)
 , addedCount_(0)
{
    Init(payloadtype, port);
    assert(mediasrc);
    mediaSource_ = mediasrc;
}

void   ServerMediaSubSession::Init(int payloadtype, unsigned short port)
{
    payloadType_ = payloadtype;
    portNum_     = port;
    nextSendTime_.tv_sec = 0;
    nextSendTime_.tv_usec = 0;
    destinationAddr_ = "239.168.168.168";
}
ServerMediaSubSession::~ServerMediaSubSession(void)
{
}

//TODO
float ServerMediaSubSession::Duration() const
{
    return 0;
}


int ServerMediaSubSession::InitRTPSession()
{
    assert(mediaSource_);
    bool ret = mediaSource_->Open();
    if ( false == ret )
    {
        return ERR_RTSP_OPEN_FILE;
    }
    return RTSP_OK;
}

int ServerMediaSubSession::Start()
{
    bExit_ = false;
    int err = pthread_create(&threadid_, NULL , TransThread, this);
    if ( 0 != err )
    {
        RTSPDEBUG("[Error]Create Trans Data Thread ERROR");
        return ERR_RTSP_PTHREAD_CREATE;
    }
    return RTSP_OK;
}

int ServerMediaSubSession::Stop()
{
    void *tret = 0;
    bExit_  = true;
  //  mediaSource_->Close();
  //  mediaSource_->Open(); // Open it for next request
    int err = pthread_join(threadid_, &tret);
    if ( 0 != err )
    {
        RTSPDEBUG("[Error]Can not join with thread: %s",strerror(err));
    }

    return 0;
}


string ServerMediaSubSession::GetRangeSDPLine() const
{
    // If all of our parent's subsessions have the same duration
    // (as indicated by "fParentSession->duration() >= 0"), there's no "a=range:" line:
   // if (fParentSession->duration() >= 0.0) return strDup("");

    // Use our own duration for a "a=range:" line:
    float ourDuration = Duration();
    if ( ourDuration <= 1e-6)
    {
        return string("a=range:npt=0-\r\n");
    }
    else
    {
        char buf[100];
        sprintf(buf, "a=range:npt=0-%.3f\r\n", ourDuration);
        return string(buf);
    }
}

void * ServerMediaSubSession::TransThread( void * arg )
{
    RTSPDEBUG("[Info]Successfully Create Trans Data Thread.");
    ServerMediaSubSession *s = (ServerMediaSubSession *)arg;
    struct timeval now;
    int ret = 0;
    while ( false == s->bExit_ )
    {
        ret = s->SendPacket();
        if ( ret < 0 ) // Error Occured
            break;
        gettimeofday(&now, NULL);
        unsigned long nowtime  = now.tv_sec * MILLION + now.tv_usec;
        unsigned long nextsendTime = s->CalcNextSendTime();
        unsigned long delay = 0;
        if ( nowtime < nextsendTime )
        {
            delay = nextsendTime - nowtime;
        }
        usleep( delay );
        //cout << s->TrackName() << " : waited " << delay << "uS\n";
    }
    RTSPDEBUG("[Info]Exit Trans Data Thread...");
    return (void *)0;
}

void ServerMediaSubSession::AddDestination( string &dst )
{
    destinationAddr_ = dst;
    unsigned long intIP = inet_addr(destinationAddr_.c_str());

    //put in host byte order
    intIP = ntohl(intIP);
    RTPIPv4Address rtpAddr(intIP, portNum_);
    int status = theRTPSession_.AddDestination(rtpAddr);
    if ( 0 > status )
    {
        string stdErrStr = RTPGetErrorString(status);
        RTSPDEBUG("[Error] : %s", stdErrStr.c_str());
    }
}

string ServerMediaSubSession::RTPInfo()
{
    string rtpinfo;
    rtpinfo  = TrackName() + ";seq=" + Utility::ToString(theRTPSession_.GetSequenceNumber());
    rtpinfo += ";rtptime="  + Utility::ToString(theRTPSession_.GetTimestamp());
    return rtpinfo;
}
