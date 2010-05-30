/*!@file SocketBase.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "SocketBase.h"
#include <string.h>

SocketBase::SocketBase(void)
:socket_(INVALID_SOCKET)
{
}

SocketBase::~SocketBase(void)
{
}

//! Creat socket
/**
@param:  unsigned int port
@param:  int type SOCK_STREAM or SOCK_DGRAM
@return: bool success or failure
*/
bool SocketBase::Create( unsigned short port, int type, bool reuse)
{
    socket_ = socket(AF_INET, type, 0);
    if (INVALID_SOCKET == socket_)
    {
        perror("socket");
        return false;
    }
    if (reuse)
    {
        int opval = 1;
        if ( -1 == setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char *)&opval, sizeof(int)) )
        {
            perror("setsockopt");
            return false;
        }
    }
    localAddr_.sin_family        = AF_INET;
    localAddr_.sin_port          = htons(port);
    localAddr_.sin_addr.s_addr   = INADDR_ANY;
    memset(&(localAddr_.sin_zero), 0, sizeof(char) * 8);

    int ret = bind(socket_, (struct sockaddr *)&localAddr_, sizeof(struct sockaddr));
    if ( -1 ==  ret )
    {
        perror("bind");
        return false;
    }
    return false;
}

//! Close socket
/**
@return: bool success or failure
*/
bool SocketBase::Close()
{
    if ( INVALID_SOCKET == socket_)
    {
        return true;
    }

#ifdef WIN32
    int ret = closesocket(socket_);
#else
    int ret = close(socket_);
#endif

    if ( -1 == ret )
    {
        perror("Close");
        return false;
    }
    return true;
}

//! get socket fd, used for select()
/**
@return: int the socket fd
*/
int SocketBase::GetSocketFD()
{
    return socket_;
}

char *SocketBase::GetRemoteAddr()
{
    return remoteAddr_.GetAddrStr();
}

unsigned short SocketBase::GetRemotePort()
{
    return remoteAddr_.Port();
}

char * SocketBase::GetLocalAddr()
{
    return inet_ntoa(localAddr_.sin_addr);
}

unsigned short SocketBase::GetLocalPort()
{
    return localAddr_.sin_port;
}
