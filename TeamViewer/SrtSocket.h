#pragma once
#include "packets.h"
#include <queue>
#include <thread>
#include <string>
#include <bitset>

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
		uint32_t _srcIP;
		uint16_t _dstPort;
		uint32_t _dstIP;
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
	void connectToServer(sockaddr_in* addrs);
	void sendSrt();
	std::string recvSrt();
};

