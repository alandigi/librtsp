/*!@file RTSPSessionBase.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "rtspCommon.h"
#include "RTSPSessionBase.h"

RTSPSessionBase::RTSPSessionBase(void)
: server_(0)
, socket_(0)
{
}

RTSPSessionBase::~RTSPSessionBase(void)
{
}
