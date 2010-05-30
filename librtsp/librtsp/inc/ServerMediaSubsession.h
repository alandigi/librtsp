/*!@file ServerMediaSubsession.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once

class RTPSession;
class MediaSource;

class ServerMediaSubSession
{
public:
    ServerMediaSubSession(int payloadtype, unsigned short port);
    ServerMediaSubSession(MediaSource *mediasrc, int payloadtype, unsigned short port);
    ~ServerMediaSubSession(void);

public:
    virtual string SDPLines() = 0;
    virtual string RTPInfo();
    virtual int    InitRTPSession();

    float Duration() const;
    bool  IsAlreadyUsed()
    {
        return ( addedCount_ > 0 );
    }
    void IncreUseCount()
    {
        ++addedCount_;
    }

    //! @brief Set Track ID
    /**
    @param:  int id
    @return: void
    */
    void SetTrackID(int id)
    {
        trackID_ = id;
    }

    //! @brief Get Track ID
    /**
    @return: int
    */
    int GetTrackID()
    {
        return trackID_;
    }

    //! @brief Track Name the this media session
    /**
    @return: string
    */
    string TrackName()
    {
        return mediaType_ + Utility::ToString(trackID_);
    }

    //! @brief port base of RTP Session
    /**
    @return: unsigned short
    */
    unsigned short PortBase()
    {
        return portNum_;
    }

    //! @brief Start Data Trans Thread
    /**
    @return: int
    */
    virtual int Start();

    //! @brief Stop Data Trans Thread
    /**
    @return: int
    */
    virtual int Stop();

    //! @brief get range SDP line
    /**
    @return: string
    */
    string GetRangeSDPLine() const;

    //! @brief add mcast destination
    /**
    @param:  string & dst IP addr with format "XXX.XXX.XXX.XXX"
    @return: void
    */
    void   AddDestination(string &dst);

    const int GetTTL() const
    {
        return ttl_;
    }

    void SetTTL(int ttl)
    {
        ttl_ = ttl;
    }

protected:
    virtual unsigned long CalcNextSendTime() = 0;
    virtual int SendPacket() = 0;

private:

    //! @brief Thread to trans data
    /**
    @param:  void * arg
    @return: void *
    */
    static void *TransThread(void *);
    void   Init(int payloadtype, unsigned short port);

protected:
    bool           bExit_;

    int            trackID_;
    bool           addedCount_;
    int            payloadType_;
    string         mediaType_;   // audio/video
    unsigned short portNum_;
    struct timeval nextSendTime_;
    RTPSession     theRTPSession_;
    string         destinationAddr_;
    pthread_t      threadid_;
    int            ttl_;
    MediaSource    *mediaSource_;
};
