#pragma once
#include "packets.h"
#include "packetParser.h"
#include <queue>
#include <thread>
#include <string>
#include <bitset>

class SrtSocket
{
private:
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
	void controlThreadFunction(); // we need to think how we implement it 
	bool isValidIpv4Checksum(const IpPacket& ipPacket);
	
public:
	SrtSocket();
	~SrtSocket();
	void listenAndAccept(); // needs to block the current thread and start the thread function 
	void srtBind(sockaddr_in* sockaddr);
	void connectToServer(sockaddr_in* addrs);
	void sendSrt();
	const DefaultPacket* recvSrt();
};

