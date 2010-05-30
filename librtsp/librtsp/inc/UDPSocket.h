/*!@file UDPSocket.h
@brief 

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once
#include "SocketBase.h"

class UDPSocket :
	public SocketBase
{
public:
	UDPSocket(void);
	~UDPSocket(void);

public:
    bool Create(unsigned short port);
};
