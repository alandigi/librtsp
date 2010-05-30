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

#ifndef RTPUDPV4NATTRANSMITTER_H

#define RTPUDPV4NATTRANSMITTER_H

#include "rtpconfig.h"
#include "rtptransmitter.h"
#include "rtpipv4natdestination.h"
#include "rtphashtable.h"
#include "rtpkeyhashtable.h"
#include "rtptimeutilities.h"
#include <list>

#ifdef RTP_SUPPORT_THREAD
	#include <jmutex.h>
#endif // RTP_SUPPORT_THREAD

#define RTPUDPV4NATTRANS_HASHSIZE									8317
#define RTPUDPV4NATTRANS_DEFAULTPORTBASE								5000

class RTPUDPv4NATTransmissionParams : public RTPTransmissionParams
{
public:
	RTPUDPv4NATTransmissionParams():RTPTransmissionParams(RTPTransmitter::UserDefinedProto)	{ portbase = RTPUDPV4NATTRANS_DEFAULTPORTBASE; bindIP = 0; reflectIP = 0; reflectport = 0; }
	void SetUDPReflectorAddress(uint32_t ip,uint32_t port)					{ reflectIP = ip; reflectport = port; }
	void SetBindIP(uint32_t ip)								{ bindIP = ip; }
	void SetPortbase(uint16_t pbase)							{ portbase = pbase; }
	void SetLocalIPList(std::list<uint32_t> &iplist)					{ localIPs = iplist; } 
	void ClearLocalIPList()									{ localIPs.clear(); }
	uint32_t GetBindIP() const								{ return bindIP; }
	uint16_t GetPortbase() const								{ return portbase; }
	uint32_t GetUDPReflectorIP() const							{ return reflectIP; }
	uint16_t GetUDPReflectorPort() const							{ return reflectport; }
	const std::list<uint32_t> &GetLocalIPList() const					{ return localIPs; }
private:
	uint16_t portbase;
	uint32_t bindIP;
	uint32_t reflectIP;
	uint16_t reflectport;
	std::list<uint32_t> localIPs;
};

class RTPUDPv4NATTransmissionInfo : public RTPTransmissionInfo
{
public:
#if ! (defined(WIN32) || defined(_WIN32_WCE))
	RTPUDPv4NATTransmissionInfo(std::list<uint32_t> iplist,int rtpsock,int rtcpsock,uint32_t outIP,uint16_t outrtpport,uint16_t outrtcpport) : RTPTransmissionInfo(RTPTransmitter::UserDefinedProto) 
#else
	RTPUDPv4NATTransmissionInfo(std::list<uint32_t> iplist,SOCKET rtpsock,SOCKET rtcpsock,uint32_t outIP,uint16_t outrtpport,uint16_t outrtcpport) : RTPTransmissionInfo(RTPTransmitter::UserDefinedProto) 
#endif  // WIN32
												{ localIPlist = iplist; rtpsocket = rtpsock; rtcpsocket = rtcpsock; outsidertpport = outrtpport; outsidertcpport = outrtcpport; outsideIP = outIP; }

	~RTPUDPv4NATTransmissionInfo()								{ }
	std::list<uint32_t> GetLocalIPList() const						{ return localIPlist; }
#if ! (defined(WIN32) || defined(_WIN32_WCE))
	int GetRTPSocket() const								{ return rtpsocket; }
	int GetRTCPSocket() const								{ return rtcpsocket; }
#else
	SOCKET GetRTPSocket() const								{ return rtpsocket; }
	SOCKET GetRTCPSocket() const								{ return rtcpsocket; }
#endif // WIN32
	uint32_t GetOutsideIP() const								{ return outsideIP; }
	uint16_t GetOutsideRTPPort() const							{ return outsidertpport; }
	uint16_t GetOutsideRTCPPort() const							{ return outsidertcpport; }
private:
	std::list<uint32_t> localIPlist;
#if ! (defined(WIN32) || defined(_WIN32_WCE))
	int rtpsocket,rtcpsocket;
#else
	SOCKET rtpsocket,rtcpsocket;
#endif // WIN32
	uint32_t outsideIP;
	uint16_t outsidertpport,outsidertcpport;
};
	
class RTPUDPv4NATTrans_GetHashIndex_IPv4Dest
{
public:
	static int GetIndex(const RTPIPv4NATDestination &d)					{ return d.GetIP_HBO()%RTPUDPV4NATTRANS_HASHSIZE; }
};

class RTPUDPv4NATTrans_GetHashIndex_uint32_t
{
public:
	static int GetIndex(const uint32_t &k)							{ return k%RTPUDPV4NATTRANS_HASHSIZE; }
};

#define RTPUDPV4NATTRANS_HEADERSIZE						(20+8)
	
