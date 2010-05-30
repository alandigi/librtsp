/*!@file SocketBase.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once
#ifndef WIN32
// this is for Linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#else
#include <Winsock2.h>
#endif
#include "InetAddr.h"
class SocketBase
{
public:
	SocketBase(void);
	~SocketBase(void);
public:
    virtual  bool Create(unsigned short port) = 0;
    bool Close();
    int  GetSocketFD();
    void SetSocketFD(int fd)
    {
        socket_ = fd;
    }
    char *GetRemoteAddr();
    unsigned short GetRemotePort();

    char *GetLocalAddr();
    unsigned short GetLocalPort();
    // for listener socket
    virtual  bool Listen()
    {
        return false;
    }
    virtual  void SetBackLog(int count)
    {
    }
    virtual  int  Accept()
    {
        return INVALID_SOCKET;
    }
    // for TCP socket
    virtual bool Connect(InetAddr addr)
    {
        return false;
    }
    virtual int  Send(char *buf, int len)
    {
        return 0;
    }
    virtual int  Receive(char *buf, int len)
    {
        return 0;
    }

protected:
    bool Create(unsigned short port, int type, bool reuse = true);

    int         socket_;
    sockaddr_in localAddr_;
    InetAddr    remoteAddr_;
};
