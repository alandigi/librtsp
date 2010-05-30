/*!@file ServerMediasession.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "rtspCommon.h"
#include "util.h"
#include "jrtplib/rtpsession.h"
#include <sys/time.h>
#include "ServerMediaSubsession.h"
#include "ServerMediasession.h"
#include <cmath>



ServerMediaSession::ServerMediaSession( string streamName /*= "VoIP_Stream"*/,
                                        string InfoString /*= "VoIP Live"*/,
                                        string DesString /*= "Streaming By VoIP Live System"*/,
                                        string micsString /*= ""*/ )
{
    streamName_           = streamName;
    SDPInfoString_        = InfoString;
    SDPDescriptionString_ = DesString;
    SDPmicsString_        = micsString;
    streamingStatus_      = STREAM_READY;
}

ServerMediaSession::~ServerMediaSession(void)
{
    SubsessionIterator begin = subSessions_.begin();
    SubsessionIterator end   = subSessions_.end();
    SubsessionIterator Itor  = begin;

    for ( ; Itor != end; ++Itor )
    {
        if ( *Itor )
        {
            delete *Itor;
            *Itor = NULL;
        }
    }
}

string ServerMediaSession::GenerateSDPDescription()
{
    //To support SSM source in the future

    float dur = Duration();
    string rangeLine;
    if (dur == 0.0) ///FIXME
    {
        rangeLine = string("a=range:npt=0-");
    }
    else if (dur > 0.0)
    {
        char buf[100];
        sprintf(buf, "a=range:npt=0-%.3f", dur);
        rangeLine = string(buf);
    }
    else
    { // subSessions have differing durations, so "a=range:" lines go there
        rangeLine = "";
    }

    string sdpinfo;
    sdpinfo += "v=0\r\n"; // Version 0
    struct timeval generatetime;
    gettimeofday(&generatetime, NULL);
    char buf[100];

    sprintf(buf, "o=- %ld%06ld %d IN IP4 %s\r\n",
        generatetime.tv_sec,
        generatetime.tv_usec, 1,
        localIPAddr_.data());
    sdpinfo += buf; // Version 0
    sdpinfo += "s=" + SDPDescriptionString_ + RTSP::CRLF;
    sdpinfo += "i=" + SDPInfoString_ + RTSP::CRLF;
    //sdpinfo += "a=tool:" + ""
    sdpinfo += "t=0 0\r\n";
    sdpinfo += "a=type:broadcast\r\n";
    sdpinfo += "a=control:*\r\n";
    sdpinfo += rangeLine + RTSP::CRLF;
    sdpinfo += "a=x-qt-text-nam:" + SDPDescriptionString_ + RTSP::CRLF;
    sdpinfo += "a=x-qt-text-inf:" + SDPInfoString_ + RTSP::CRLF;

    SubsessionIterator begin = subSessions_.begin();
    SubsessionIterator end   = subSessions_.end();
    SubsessionIterator Itor  = begin;

    for ( ; Itor != end; ++Itor )
    {
       sdpinfo += (*Itor)->SDPLines();
    }

    return sdpinfo;
}

bool ServerMediaSession::AddSubsession( ServerMediaSubSession * subsession)
{
    if ( subsession->IsAlreadyUsed() )
    {
        return false;
    }
    else
    {
        subSessions_.push_back(subsession);
        subsession->IncreUseCount();
        subsession->SetTrackID(subSessions_.size());
    }
    return true;
}

float ServerMediaSession::Duration() const
{
    float result = 0.0;
    float maxduration = 0;
    float minduration = 0;
    const_SubsessionIterator begin = subSessions_.begin();
    const_SubsessionIterator end   = subSessions_.end();
    const_SubsessionIterator Itor  = begin;

    for ( ; Itor != end; ++Itor )
    {
        float dur = (*Itor)->Duration();
        if ( dur > maxduration )
        {
            maxduration = dur;
        }
        if ( dur < minduration )
        {
            minduration = dur;
        }
    }
    // subSession durations differ
    // fabs( maxduration - minduration ) < 1e-6
    if ( maxduration - minduration < 1e-6
        || - ( maxduration - minduration ) < 1e-6)
    {
        result = - maxduration;
    }
    else  // all subSession durations are the same
    {
        result = maxduration;
    }
    return result;
}

int ServerMediaSession::StartStream()
{
    if ( STREAMING == streamingStatus_ )
    {
        return 0;
    }
    SubsessionIterator begin = subSessions_.begin();
    SubsessionIterator end   = subSessions_.end();
    SubsessionIterator Itor  = begin;


    for ( ; Itor != end; ++Itor )
    {
        (*Itor)->Start();
    }
    streamingStatus_ = STREAMING;
    return -1;
}

void ServerMediaSession::TestScaleFactor( float& scale )
{
    scale = 1;
}

void ServerMediaSession::AddDestinationAddr( string dstaddr )
{
    SubsessionIterator begin = subSessions_.begin();
    SubsessionIterator end   = subSessions_.end();
    SubsessionIterator Itor  = begin;
    destinationAddr_ = dstaddr;
    for ( ; Itor != end; ++Itor )
    {
        (*Itor)->AddDestination(dstaddr);
    }
}

ServerMediaSubSession * ServerMediaSession::FindSubSessionByTrackName( string trackname )
{
    SubsessionIterator begin = subSessions_.begin();
    SubsessionIterator end   = subSessions_.end();
    SubsessionIterator Itor  = begin;

    for ( ; Itor != end; ++Itor )
    {
        string tname = (*Itor)->TrackName();
        if (tname == trackname )
        {
            return *Itor;
        }
    }
    return NULL;
}

string ServerMediaSession::GetRTPInfo( string requestURI )
{
    SubsessionIterator begin = subSessions_.begin();
    SubsessionIterator end   = subSessions_.end();
    SubsessionIterator Itor  = begin;
    string rtpInfo;
    for ( ; Itor != end; ++Itor )
    {
        if (Itor != begin )
        {
            rtpInfo += ",";
        }
        rtpInfo += "url=" + requestURI + "/" +(*Itor)->RTPInfo();
    }
    return rtpInfo;
}

int ServerMediaSession::StopStream()
{
    if ( STREAM_STOP == streamingStatus_ )
    {
        return 0;
    }
    SubsessionIterator begin = subSessions_.begin();
    SubsessionIterator end   = subSessions_.end();
    SubsessionIterator Itor  = begin;

    for ( ; Itor != end; ++Itor )
    {
        (*Itor)->Stop();
    }
    streamingStatus_ = STREAM_STOP;
    return 0;
}

void ServerMediaSession::SetLoaclAddr( string addr )
{
    localIPAddr_ = addr;
}
