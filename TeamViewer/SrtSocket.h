#pragma once
#include "headers.h"
#include <queue>
#include <thread>
#include <string>

struct ConnectionRequestInfo // change if needed
{
	uint16_t clientPort;
	uint16_t clientIP;
};

class SrtSocket
{
private:
	SOCKET _srtSocket;
	struct {
		uint16_t _srcPort;
		std::string _srcIP;
		uint16_t _dstPort;
		std::string _dstIP;
	} _packetInfo;
	std::queue<ConnectionRequestInfo> _connectionQueue; 
	std::thread _connectionThread;

public:
	SrtSocket();
	~SrtSocket();
	void listen();
	void srtBind(sockaddr_in* sockaddr);
	SrtSocket accept();
	void connection();
	void sendSrt();
	std::string recvSrt();
	
};

