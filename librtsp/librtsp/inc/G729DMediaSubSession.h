#pragma once
#include "ServerMediaSubsession.h"

class G729DMediaSubSession :
    public ServerMediaSubSession
{
public:
    G729DMediaSubSession(string g729dfile, int payloadtype, unsigned short port);
    G729DMediaSubSession(MediaSource *mediasrc, int payloadtype, unsigned short port);
    ~G729DMediaSubSession(void);

public:
    virtual string SDPLines();
    virtual int    InitRTPSession();

protected:
    virtual unsigned long CalcNextSendTime();
    virtual int SendPacket();

private:

private:
    unsigned long   lastDuration_;
    unsigned long   lastTS_;
};
