/*!@file InetAddr.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "InetAddr.h"
#include <string.h>

InetAddr::~InetAddr(void)
{
}

void InetAddr::SetHostName( const char *hostname )
{
    in_addr inaddr;
    inaddr.s_addr = inet_addr(hostname);
    if ( INADDR_NONE == inaddr.s_addr )
    {
        hostent* hp = gethostbyname(hostname);
        if ( 0 == hp )
        {
            // report error
            return;
        }
        memcpy(&inaddr, hp->h_addr, hp->h_length);
        socketaddr_.sin_family = hp->h_addrtype;
    }
    else
    {
        socketaddr_.sin_family = AF_INET;
        socketaddr_.sin_addr   = inaddr;
    }
}

char *InetAddr::GetAddrStr()
{
    return inet_ntoa(socketaddr_.sin_addr);
}
