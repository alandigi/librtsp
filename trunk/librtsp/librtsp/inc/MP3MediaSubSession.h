/*!@file MP3MediaSubSession.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.24
@version 1.0
*/
#pragma once

#include "ServerMediaSubsession.h"

class ServerMediaSubSession;

class MP3MediaSubSession :
    public ServerMediaSubSession
{
public:
    MP3MediaSubSession(string mp3file, int payloadtype, unsigned short port);
    MP3MediaSubSession(MediaSource *mediasrc, int payloadtype, unsigned short port);
    ~MP3MediaSubSession(void);
public:

    //! @brief get media independent SDP lines
    /**
    @return: string
    */
    virtual string SDPLines();

    //! @brief Init RTSSession
    /**
    @return: int
    */
    virtual int    InitRTPSession();
protected:
    virtual unsigned long CalcNextSendTime();
    virtual int SendPacket();
private:
    unsigned long lastDuration_;
    unsigned long lastTS_;
};
