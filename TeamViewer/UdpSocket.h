#pragma once
#include "winapi.h"
#include <string>

class UdpSocket
{
protected:
	SOCKET _socket;
	int _port;

public:
	UdpSocket();
	~UdpSocket();
	/*virtual void sendMsgTo(std::string msg, sockaddr* to);
	virtual int recv(std::string buffer, int len);*/

};

