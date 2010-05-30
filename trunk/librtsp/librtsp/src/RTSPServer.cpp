/*!@file RTSPServer.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "rtspCommon.h"
#include "RTSPListener.h"
#include "RTSPPacket.h"
#include "rtspCommon.h"
#include "RTSPServer.h"
#include <cstring>
#include "ServerMediasession.h"
#include "util.h"

RTSPServer::RTSPServer(unsigned short port)
 : listenPort_(port)
{
    exitThread_ = false;
    started_    = false;
    listener_   = 0;
    nfds_       = 0;
}

RTSPServer::~RTSPServer(void)
{
    if (listener_)
    {
        delete listener_;
        listener_ = NULL;
    }
    for ( RTSPSessionItor itor = rtspSessions_.begin(); itor != rtspSessions_.end(); ++itor )
    {
        DeleteRTSPSession(itor->first);
    }
    for ( MediaSessionTable::iterator itor = mediaSessions_.begin(); itor != mediaSessions_.end(); ++itor )
    {
        delete itor->second;
    }
}

bool RTSPServer::AddRTSPSession( int session_id, RTSPSessionBase *session )
{
    // Can not find the session with the given name
    if (rtspSessions_.find(session_id) == rtspSessions_.end())
    {
        rtspSessions_[session_id] = session;
        session->SetOwer(this);
        if (session_id > nfds_) // keep trace of the max fd
        {
            nfds_ = session_id;

        }
        FD_SET(session_id, &fdsets_);
        return true;
    }
    return false;
}

bool RTSPServer::DeleteRTSPSession(int session_id )
{
    FD_CLR(session_id, &fdsets_);
    rtspSessions_.erase(session_id);
    return true;
}

void RTSPServer::Start()
{
    if ( true == started_ )
    {
        RTSPDEBUG("[Info]RTSPServer Already Started!!");
    }
    else
    {
        RTSPDEBUG("[Info]RTSPServer Starting...");
    }
    FD_ZERO(&fdsets_);

    listener_   = new RTSPListener(listenPort_);
    listener_->SetOwer(this);
    int fd = listener_->GetSocketFD();
    AddRTSPSession(fd, listener_); // add itself to sessions

    nfds_ = fd; // keep trace of the max fd
    FD_SET(fd, &fdsets_);
    int err = pthread_create(&threadid_, NULL , ProcessThread, this);
    if ( 0 != err )
    {
       RTSPDEBUG("[Error]Create Process Thread ERROR");
       //return ERR_RTSP_PTHREAD_CREATE;
    }
    else
    {
        started_ = true;
        //RTSPDEBUG("[Info]Successfully Create Process Thread.");
    }
}

void  RTSPServer::Joint()
{
    void *tret = 0;
    RTSPDEBUG("[Info]Waiting thread : %u", pthread_self());
    int err = pthread_join(threadid_, &tret);
    if ( 0 != err )
    {
        RTSPDEBUG("[Error]Can not join with thread:%s", strerror(err) );
    }
}

void RTSPServer::Stop()
{
    exitThread_ = true;
    started_    = false;
    Joint();
}

void *RTSPServer::ProcessThread( void * arg)
{
    RTSPDEBUG("[Info]Thread %u Started.", pthread_self());
    RTSPServer *server = (RTSPServer *)arg;
    timeval interval;

    while (false == server->exitThread_)
    {
        interval.tv_sec =  2;
        interval.tv_usec = 0;
        server->BuildFDSETs();
        fd_set sets = server->fdsets_; // copy it
        int    nfds = server->nfds_;   // copy it
        int ret = select(nfds + 1, &sets, NULL, NULL,&interval);
        if (ret == 0)
        {
            continue;
        }
        else if ( -1 == ret )
        {
            perror("select");
            continue;
        }
        else
        {
            for (RTSPSessionItor itor = server->rtspSessions_.begin(); itor != server->rtspSessions_.end(); ++itor)
            {
                if (FD_ISSET(itor->first, &sets))
                {
                    if (0 > itor->second->EventHandler() )
                    {
                        //!< time out or connection closed
                        server->DeleteRTSPSession(itor->first);
                        //FIXME erase in map travel loop: valgrind warning
                    }
                }
            }
        }
    }
    return (void *)0;
}

bool RTSPServer::BuildFDSETs()
{
    FD_ZERO(&fdsets_);
    for (RTSPSessionItor itor = rtspSessions_.begin(); itor != rtspSessions_.end(); ++itor)
    {
        FD_SET(itor->first, &fdsets_);
    }
    return true;
}

bool RTSPServer::AddMediaSession( string streamname, ServerMediaSession *session )
{
    // Can not find the session with the given name
    if (mediaSessions_.find(streamname) == mediaSessions_.end())
    {
        mediaSessions_[streamname] = session;
        return true;
    }
    return false;
}

ServerMediaSession * RTSPServer::FindMediaSessionByStreamName( string streamname )
{
    return mediaSessions_[streamname];
}
