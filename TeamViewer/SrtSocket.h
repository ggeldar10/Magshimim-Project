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
	//
	// Fields
	//
	SOCKET _srtSocket;
	struct {
		uint16_t _srcPort;
		std::string _srcIP;
		uint16_t _dstPort;
		std::string _dstIP;
	} _commInfo;
	std::queue<std::string> _userRecvDataQueue; // for the recv to save the given information
	std::thread controlThread;
	//
	// Methods
	//
	void controlThreadFunction(); // we need to think how we implement it 


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
	
};