class RTPUDPv4NATTransmitter : public RTPTransmitter
{
public:
	RTPUDPv4NATTransmitter();
	~RTPUDPv4NATTransmitter();

	int Init(bool treadsafe);
	int Create(size_t maxpacksize,const RTPTransmissionParams *transparams);
	void Destroy();
	RTPTransmissionInfo *GetTransmissionInfo();

	int GetLocalHostName(uint8_t *buffer,size_t *bufferlength);
	bool ComesFromThisTransmitter(const RTPAddress *addr);
	size_t GetHeaderOverhead()							{ return RTPUDPV4NATTRANS_HEADERSIZE; }
	
	int Poll();
	int WaitForIncomingData(const RTPTime &delay,bool *dataavailable = 0);
	int AbortWait();
	
	int SendRTPData(const void *data,size_t len);
	int SendRTCPData(const void *data,size_t len);

	void ResetPacketCount();
	uint32_t GetNumRTPPacketsSent();
	uint32_t GetNumRTCPPacketsSent();
				
	int AddDestination(const RTPAddress &addr);
	int DeleteDestination(const RTPAddress &addr);
	void ClearDestinations();

	bool SupportsMulticasting()							{ return false; }
	int JoinMulticastGroup(const RTPAddress &addr)					{ return 0; }
	int LeaveMulticastGroup(const RTPAddress &addr)					{ return 0; }
	void LeaveAllMulticastGroups()							{ }

	int SetReceiveMode(RTPTransmitter::ReceiveMode m);
	int AddToIgnoreList(const RTPAddress &addr);
	int DeleteFromIgnoreList(const RTPAddress &addr);
	void ClearIgnoreList();
	int AddToAcceptList(const RTPAddress &addr);
	int DeleteFromAcceptList(const RTPAddress &addr);
	void ClearAcceptList();
	int SetMaximumPacketSize(size_t s);	
	
	bool NewDataAvailable();
	RTPRawPacket *GetNextPacket();
#ifdef RTPDEBUG
	void Dump();
#endif // RTPDEBUG
private:
	int CreateLocalIPList();
	bool GetLocalIPList_Interfaces();
	void GetLocalIPList_DNS();
	void AddLoopbackAddress();
	void FlushPackets();
	int PollSocket(bool rtp);
	int ProcessAddAcceptIgnoreEntry(uint32_t ip,uint16_t port);
	int ProcessDeleteAcceptIgnoreEntry(uint32_t ip,uint16_t port);
#ifdef RTP_SUPPORT_IPV4MULTICAST
	bool SetMulticastTTL(uint8_t ttl);
#endif // RTP_SUPPORT_IPV4MULTICAST
	bool ShouldAcceptData(uint32_t srcip,uint16_t srcport);
	void ClearAcceptIgnoreInfo();
	void SendToReflector(RTPTime curtime);
	
	bool init;
	bool created;
	bool waitingfordata;
#if (defined(WIN32) || defined(_WIN32_WCE))
	SOCKET rtpsock,rtcpsock;
#else // not using winsock
	int rtpsock,rtcpsock;
#endif // WIN32
	uint32_t bindIP;
	std::list<uint32_t> localIPs;
	uint16_t portbase;
	uint32_t outsideIP;
	uint16_t outsidertpport, outsidertcpport;
	RTPTransmitter::ReceiveMode receivemode;

	uint32_t reflectorIP;
	uint16_t reflectorport;
	RTPTime lastreflectsend;

	uint8_t *localhostname;
	size_t localhostnamelength;
	
	RTPHashTable<const RTPIPv4NATDestination,RTPUDPv4NATTrans_GetHashIndex_IPv4Dest,RTPUDPV4NATTRANS_HASHSIZE> destinations;
	std::list<RTPRawPacket*> rawpacketlist;

	size_t maxpacksize;

	class PortInfo
	{
	public:
		PortInfo() { all = false; }
		
		bool all;
		std::list<uint16_t> portlist;
	};

	RTPKeyHashTable<const uint32_t,PortInfo*,RTPUDPv4NATTrans_GetHashIndex_uint32_t,RTPUDPV4NATTRANS_HASHSIZE> acceptignoreinfo;

	// notification descriptors for AbortWait (0 is for reading, 1 for writing)
#if (defined(WIN32) || defined(_WIN32_WCE))
	SOCKET abortdesc[2];
#else
	int abortdesc[2];
#endif // WIN32
	int CreateAbortDescriptors();
	void DestroyAbortDescriptors();
	void AbortWaitInternal();
#ifdef RTP_SUPPORT_THREAD
	JMutex mainmutex,waitmutex;
	int threadsafe;
#endif // RTP_SUPPORT_THREAD

	uint32_t rtppackcount,rtcppackcount;
};

#endif // RTPUDPV4NATTRANSMITTER_H

