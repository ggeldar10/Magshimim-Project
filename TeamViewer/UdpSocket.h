#pragma once
#include "winapi.h"
#include <string>

#define FOUR_BYTES 4

class UdpSocket
{
protected:
	SOCKET _socket;
	int _port;
	
public:
	UdpSocket();
	~UdpSocket();
	virtual int sendTo(std::string msg, sockaddr* to);
	virtual int sendTo(char* msg, sockaddr* to);
	virtual int recvieve(char* buffer, int len);
	virtual void bindUdpSocket(int port, sockaddr* addrs);

};

