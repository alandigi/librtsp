/*!@file ListenSocket.cpp
@brief 

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "ListenSocket.h"

ListenSocket::ListenSocket(void)
: backlog_(5)
{
}

ListenSocket::~ListenSocket(void)
{
}

bool ListenSocket::Listen()
{
    int ret = listen(socket_, backlog_);
    if( -1 == ret)
    {
        perror("Listen");
        return false;
    }
    return true;
}

void ListenSocket::SetBackLog( int count )
{
    if (count > 0)
    {
        backlog_ = count;
    }
}

int ListenSocket::Accept()
{
    //int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
    socklen_t sin_size = remoteAddr_.GetSocketAddrLen();
    int ret = accept(socket_,remoteAddr_.GetSocketAddr(), &sin_size);
    if ( -1 == ret )
    {
        perror("accept");
    }
    else
    {
        return ret;
    }
    return -1;
}

bool ListenSocket::Create( unsigned short port )
{
    if (SocketBase::Create(port, SOCK_STREAM))
    {
        return true;
    }
    return false;
}
