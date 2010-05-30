/*

  This file is a part of JRTPLIB
  Copyright (c) 1999-2007 Jori Liesenborgs

  Contact: jori.liesenborgs@gmail.com

  This library was developed at the "Expertisecentrum Digitale Media"
  (http://www.edm.uhasselt.be), a research center of the Hasselt University
  (http://www.uhasselt.be). The library is based upon work done for 
  my thesis at the School for Knowledge Technology (Belgium/The Netherlands).

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

*/

#ifndef RTPIPV4NATADDRESS_H

#define RTPIPV4NATADDRESS_H

#include "rtpconfig.h"
#include "rtpaddress.h"
#include "rtptypes.h"

// NOTE: the second port should only be used when adding or removing destinations, not
//       for controlling accept or ignore addresses for example.

class RTPIPv4NATAddress : public RTPAddress
{
public:
	RTPIPv4NATAddress(uint32_t ip = 0, uint16_t port1 = 0,uint16_t port2 = 0):RTPAddress(UserDefinedAddress)	{ RTPIPv4NATAddress::ip = ip; RTPIPv4NATAddress::port1 = port1; RTPIPv4NATAddress::port2 = port2;}
	RTPIPv4NATAddress(const uint8_t ip[4],uint16_t port1 = 0,uint16_t port2 = 0):RTPAddress(UserDefinedAddress)	{ RTPIPv4NATAddress::ip = (uint32_t)ip[3]; RTPIPv4NATAddress::ip |= (((uint32_t)ip[2])<<8); RTPIPv4NATAddress::ip |= (((uint32_t)ip[1])<<16); RTPIPv4NATAddress::ip |= (((uint32_t)ip[0])<<24); RTPIPv4NATAddress::port1 = port1; RTPIPv4NATAddress::port2 = port2; }
	~RTPIPv4NATAddress()									{ }
	void SetIP(uint32_t ip)									{ RTPIPv4NATAddress::ip = ip; }
	void SetIP(const uint8_t ip[4])								{ RTPIPv4NATAddress::ip = (uint32_t)ip[3]; RTPIPv4NATAddress::ip |= (((uint32_t)ip[2])<<8); RTPIPv4NATAddress::ip |= (((uint32_t)ip[1])<<16); RTPIPv4NATAddress::ip |= (((uint32_t)ip[0])<<24); }
	void SetPort1(uint16_t port)								{ RTPIPv4NATAddress::port1 = port; }
	void SetPort2(uint16_t port)								{ RTPIPv4NATAddress::port2 = port; }
	uint32_t GetIP() const									{ return ip; }
	uint16_t GetPort1() const								{ return port1; }
	uint16_t GetPort2() const								{ return port2; }
	RTPAddress *CreateCopy() const;
	bool IsSameAddress(const RTPAddress *addr) const;
	bool IsFromSameHost(const RTPAddress *addr) const;
#ifdef RTPDEBUG
	std::string GetAddressString() const;
#endif // RTPDEBUG
private:
	uint32_t ip;
	uint16_t port1,port2;
};

#endif // RTPIPV4NATADDRESS_H

