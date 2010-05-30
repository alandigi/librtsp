/*!@file FileSource.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.24
@version 1.0
*/

#pragma once

class MediaSource;

class FileSource :
    public MediaSource
{
public:
    FileSource(string filenname);
    ~FileSource(void);
public:

    //! @brief Open the source, you can put initialization code here
    /**
    @return: bool
    */
    virtual bool Open() = 0;


    //! @brief Close the source, you can put de-initialization code here
    /**
    @return: bool
    */
    virtual bool Close() = 0;


    //! @brief Get Source Data
    /**
    @param:  char * buf  where to store the data
    @param:  unsigned int bufsize got size
    @param:  unsigned long & duration the play duration of the data got, unit is microsecond
    @param:  unsigned long & ts timestamp
    @return: int
    */
    virtual int  GetMediaData(char *buf,
                              unsigned int bufsize,
                              unsigned long &duration,
                              unsigned long &ts) = 0;

    //! @brief Get Config Infos
    /**
    @return: const string
    */
    virtual const string GetConfig() = 0;

protected:
    string filename_;
};
