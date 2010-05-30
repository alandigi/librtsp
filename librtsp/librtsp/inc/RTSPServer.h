/*!@file RTSPServer.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once

#include "RTSPSessionBase.h"
#include "SocketBase.h"
#include <pthread.h>

class RTSPSessionBase;
class RTSPListener;
class ServerMediaSession;

class RTSPServer
{
public:
    typedef map<int, RTSPSessionBase *> RTSPSessionTable;
    typedef RTSPSessionTable::iterator RTSPSessionItor;
    typedef RTSPSessionTable::const_iterator Const_RTSPSessionItor;
    typedef map<string, ServerMediaSession *> MediaSessionTable;


public:
    RTSPServer(unsigned short port);
    ~RTSPServer(void);

    //! Start services, it created a thread
    /**
    @return: void
    */
    void Start();

    //! Let the thread jump out the loop, then stop
    /**
    @return: void
    */
    void Stop();

    //! thread func, process incoming connection request and incoming data of connected socket
    /**
    @param:  void * arg
    @return: void *
    */
    static void *ProcessThread(void *);

    //! @brief Add a Session to session list
    /**
    @param:  int session_id the id of session, in fact it is a socket fd
    @param:  RTSPSessionBase * session pointer to session
    @return: bool added success or not
    */
    bool AddRTSPSession(int session_id, RTSPSessionBase *session);

    //! @brief Delete a session from session list by its id
    /**
    @param:  int session_id the id of session, in fact it is a socket fd
    @return: bool
    */
    bool DeleteRTSPSession(int session_id);

    //! @brief Add A MediaSession
    /**
    @param:  string streamname
    @param:  ServerMediaSession * session
    @return: bool
    */
    bool AddMediaSession(string streamname, ServerMediaSession *session);

    //! @brief Find MediaSession by streamname
    /**
    @param:  string streamname
    @return: ServerMediaSession *
    */
    ServerMediaSession *FindMediaSessionByStreamName(string streamname);


    void SetLoaclIPAddr(string ip)
    {
        localIPAddr_ = ip;
    }

    string GetLocalIPAddr()
    {
        return localIPAddr_;
    }
private:

    //! End the thread
    /**
    @return: void
    */
    void Joint();

    //! build fd_set again, we need to call this before every 'select()'
    /**
    @return: bool
    */
    bool BuildFDSETs();

private:
    MediaSessionTable mediaSessions_;
    RTSPSessionTable  rtspSessions_;
    fd_set            fdsets_;
    int               nfds_;
    bool              exitThread_;
    bool              started_;
    pthread_t         threadid_;
    RTSPListener      *listener_;
    unsigned short    listenPort_;
    string            localIPAddr_;

};
