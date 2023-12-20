#pragma once
#include "headers.h"
#include <deque>
#include <thread>
#include <string>

struct ConnectionRequestInfo // change if needed
{
	short clientPort;
	short clientIP;
};

class SrtSocket
{
private:
	SOCKET _srtSocket;
	struct {
		short _srcPort;
		short _srcIP;
		short _dstPort;
		short _dstIP;
	} _packetInfo;
	std::deque<ConnectionRequestInfo> _connectionQueue;
	std::thread _connectionThread;

public:
	void listen();
	void bind();
	SrtSocket accept();
	void connection();
	void sendSrt();
	std::string recvSrt();
	
};

