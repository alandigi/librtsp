/*!@file InetAddr.h
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#pragma once
#ifndef WIN32
// this is for Linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <netdb.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#else
#include <Winsock2.h>
#endif

class InetAddr
{
public:
	InetAddr(unsigned long ipaddr = 0, unsigned short port = 0)
	{
        socketaddr_.sin_family      = AF_INET;
		socketaddr_.sin_port		= port;
		socketaddr_.sin_addr.s_addr = ipaddr;
	}

    InetAddr(const char *hostname, unsigned short port)
    {
        SetHostName(hostname);        // Get IP By hostname
        socketaddr_.sin_port = port;
    }

	InetAddr(const InetAddr& src)
	{
		socketaddr_ = src.socketaddr_;
	}

	InetAddr & operator=(const InetAddr & src)
	{
		socketaddr_ = src.socketaddr_;
		return *this;
	}
	~InetAddr(void);

    //! @brief Get the IP address as a unsigned long number
    /**
    @return: unsigned long
    */
    unsigned long  IPAddr()
    {
        return socketaddr_.sin_addr.s_addr;
    }

    //! @brief get port num of the address
    /**
    @return: unsigned short
    */
    unsigned short Port()
    {
        return socketaddr_.sin_port;
    }

    sockaddr *GetSocketAddr()
    {
        return (sockaddr *)&socketaddr_;
    }

    int GetSocketAddrLen()
    {
        return sizeof(sockaddr);
    }

    //! @brief Get the IP address as a string
    /**
    @return: char *
    */
    char *GetAddrStr();
private:
    //! @brief
    /**
    @param:  const char * hostname
    @return: void
    */
    void SetHostName(const char *hostname);

private:
	sockaddr_in socketaddr_;
};
