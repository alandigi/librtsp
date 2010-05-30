#pragma once
#include "FileSource.h"
#include <cstdio>

#define G729D_FRAME_SIZE 8 // 8 BYTE

class G729DFileSource :
    public FileSource
{
public:
    G729DFileSource(string filename);
public:
    ~G729DFileSource(void);

public:
    //! @brief Open the source, you can put initialization code here
    /**
    @return: bool
    */
    virtual bool Open();


    //! @brief Close the source, you can put de-initialization code here
    /**
    @return: bool
    */
    virtual bool Close();


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
        unsigned long &ts);

    //! @brief Get Config Infos
    /**
    @return: const string
    */
    virtual const string GetConfig();
private:
    FILE         *g729dfp_;
    unsigned int framecount_;
};
