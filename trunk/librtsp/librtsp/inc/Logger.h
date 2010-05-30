/*!@file Logger.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.27
@version 1.0
*/
#pragma once

#include <sstream>
#include <string>


class Logger
{
public:
    Logger();
    static Logger* Instance();

    template <typename T>
    Logger& operator << (T value)
    {
        std::stringstream ss;
        ss << "[" << datetime() <<"]:" << value;
        buffer_.append( ss.str() );

        return *this;
    }

    //! @brief Save log to a file
    /**
    @param:  const char * file the file to save logs
    @return: void
    */
    void save_to(const char *file);

    //! @brief clear logs
    /**
    @return: void
    */
    void clear();

    const char* c_str();


    void set_file(const char *filename)
    {
        logfilename_ = filename;
    }

private:
    const std::string datetime();

private:
    static Logger logger_;
    std::string buffer_;
    std::string logfilename_;
};

//#define LOG
