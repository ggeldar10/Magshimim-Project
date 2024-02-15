#pragma once
#include "packets.h"
#include <queue>
#include <thread>
#include <string>
#include <functional>
#include <mutex>

#define RECV_BUFFER_SIZE 1024

class SrtSocket
{
private:
	SOCKET _srtSocket;
	struct
	{
		uint16_t _srcPort;
		uint32_t _srcIP;
		uint16_t _dstPort;
		uint32_t _dstIP;
		uint32_t _seqNum;
		unsigned int _otherComputerMaxWindowSize;
		unsigned int _otherComputerMTU;
		/*sockaddr_in */

	} _commInfo; // add connection information here
	std::queue<std::string> _userRecvDataQueue; // for the recv to save the given information
	std::queue<std::vector<uint8_t>> _packetSendQueue;
	std::mutex _packetSendQueueMtx;
	std::thread _controlThread;
	
	void waitForValidPacket(std::function<bool(char*, int)> checkFunction, std::vector<char>* buffer);
	void controlThreadFunction();
	bool isValidIpv4Checksum(const IpPacket& ipPacket); // add data
	bool isValidIpHeaders(const IpPacket& ipHeaders);
	bool isValidUdpHeaders(const UdpPacket& udpHeaders);
	void srtBind(sockaddr_in* sockaddr);
	void connectToServer(sockaddr_in* addrs);
	void sendSrt();
	const DefaultPacket* recvSrt();

public:
	SrtSocket();
	~SrtSocket();
	void listenAndAccept(); // needs to block the current thread and start the thread function 
	
};

