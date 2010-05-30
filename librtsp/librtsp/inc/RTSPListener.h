/*!@file RTSPListener.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once

#include "RTSPSessionBase.h"

class RTSPListener :
    public RTSPSessionBase
{
public:
    RTSPListener(unsigned short port);
    ~RTSPListener(void);
public:

    //! @brief process incoming request
    /**
    @return: int
    */
    virtual int EventHandler();

    virtual void Init()
    {
    }

private:
    unsigned short port_;
};
