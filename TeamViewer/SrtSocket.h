#pragma once
#include "packets.h"
#include <queue>
#include <thread>
#include <string>
#include <functional>

#define RECV_BUFFER_SIZE 1024

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
		unsigned int _otherComputerMaxTransmission;
		unsigned int _otherComputerMtu;
		/*sockaddr_in */

	} _commInfo; // add connection information here
	std::queue<std::string> _userRecvDataQueue; // for the recv to save the given information
	std::thread controlThread;
	//
	// Methods
	//
	void waitForValidPacket(std::function<bool(char*, int)> checkFunction, std::vector<char>* buffer);
	void controlThreadFunction(); // we need to think how we implement it 
	bool isValidIpv4Checksum(const IpPacket& ipPacket); // add data
	bool isValidIpHeaders(const IpPacket& ipHeaders);
	bool isValidUdpHeaders(const UdpPacket& udpHeaders);

public:
	//
	// Methods
	//
	SrtSocket();
	~SrtSocket();
	void listenAndAccept(); // needs to block the current thread and start the thread function 
	void srtBind(sockaddr_in* sockaddr);
	void connectToServer(sockaddr_in* addrs);
	void sendSrt(const DefaultPacket* packet);
	std::string recvSrt();
};

