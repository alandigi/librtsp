/*!@file MPEG4MediaSubSession.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.24
@version 1.0
*/
#pragma once
#include "ServerMediaSubsession.h"

class MPEG4MediaSubSession :
    public ServerMediaSubSession
{
public:
    MPEG4MediaSubSession(string m4efile, int payloadtype, unsigned short port);
    MPEG4MediaSubSession(MediaSource *mediasrc, int payloadtype, unsigned short port);
    ~MPEG4MediaSubSession(void);
public:
    virtual string SDPLines();
    virtual int    InitRTPSession();

protected:
    virtual unsigned long CalcNextSendTime();
    virtual int SendPacket();

private:
    int SplitAndSend(char * buf, int len);

private:
    unsigned long   lastDuration_;
    unsigned long   lastTS_;

};
