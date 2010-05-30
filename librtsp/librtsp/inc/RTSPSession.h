#pragma once
#include "rtspCommon.h"

using RTSP::RTSP_STATE;


class RTSPSession :
    public RTSPSessionBase
{
public:
    typedef int (RTSPSession::*Hanlder)(RTSPPacket &, RTSPPacket &);
    typedef map<string, Hanlder> HandlerMap;
    typedef HandlerMap::iterator HandlerMap_Iterrator;
    typedef HandlerMap::const_iterator const_HandlerMap_Iterrator;

//    typedef map< RTSP_STATE, Hanlder> StateMap;
//    typedef map< string, StateMap > FSMTable;
//    typedef struct _sFSMTable
//    {
//        RTSP_STATE state;     //! Current state
//        string     input;     //! Input
//        RTSP_STATE nextState; //! Next State
//        Hanlder    handler;   //! Action
//    }FSMTable;
public:
    RTSPSession(void);
    ~RTSPSession(void);
public:

    //! @brief Init RTSP Session
    /**
    @return: void
    */
    virtual void Init();

    //! @brief virtual func handle incoming data
    /**
    @return: int
    */
    virtual int EventHandler();

    //! @brief Set the current status of RTSP State
    /**
    @param:  RTSP::RTSP_STATE & status
    @return: void
    */
    void        SetStauts(RTSP::RTSP_STATE &status)
    {
        rtspStatus_ = status;
    }

    //! @brief Get RTSP State
    /**
    @return: RTSP::RTSP_STATE
    */
    RTSP::RTSP_STATE GetStatus()
    {
        return rtspStatus_;
    }

    //! @brief Get this session's ID
    /**
    @return: int
    */
    int  GetSessionID()
    {
        return socket_->GetSocketFD();
    }

private:

    //! @brief parse RTSP request string and generate according response
    /**
    @param:  char * buf
    @param:  int len
    @return: int
    */
    int  ParseRTSPRequestString(char *buf, int len);

    //! @brief handle OPTION Request
    /**
    @param:  RTSPPacket & requestpkt
    @param:  RTSPPacket & responsepkt
    @return: int
    */
    int  HandleOPTIONSRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt);

    //! @brief Handle SETUP Request
    /**
    @param:  RTSPPacket & requestpkt
    @param:  RTSPPacket & responsepkt
    @return: int
    */
    int  HandleSETUPRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt);

    //! @brief Handle PLAY Request
    /**
    @param:  RTSPPacket & requestpkt
    @param:  RTSPPacket & responsepkt
    @return: int
    */
    int  HandlePLAYRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt);

    //! @brief Handle DESCRIBE Request
    /**
    @param:  RTSPPacket & requestpkt
    @param:  RTSPPacket & responsepkt
    @return: int
    */
    int  HandleDESCRIBERequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt);

    //! @brief Handle TEARDOWN Request
    /**
    @param:  RTSPPacket & requestpkt
    @param:  RTSPPacket & responsepkt
    @return: int
    */
    int  HandleTEARDOWNRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt);

    //! @brief Handle Unspported Request
    /**
    @param:  RTSPPacket & requestpkt
    @param:  RTSPPacket & responsepkt
    @return: int
    */
    int  HandleUnspportedRequest(RTSPPacket &requestpkt, RTSPPacket &responsepkt);

    //! @brief Send Response Data
    /**
    @param:  RTSPPacket & pkt
    @return: int
    */
    int  SendResponse(RTSPPacket &pkt);

    //! @brief Add Date Header Line
    /**
    @param:  RTSPPacket & pkt
    @return: void
    */
    void AddDateInfo(RTSPPacket & pkt);

    //! @brief Init Handler Map, which is used for handling different request
    /**
    @return: void
    */
    void InitHandlerMap();

private:
    RTSP_STATE rtspStatus_;
    RTSPPacket packet_;
    HandlerMap handlerMap_;
    string     streamName_;
    //int        sessionId_;
};
