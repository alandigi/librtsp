/*!@file ServerMediasession.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once
typedef enum _eSTREAMSTATUS
{
    STREAM_READY,
    STREAMING,
    STREAM_STOP,
}STREAMSTATUS;

class ServerMediaSubSession;

class ServerMediaSession
{
public:
    typedef vector<ServerMediaSubSession *> MediaSubSessions;
    typedef MediaSubSessions::iterator SubsessionIterator;
    typedef MediaSubSessions::const_iterator const_SubsessionIterator;
public:
    ServerMediaSession(string streamName = "VoIP_Stream",
        string InfoString = "VoIP Live",
        string DesString = "Streaming By VoIP Live System",
        string micsString = "");
    ~ServerMediaSession(void);

public:
    string GenerateSDPDescription(); //! based on the entire session

    string GetRTPInfo(string requestURI);


    //! @brief add multi-cast destination
    /**
    @param:  string dstaddr IP addr with format "XXX.XXX.XXX.XXX"
    @return: void
    */
    void   AddDestinationAddr(string dstaddr);

    //! @brief Get Stream Name
    /**
    @return: const string
    */
    const string SreamName() const
    {
        return streamName_;
    }

    //! @brief Add Sub Media Session
    /**
    @param:  ServerMediaSubSession *
    @return: bool
    */
    bool   AddSubsession(ServerMediaSubSession *);

    //! @brief duration
    /**
    @return: float
    // a result == 0 means an unbounded session (the default)
    // a result < 0 means: subSession durations differ; the result is -(the largest)
    // a result > 0 means: this is the duration of a bounded session
    */
    float  Duration() const;

    //! @brief Start Streaming
    /**
    @return: int
    */
    int    StartStream();

    //! @brief Stop Streaming
    /**
    @return: int
    */
    int    StopStream();


    //! @brief scale feature, to be supported in the future, sets "scale" to the actual supported scale
    /**
    @param:  float & scale
    @return: void
    */
    void   TestScaleFactor(float& scale);
    string GetDestinationAddr()
    {
        return destinationAddr_;
    }

    ServerMediaSubSession *FindSubSessionByTrackName(string trackname);

    void SetLoaclAddr(string addr);
private:
    string           localIPAddr_; // IP XXX.XXX.XXX.XXX
    string           streamName_;
    string           SDPInfoString_;
    string           SDPDescriptionString_;
    string           SDPmicsString_;
    MediaSubSessions subSessions_;
    string           destinationAddr_;
    STREAMSTATUS     streamingStatus_;
};
