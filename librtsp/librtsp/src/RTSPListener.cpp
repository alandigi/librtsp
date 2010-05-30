/*!@file RTSPListener.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "rtspCommon.h"
#include "RTSPListener.h"
#include "RTSPPacket.h"
#include "RTSPSession.h"
#include "ListenSocket.h"
#include "util.h"

RTSPListener::RTSPListener(unsigned short port)
: port_(port)
{
    socket_ = new ListenSocket;
    socket_->Create(port_);
    socket_->Listen();
}

RTSPListener::~RTSPListener(void)
{
    if (socket_)
    {
        socket_->Close();
        delete socket_;
        socket_ = NULL;
    }
}

int RTSPListener::EventHandler()
{
    int socketfd = socket_->Accept();

    RTSPSessionBase * session  = new RTSPSession;
    session->SetSocketFD(socketfd);
    session->Init();
    server_->AddRTSPSession(socketfd, session);
    RTSPDEBUG("[Info]Connected with %s:%d", socket_->GetRemoteAddr(), socket_->GetRemotePort());
    return 0;
}
