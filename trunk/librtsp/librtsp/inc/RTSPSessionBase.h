/*!@file RTSPSessionBase.h
@brief 

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once

#include "RTSPServer.h"
#include "SocketBase.h"

class RTSPServer;

class RTSPSessionBase
{
public:
    RTSPSessionBase(void);
    virtual ~RTSPSessionBase(void);

public:
    virtual int EventHandler() = 0;
    virtual void Init() = 0;
    void SetOwer(RTSPServer *ower)
    {
        server_ = ower;
    }
    void SetSocketFD(int fd)
    {
        socket_->SetSocketFD(fd);
    }
    int GetSocketFD()
    {
        return socket_->GetSocketFD();
    }

protected:
    RTSPServer *server_;
    SocketBase *socket_;
};
