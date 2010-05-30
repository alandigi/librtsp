/*!@file
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once

#include "rtspCommon.h"
using RTSP::PACKETTYPE;
using RTSP::RTSPVERSION;

class RTSPPacket
{
public:
    typedef map<string, string> HeaderPair;
    typedef HeaderPair::iterator headerItor;
    typedef HeaderPair::const_iterator const_headerItor;
public:
    RTSPPacket(void);
    ~RTSPPacket(void);

public:

    //!  Fill this packet
    /**
    @param:  char * data pointer to data
    @param:  int len the length of data
    @return: void
    */
    void SetData(char *data, int len);

    //! @brief get packet data
    /**
    @return: const char * the pointer to real packet data
    */
    const string GetData();

    //! @brief is this packet received all data belong to this packet
    /**
    @return: bool
    */
    bool  IsCompletePacket() const
    {
        return isCompletePacket_;
    }

    //! @brief Get Request method
    /**
    @return: string
    */
    string GetRequestMethod()
    {
        return method_;
    }

    //! @brief Get Packet Type
    /**
    @return: PACKETTYPE
    */
    PACKETTYPE GetPktType()
    {
        return pktType_;
    }

    //! @brief Set Packet Type
    /**
    @param:  PACKETTYPE type
    @return: void
    */
    void SetPktType(PACKETTYPE type)
    {
        pktType_ = type;
    }

    //! @brief Get Status code
    /**
    @return: string
    */
    string GetStatusCode()
    {
        return statuscode_;
    }

    //! @brief Set Status Code
    /**
    @param:  string code
    @return: void
    */
    void SetStatusCode(string code)
    {
        statuscode_ = code;
    }

    //! Add a header field
    /**
    @param:  string property_name
    @param:  string property_value
    @return: void
    */
    void AddAHeaderLine(string property_name, string property_value);

    //! @brief dump packet data
    /**
    @return: void
    */
    void DumpPacket() const;

    //! @brief clear packet data
    /**
    @return: void
    */
    void Reset();

    //! @brief Get the Request-URI in the request line
    /**
    @return: string
    */
    string GetRequestURI()
    {
        return requestURI_;
    }

    //! @brief Get Header line value by header name
    /**
    @param:  string property_name header name
    @return: string  Header line value
    */
    string GetHeaderLine(string property_name)
    {
        return header_[property_name];
    }

    //! @brief functional description of the function
    /**
    @param:  string msgbody
    @return: void
    */
    void SetContent(string content)
    {
        content_ = content;
    }

private:

    //! Parser Request Line
    /**
    @param:  string str the content to be parsed
    @return: int parser result
    */
    int    ParserRequestLine(string str);

    //! Parser Request Header
    /**
    @param:  string str the content to be parsed
    @return: int parser result
    */
    int    ParserRequestHeader(string str);

    //! Add default header lines
    /**
    @return: void
    */
    void   AddDefaultHeader();

    //! @brief Get Status-Reason By Status-Code
    /**
    @param:  string code Status-Code
    @return: string  Status-Reason
    */
    string GetStatusReasonByCode(string code) const;

private:
    PACKETTYPE  pktType_;
    int         len_;
    bool        isCompletePacket_;
    int         setdatacout_;
    string      method_;
    string      requestURI_;
    string      statuscode_;
    HeaderPair  header_;
    string      content_;
    string      data_;
    RTSPVERSION rtspVersion_;
    int         contentLength_;
};
