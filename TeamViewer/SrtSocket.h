#pragma once
#include "windowsHeaders.h"
#include "packets.h"
#include <queue>
#include <thread>
#include <string>

#define FOUR_BITS 4
#define BYTE_IN_BITS 8

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
	template<typename nthSize>
	inline static nthSize networkToHost(const std::string& buffer, int index);
};

