/*!@file MediaSource.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.25
@version 1.0
*/
#pragma once
#include <string>
using std::string;
#define MILLION 1000000

class MediaSource
{
public:
    MediaSource(void);
    virtual ~MediaSource(void);

public:
    virtual const string GetConfig() = 0;
    virtual bool         Open() = 0;  //!< Open the source
    virtual bool         Close() = 0; //!< Close the source
    virtual int          GetMediaData(char *buf,
                                      unsigned int bufsize,
                                      unsigned long &duration,
                                      unsigned long &ts) = 0;
protected:
    bool isOpened_;
};
