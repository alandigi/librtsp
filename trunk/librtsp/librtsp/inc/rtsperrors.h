/*!@file rtsperrors.h
@brief 

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.25
@version 1.0
@ref     rtperrors.h
*/
#pragma once

#include <string>

//! @brief Returns a string describing the error code \c errcode.
/**
@param:  int errcode
@return: std::string
*/
std::string RTSPGetErrorString(int errcode);

#define ERR_RTSP_OPEN_FILE          (-18)
#define ERR_RTSP_PARSER_DATA        (-19)
#define ERR_RTSP_NOT_ENOUGH_BUF     (-20)
#define ERR_RTSP_READ_FILE          (-21)
#define ERR_RTSP_GET_DATA           (-22)
#define ERR_RTSP_CRAETE_RTP_SESSION (-23)
#define ERR_RTSP_PTHREAD_INIT       (-24)
#define ERR_RTSP_PTHREAD_CREATE     (-25)
#define ERR_RTSP_WRONG_INDEX        (-26)

