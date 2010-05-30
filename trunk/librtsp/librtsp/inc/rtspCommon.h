/*!@file
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once

#include <string>
#include <map>
#include <iostream>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::vector;

// defines
#define MAX_BUF_SIZE 1024
#define MAX_PACKET_SIZE	((1024 * 64) - 1)
#define MILLION 1000000

#define RTSP_OK 0
//!< typedefs
namespace RTSP
{
    typedef enum _eRTSP_STATE
    {
        RTSP_INIT,
        RTSP_READY,
        RTSP_PLAYING,
        RTSP_RECORD,
        RTSP_NONE,
    }RTSP_STATE;

    typedef struct _sRTSPVERSION
    {
        int major;
        int minor;
    }RTSPVERSION;

    typedef enum _ePACKETTYPE
    {
        REQUEST_PKT,
        RESPONSE_PKT,
        NONE_PKT,
    }PACKETTYPE;

    const string SP   = " ";
    const string CRLF = "\r\n";

    const string MEDIATYPE_AUDIO = "audio";
    const string MEDIATYPE_VIDEO = "video";
};
namespace RTSP
{
    // Methods
    namespace Method
    {
        const string OPTIONS       = "OPTIONS";
        const string DESCRIBE      = "DESCRIBE";
        const string ANNOUNCE      = "ANNOUNCE";
        const string SETUP         = "SETUP";
        const string PLAY          = "PLAY";
        const string PAUSE         = "PAUSE";
        const string TEARDOWN      = "TEARDOWN";
        const string GET_PAPAMETER = "GET_PAPAMETER";
        const string SET_PAPAMETER = "SET_PAPAMETER";
        const string REDIRECT      = "REDIRECT";
        const string RECORD        = "RECORD";
        const string UNPPORTED     = "UNSPPORTED";
    }

    // Header Field
    namespace HeaderField
    {
        const string Accept             = "Accept";
        const string Accept_Encoding    = "Accept-Encoding";
        const string Accept_Language    = "Accept-Language";
        const string Allow              = "Allow";
        const string Authorization      = "Authorization";
        const string Bandwidth          = "Bandwidth";
        const string Blocksize          = "Blocksize";
        const string Cache_Control      = "Cache-Control";
        const string Conference         = "Conference";
        const string Connection         = "Connection";
        const string Content_Base       = "Content-Base";
        const string Content_Encoding   = "Content-Encoding";
        const string Content_Language   = "Content-Language";
        const string Content_Length     = "Content-Length";
        const string Content_Location   = "Content-Location";
        const string Content_Type       = "Content-Type";
        const string CSeq               = "CSeq";
        const string Date               = "Date";
        const string Expires            = "Expires";
        const string From               = "From";
        const string Host               = "Host";
        const string If_Match           = "If-Match";
        const string If_Modified_Since  = "If-Modified-Since";
        const string Last_Modified      = "Last-Modified";
        const string Location           = "Location";
        const string Proxy_Authenticate = "Proxy-Authenticate";
        const string Proxy_Require      = "Proxy-Require";
        const string Public             = "Public";
        const string Range              = "Range";
        const string Referer            = "Referer";
        const string Retry_After        = "Retry-After";
        const string Require            = "Require";
        const string RTP_Info           = "RTP-Info";
        const string Scale              = "Scale";
        const string Speed              = "Speed";
        const string Server             = "Server";
        const string Session            = "Session";
        const string Timestamp          = "Timestamp";
        const string Transport          = "Transport";
        const string Unsupported        = "Unsupported";
        const string User_Agent         = "User-Agent";
        const string Vary               = "Vary";
        const string Via                = "Via";
        const string WWW_Authenticate   = "WWW-Authenticate";
    }
    typedef struct _sSTATUS_CODE_MAP
    {
        string code;
        string reason;
    }STATUS_CODE_MAP;
};

static RTSP::STATUS_CODE_MAP codeAndReason[] =
{
    {"100", "Continue"},
    {"200", "OK"},
    {"201", "Created"},
    {"250", "Low on Storage Space"},
    {"300", "Multiple Choices"},
    {"301", "Moved Permanently"},
    {"302", "Moved Temporarily"},
    {"303", "See Other"},
    {"304", "Not Modified"},
    {"305", "Use Proxy"},
    {"400", "Bad Request"},
    {"401", "Unauthorized"},
    {"402", "Payment Required"},
    {"403", "Forbidden"},
    {"404", "Not Found"},
    {"405", "Method Not Allowed"},
    {"406", "Not Acceptable"},
    {"407", "Proxy Authentication Required"},
    {"408", "Request Time-out"},
    {"410", "Gone"},
    {"411", "Length Required"},
    {"412", "Precondition Failed"},
    {"413", "Request Entity Too Large"},
    {"414", "Request-URI Too Large"},
    {"415", "Unsupported Media Type"},
    {"451", "Parameter Not Understood"},
    {"452", "Conference Not Found"},
    {"453", "Not Enough Bandwidth"},
    {"454", "Session Not Found"},
    {"455", "Method Not Valid in This State"},
    {"456", "Header Field Not Valid for Resource"},
    {"457", "Invalid Range"},
    {"458", "Parameter Is Read-Only"},
    {"459", "Aggregate operation not allowed"},
    {"460", "Only aggregate operation allowed"},
    {"461", "Unsupported transport"},
    {"462", "Destination unreachable"},
    {"500", "Internal Server Error"},
    {"501", "Not Implemented"},
    {"000", "END"}
};

#ifdef _DEBUG
//#define
#else
#define NDEBUG
#endif
