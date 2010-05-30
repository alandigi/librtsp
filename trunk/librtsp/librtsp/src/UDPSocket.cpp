/*!@file UDPSocket.cpp
@brief 

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "UDPSocket.h"
#include "InetAddr.h"

UDPSocket::UDPSocket(void)
{
}

UDPSocket::~UDPSocket(void)
{
}

bool UDPSocket::Create( unsigned short port )
{
    if (SocketBase::Create(port, SOCK_DGRAM))
    {
        return true;
    }
    return false;
}
