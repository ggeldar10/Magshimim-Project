#pragma once
#include "winapi.h"
#include <string>

#define DATAGRAM_SIZE 8
#define EIGHT_BITS 8
#define DST_PORT_POSITION 2
#define LENGTH_POSITION 4

class UdpSocket
{
protected:
	SOCKET _socket;
	int _port = 0;

	void convertShortToChar(short num, char* str);
	
public:
	UdpSocket();
	~UdpSocket();
	virtual void sendMsg(std::string msg, sockaddr_in* to);
	virtual int recvieve(char* buffer, int len);
	virtual void bindUdpSocket(int port, std::string addr);
	virtual void bindUdpSocket(int port, sockaddr_in* addr);

};

