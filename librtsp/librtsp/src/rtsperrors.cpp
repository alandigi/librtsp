/*!@file rtsperrors.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.25
@version 1.0
*/

#include "rtsperrors.h"
#include "util.h"

typedef struct _sRTSPErrorInfo
{
    int code;
    std::string description;
}RTSPErrorInfo;

static RTSPErrorInfo RTSPErrorDescriptions[] =
{
    {ERR_RTSP_OPEN_FILE,     "Open File Error"},
    {ERR_RTSP_PARSER_DATA,   "Parser RTSP Data Error"},
    {ERR_RTSP_NOT_ENOUGH_BUF, "Not Enough Buffer for Storing Data"},
    {ERR_RTSP_READ_FILE,     "Read File Error"},
    {ERR_RTSP_GET_DATA,      "Get Data Failed"},
    {ERR_RTSP_CRAETE_RTP_SESSION, "Create RTP Session Failed"},
    {ERR_RTSP_PTHREAD_INIT, "pthread Init (cond/mutex etc.) Failed"},
    {ERR_RTSP_PTHREAD_CREATE, "pthread Create Failed"},
    {0, 0},
};

std::string RTSPGetErrorString( int errcode )
{
    int i;

    if (errcode >= 0)
        return std::string("No error");

    i = 0;
    while (RTSPErrorDescriptions[i].code != 0)
    {
        if (RTSPErrorDescriptions[i].code == errcode)
            return RTSPErrorDescriptions[i].description;
        i++;
    }

    return std::string("Unknown error code") + Utility::ToString(errcode);
}
