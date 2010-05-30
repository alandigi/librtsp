/*!@file TCPSocket.h
@brief 

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once
#include "SocketBase.h"
#include "InetAddr.h"

class TCPSocket :
	public SocketBase
{
public:
	TCPSocket(void);
	~TCPSocket(void);
    bool Create(unsigned short port);
    bool Connect(InetAddr addr);
    int  Send(char *buf, int len);
    int  Receive(char *buf, int len);
};
