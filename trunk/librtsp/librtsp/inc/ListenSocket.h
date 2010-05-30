/*!@file
@brief 

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once
#include "SocketBase.h"

class ListenSocket :
	public SocketBase
{
public:
	ListenSocket(void);
	~ListenSocket(void);

public:
    virtual  bool Create(unsigned short port);

    //! @brief Listen method of listening socket
    /**
    @return: bool
    */
    bool Listen();

    //! @brief set max num of accept connection request at the same time
    /**
    @param:  int count
    @return: void
    */
    void SetBackLog(int count);

    //! accept the connection
    /**
    @return: int socketfd
    */
    int  Accept();

private:
    int backlog_;
};
