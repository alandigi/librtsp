/*!@file TCPSocket.cpp
@brief 

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "TCPSocket.h"

TCPSocket::TCPSocket(void)
{
}

TCPSocket::~TCPSocket(void)
{
}

//! @brief create a socket
/**
@param:  unsigned short port
@return: bool
*/
bool TCPSocket::Create( unsigned short port )
{
    if (SocketBase::Create(port, SOCK_STREAM))
    {
        return true;
    }
    return false;
}

//! @brief connect of TCP socket
/**
@param:  InetAddr addr
@return: bool
*/
bool TCPSocket::Connect( InetAddr addr )
{
    int ret = connect(socket_, addr.GetSocketAddr(), addr.GetSocketAddrLen());
    if ( -1 == ret )
    {
        perror("connect");
        return false;
    }
    remoteAddr_ = addr;
    return true;
}

//! @brief Send of TCP socket
/**
@param:  char * buf
@param:  int len
@return: int
*/
int TCPSocket::Send( char *buf, int len )
{
    int numbytes = 0;
    int totalnumbytes = 0;
    do
    {
        numbytes = send(socket_, buf+totalnumbytes, len - totalnumbytes, 0);
        if ( -1 == numbytes )
        {
            perror("send");
            break;
        }
        totalnumbytes += numbytes;
    }while(totalnumbytes < len);
    return numbytes;
}

//! @brief Receive of TCP socket
/**
@param:  char * buf
@param:  int len
@return: int
*/
int TCPSocket::Receive( char *buf, int len )
{
    int numbytes = recv(socket_, buf, len-1, 0);
    if ( -1 == numbytes)
    {
        perror("recv");
    }
    return numbytes;
}
