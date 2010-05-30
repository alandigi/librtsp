/*!@file Logger.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.27
@version 1.0
*/
#include "Logger.h"
#include <fstream>
#include <ctime>
#include <cstring>

Logger Logger::logger_;

void Logger::save_to( const char *file )
{
    std::ofstream fileStream;
    fileStream.open(file, std::ios_base::out | std::ios_base::app);
    fileStream << buffer_;
    fileStream.close();
}

const char* Logger::c_str()
{
    return buffer_.c_str();
}

Logger::Logger()
{
}

void Logger::clear()
{
    buffer_ = "";
}

const std::string Logger::datetime()
{
    time_t tt = time(NULL);
    char buf[50];
    ::memset(buf, 0, sizeof(char) * 50);
    strftime(buf, sizeof buf, "%y-%m-%d %H:%M:%S", gmtime(&tt));
    return std::string(buf);
}

Logger* Logger::Instance()
{
    return &logger_;
}
