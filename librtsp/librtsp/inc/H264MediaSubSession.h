#pragma once

#include "ServerMediaSubsession.h"

class H264MediaSubSession :
    public ServerMediaSubSession
{
public:
    H264MediaSubSession(string m4efile, int payloadtype, unsigned short port);
    H264MediaSubSession(MediaSource *mediasrc, int payloadtype, unsigned short port);
    ~H264MediaSubSession(void);

public:
    virtual string SDPLines();
    virtual int    InitRTPSession();

protected:
    virtual unsigned long CalcNextSendTime();
    virtual int SendPacket();

private:
    void RTPSendH264(uint8_t * buf, int size, unsigned long tsinc);
    void H264SendNAL(const uint8_t * buf, int size, unsigned long tsinc, bool marker);
private:
    unsigned long   lastDuration_;
    unsigned long   lastTS_;

};
