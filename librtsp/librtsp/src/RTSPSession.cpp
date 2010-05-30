/*!@file RTSPSession.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "RTSPPacket.h"
#include "RTSPServer.h"
#include "RTSPSessionBase.h"
#include "RTSPSession.h"
#include "TCPSocket.h"
#include "util.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include "jrtplib/rtpsession.h"
#include "ServerMediasession.h"
#include "ServerMediaSubsession.h"

//#define DUMP_PACKET

RTSPSession::RTSPSession(void)
: rtspStatus_(RTSP::RTSP_INIT)
, streamName_("")
//, sessionId_(0)
{
    socket_ = new TCPSocket;
}

RTSPSession::~RTSPSession(void)
{
    if (socket_)
    {
        socket_->Close();
        delete socket_;
        socket_ = NULL;
    }
}

int RTSPSession::EventHandler()
{
    bool exit = false;
    char buf[MAX_BUF_SIZE];
    int ret = 0;
    //int  length = MAX_BUF_SIZE;
    while (false == exit)
    {
        ::memset(buf, 0, sizeof(char) * MAX_BUF_SIZE);
        int numbytes = socket_->Receive(buf, MAX_BUF_SIZE);

        if (numbytes <= 0)
        {
            //RTSPDEBUG("Disconnected from %s:%d", socket_->GetRemoteAddr(), socket_->GetRemotePort());
            RTSPDEBUG("Disconnected!");
            ret = -1;
            exit = true;
            break;
        }
        ParseRTSPRequestString(buf, numbytes);
        if ( MAX_BUF_SIZE - 1 > numbytes)
        {
            ret = 0;
            exit = true;
            break;
        }
    }
    return ret;
}


int RTSPSession::ParseRTSPRequestString( char *buf, int len )
{
    std::fstream of;
	of.open("received.log",std::ios_base::out | std::ios_base::app);
    of << "received:\n";
    for ( int i = 0; i < len; ++i)
    {
        of << buf[i];
    }
    of.close();

    packet_.SetData(buf, len);
#ifdef DUMP_PACKET
    packet_.DumpPacket();
#endif
    if ( packet_.IsCompletePacket() ) // receive all the data of a packet
    {
        string method = packet_.GetRequestMethod();
        RTSPPacket responsepkt;
        responsepkt.SetPktType(RTSP::RESPONSE_PKT);

        if ( handlerMap_[method] )//FIXME, this should be wrong
        {
            int ret = (this->*handlerMap_[method])(packet_, responsepkt);
            if ( ret < 0 )
            {
                RTSPDEBUG("[Error] in ParseRTSPRequestString.");
            }
        }
        else
        {
            HandleUnspportedRequest(packet_, responsepkt);
            RTSPDEBUG("[Waring] Unsupported Method.");
        }
        responsepkt.AddAHeaderLine(RTSP::HeaderField::CSeq, packet_.GetHeaderLine(RTSP::HeaderField::CSeq));
        SendResponse(responsepkt);
        packet_.Reset();
    }
    return 0;
}

int RTSPSession::HandleOPTIONSRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt)
{
    string requestURI = requestpkt.GetRequestURI();
    streamName_ = requestURI.substr(requestURI.find('/')+1, requestURI.length());
    ServerMediaSession *msession = server_->FindMediaSessionByStreamName(streamName_);
    if ( NULL == msession )
    {
        RTSPDEBUG("[Warning] StreamName: %s Not Found.", streamName_.c_str());
        responsepkt.SetStatusCode("404");
        return -1;
    }
    else
    {
        responsepkt.SetStatusCode("200");
        string publicval;
        const_HandlerMap_Iterrator begin = this->handlerMap_.begin();
        const_HandlerMap_Iterrator end = this->handlerMap_.end();
        for (const_HandlerMap_Iterrator itor = begin; itor != end; ++itor)
        {
            if (itor != begin)
            {
                publicval += ", ";
            }
            publicval += itor->first;
        }
        responsepkt.AddAHeaderLine(RTSP::HeaderField::Public, publicval);
    }

    return 0;
}

int RTSPSession::HandleSETUPRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt)
{
    if ( RTSP::RTSP_INIT == rtspStatus_ )
    {
        rtspStatus_ = RTSP::RTSP_READY;
    }
    string requestURI = requestpkt.GetRequestURI();
    ServerMediaSession *mediaSesion = server_->FindMediaSessionByStreamName(streamName_);
    if ( !mediaSesion )
    {
        RTSPDEBUG("[Warning] StreamName: %s Not Found.", streamName_.c_str());
        responsepkt.SetStatusCode("404");
        return -1;
    }
    string trackname = requestURI.substr(requestURI.rfind('/')+1, requestURI.length());
    ServerMediaSubSession *submsession = mediaSesion->FindSubSessionByTrackName(trackname);
    if ( !submsession )
    {
        RTSPDEBUG("[Warning] Trackname: %s Not Found.", trackname.c_str());
        responsepkt.SetStatusCode("404");
        return -1;
    }

    // "Session: %d",
    responsepkt.AddAHeaderLine(RTSP::HeaderField::Session, Utility::ToString(this->GetSessionID()));

    // "Transport: RTP/AVP;multicast;destination=%s;source=%s;port=%d-%d;ttl=%d"
    string transportval = "RTP/AVP;multicast;destination=" + mediaSesion->GetDestinationAddr();
    unsigned short port = submsession->PortBase();
    transportval += ";source=" + server_->GetLocalIPAddr()
                 + ";port=" + Utility::ToString(port)
                 + "-" + Utility::ToString(port+1);
    transportval += ";ttl=255";

    responsepkt.AddAHeaderLine(RTSP::HeaderField::Transport, transportval);

    responsepkt.SetStatusCode("200");
    return 0;
}

int RTSPSession::HandlePLAYRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt)
{
    if (RTSP::RTSP_READY != rtspStatus_ )
    {
        responsepkt.SetStatusCode("455");
        return -1;
    }
    rtspStatus_ = RTSP::RTSP_PLAYING;
    string requestURI = requestpkt.GetRequestURI();
    ServerMediaSession *mediaSesion = server_->FindMediaSessionByStreamName(streamName_);
    if ( !mediaSesion )
    {
        RTSPDEBUG("[Warning] StreamName: %s Not Found.", streamName_.c_str());
        responsepkt.SetStatusCode("404");
        return -1;
    }
    string rtpinfo = mediaSesion->GetRTPInfo( "rtsp://" + requestURI );
    // "Session: %d",
    responsepkt.AddAHeaderLine(RTSP::HeaderField::Session, Utility::ToString(this->GetSessionID()));

    // RTPInfo
    responsepkt.AddAHeaderLine(RTSP::HeaderField::RTP_Info, rtpinfo);

    // Range
    responsepkt.AddAHeaderLine(RTSP::HeaderField::Range, responsepkt.GetHeaderLine(RTSP::HeaderField::Range));

    mediaSesion->StartStream();
    responsepkt.SetStatusCode("200");
    return 0;
}

int RTSPSession::HandleDESCRIBERequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt)
{

    string requestURI = requestpkt.GetRequestURI();
    responsepkt.AddAHeaderLine(RTSP::HeaderField::Content_Base, "rtsp://" + requestURI);
    responsepkt.AddAHeaderLine(RTSP::HeaderField::Content_Type, requestpkt.GetHeaderLine(RTSP::HeaderField::Accept));
    streamName_ = requestURI.substr(requestURI.find('/')+1, requestURI.length());
    ServerMediaSession *mediaSesion = server_->FindMediaSessionByStreamName(streamName_);
    if (NULL == mediaSesion)
    {
        RTSPDEBUG("[Warning] StreamName: %s Not Found.", streamName_.c_str());
        responsepkt.SetStatusCode("404");
        return -1;
    }
    string sdp = mediaSesion->GenerateSDPDescription();
    int length = sdp.length();
    responsepkt.AddAHeaderLine(RTSP::HeaderField::Content_Length, Utility::ToString(length));
    responsepkt.SetContent(sdp);

    responsepkt.SetStatusCode("200");
    return 0;
}

int RTSPSession::HandleTEARDOWNRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt)
{
    rtspStatus_ = RTSP::RTSP_INIT;
    ServerMediaSession *mediaSesion = server_->FindMediaSessionByStreamName(streamName_);
    if ( !mediaSesion )
    {
        RTSPDEBUG("[Warning] StreamName: %s Not Found.", streamName_.c_str());
        responsepkt.SetStatusCode("404");
        return -1;
    }
    mediaSesion->StopStream();
    responsepkt.SetStatusCode("200");
    return 0;
}

int RTSPSession::HandleUnspportedRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt)
{
    responsepkt.SetStatusCode("405");
    return 0;
}

int RTSPSession::SendResponse(RTSPPacket &pkt)
{
    AddDateInfo(pkt);

    const string src = pkt.GetData();
    int len = src.length();
    socket_->Send((char *)src.data(), len);
#ifdef DUMP_PACKET
    pkt.DumpPacket();
#endif
    return 0;
}

void RTSPSession::AddDateInfo(RTSPPacket & pkt)
{
    time_t tt = time(NULL);
    char buf[50];
    ::memset(buf, 0, sizeof(char)*50);
    strftime(buf, sizeof buf, "%a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));
    pkt.AddAHeaderLine(RTSP::HeaderField::Date, string(buf));
}

void RTSPSession::InitHandlerMap()
{
    handlerMap_[RTSP::Method::OPTIONS]  = &RTSPSession::HandleOPTIONSRequest;
    handlerMap_[RTSP::Method::SETUP]    = &RTSPSession::HandleSETUPRequest;
    handlerMap_[RTSP::Method::PLAY]     = &RTSPSession::HandlePLAYRequest;
    handlerMap_[RTSP::Method::DESCRIBE] = &RTSPSession::HandleDESCRIBERequest;
    handlerMap_[RTSP::Method::TEARDOWN] = &RTSPSession::HandleTEARDOWNRequest;
}

void RTSPSession::Init()
{
    InitHandlerMap();
}
