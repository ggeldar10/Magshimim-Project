#pragma once
#include "windowsHeaders.h"
#include "packets.h"
#include <queue>
#include <thread>
#include <string>
#include <bitset>

#define UDP_HEADERS_SIZE 8
#define UDP_HEADER_SIZE 16
#define IP_HEADERS_SIZE 20
#define IP_SRT_PROTOCOL_NUMBER 160
#define IP_VERSION_HEADER_SIZE 4
#define MAX_IP_OPTIONS_SIZE 10
#define MAX_IP_SIZE 60
#define MIN_IP_SIZE 40
#define FOUR_BITS 4
#define BYTE_IN_BITS 8
#define IPV4 4
#define DEFAULT_TTL 64
#define UDP_PROTOCOL_CODE 17

// todo later change to the packet file
enum ControlPacketType
{
	HANDSHAKE_PACKET = 0
};
enum IpPacketTypesOfServices { IPv4, IPv6, ICMPv4, ICMPv6, TCP, UDP, IGMP, IPsec, ARP, RARP };
/*
 0                   1
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Flags|     Fragment Offset     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        Header Checksum        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+         Source Address        +
|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |
+      Destination Address      +
|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            Options            |
+               +-+-+-+-+-+-+-+-+
|               |    Padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

class SrtSocket
{
private:
	//
	// Fields
	//
	SOCKET _srtSocket;
	struct
	{
		uint16_t _srcPort;
		std::string _srcIP;
		uint16_t _dstPort;
		std::string _dstIP;
		uint32_t _seqNum;
	} _commInfo; // add connection information here
	std::queue<std::string> _userRecvDataQueue; // for the recv to save the given information
	std::thread controlThread;
	//
	// Methods
	//
	void controlThreadFunction(); // we need to think how we implement it 

	bool isValidIpv4Checksum(const IpPacket& ipPacket);


public:
	//
	// Methods
	//
	SrtSocket();
	~SrtSocket();
	void listenAndAccept(); // needs to block the current thread and start the thread function 
	void srtBind(sockaddr_in* sockaddr);
	void connectToServer();
	void sendSrt();
	std::string recvSrt();
	static IpPacket createIpPacketFromString(const std::string& ipPacketBuffer);
	static IpPacket createIpPacket(IpPacketTypesOfServices serviceType, int totalLength, int packetID, int flags, int checksum, uint32_t srcAddr, uint32_t dstAddr);

	template<typename nthSize>
	inline static nthSize networkToHost(const std::string& buffer, int index);
};

