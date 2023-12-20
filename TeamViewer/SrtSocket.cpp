#include "SrtSocket.h"
#include <iostream>

SrtSocket::SrtSocket()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "Error while trying to intialize startup" << std::endl;
		throw "Error while trying to intialize startup";
	}
	this->_srtSocket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (this->_srtSocket == INVALID_SOCKET)
	{
		std::cerr << "Error while trying to open a socket" << std::endl;
		throw "Error while trying to open a socket";
	}
}

SrtSocket::~SrtSocket()
{
	closesocket(this->_srtSocket);
	WSACleanup();
}

void SrtSocket::listen()
{
}

void SrtSocket::srtBind(sockaddr_in* addrs)
{
	if (bind(this->_srtSocket, (sockaddr*)&addrs, sizeof(addrs)) < 0)
	{
		std::cerr << "Error while trying to bind" << std::endl;
		throw "Error while trying to bind";
	}
	this->_packetInfo._srcPort = addrs->sin_port;
	this->_packetInfo._srcIP = inet_ntoa(addrs->sin_addr);
}

SrtSocket SrtSocket::accept() // bind the socket to a random port
{
	return SrtSocket();
}

void SrtSocket::connection()
{
}

void SrtSocket::sendSrt()
{
}

std::string SrtSocket::recvSrt()
{
	return std::string();
}
