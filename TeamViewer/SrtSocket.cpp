#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SrtSocket.h"
#include <iostream>

void SrtSocket::controlThreadFunction()
{

}

bool SrtSocket::isValidIpv4Checksum(const IpPacket& ipPacket)
{
	return true; // todo make the check for the valid checksum
}

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

/*
intiates the communicatations and wait untill a client has
connected only one client at a time
does the handshake and the intilize the missing commInfo
input:
output:
*/
void SrtSocket::listenAndAccept()
{
	bool packetNotFound = true;
	char buffer[UDP_HEADERS_SIZE + IP_HEADERS_SIZE + 1] = { 0 };
	while (packetNotFound)
	{
		if (recv(this->_srtSocket, buffer, UDP_HEADERS_SIZE + IP_HEADERS_SIZE + 1, MSG_PEEK) < 0)
		{
			std::cerr << "Error while trying to get connection" << std::endl;
			throw "Error while trying to get connection";
		}
		std::string ipHeaders = buffer;
		ipHeaders = ipHeaders.substr(0, IP_HEADERS_SIZE);

		// todo make a check if the ip header protocol is the number we are using for ip 
		std::string udpHeaders = buffer;
		udpHeaders = udpHeaders.substr(IP_HEADERS_SIZE, UDP_HEADERS_SIZE);
		int srcPort = atoi(udpHeaders.substr(0, UDP_HEADER_SIZE).c_str());
		int dstPort = atoi(udpHeaders.substr(UDP_HEADER_SIZE, UDP_HEADER_SIZE).c_str());

	}
}

/*
binds the socket to a specific port and ip on the computer
input: a ptr to the addrs to bind to and the port to bind to
using the struct sockaddr_in
output:
*/
void SrtSocket::srtBind(sockaddr_in* addrs)
{
	if (bind(this->_srtSocket, (sockaddr*)&addrs, sizeof(addrs)) < 0)
	{
		std::cerr << "Error while trying to bind" << std::endl;
		throw "Error while trying to bind";
	}
	sockaddr_in output;
	if (addrs->sin_port == 0)
	{
		memset(&output, 0, sizeof(sockaddr_in));
		int sizeOfOutput = sizeof(sockaddr_in);
		if (getsockname(this->_srtSocket, (sockaddr*)&output, &sizeOfOutput) != 0)
		{
			std::cerr << "Error while trying to get the sock name";
			throw "Error while trying to get the sock name";
		}
	}
	this->_commInfo._srcPort = addrs->sin_port == 0 ? addrs->sin_port : output.sin_port;
	this->_commInfo._srcIP = addrs->sin_port == 0 ? inet_ntoa(addrs->sin_addr) : inet_ntoa(output.sin_addr);

}

void SrtSocket::connectToServer(sockaddr_in* addrs)
{
	this->_commInfo._dstPort = addrs->sin_port;
	this->_commInfo._dstIP = inet_ntoa(addrs->sin_addr);
	this->srtBind(addrs); 

}

void SrtSocket::sendSrt()
{
}

std::string SrtSocket::recvSrt()
{
	return std::string();
}
