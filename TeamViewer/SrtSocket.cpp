#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SrtSocket.h"
#include <iostream>
#include "packetParser.h"
#include <vector>

void SrtSocket::controlThreadFunction()
{
}

bool SrtSocket::isValidIpv4Checksum(const IpPacket& ipPacket)
{
	return true; // todo make the check for the valid checksum
}

bool SrtSocket::isValidIpHeaders(const IpPacket& ipHeaders)
{
	if (ipHeaders.getDstAddrs() != this->_commInfo._srcIP || ipHeaders.getSrcAddrs() != this->_commInfo._dstIP)
	{
		return false;
	}
	if (ipHeaders.getProtocol() != IP_SRT_PROTOCOL_NUMBER)
	{
		return false;
	}
	return true;
}

bool SrtSocket::isValidUdpHeaders(const UdpPacket& udpHeaders)
{
	if (udpHeaders.getDstPort() != this->_commInfo._srcPort || udpHeaders.getSrcPort() != this->_commInfo._dstPort)
	{
		return false;
	}
	return true;
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
		//int srcPort = atoi(udpHeaders.substr(0, UDP_HEADER_SIZE).c_str());
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
	this->_commInfo._srcPort = addrs->sin_port != 0 ? addrs->sin_port : output.sin_port;
	this->_commInfo._srcIP = addrs->sin_port != 0 ?addrs->sin_addr.s_addr : output.sin_addr.s_addr;

}

void SrtSocket::connectToServer(sockaddr_in* addrs)
{
	this->_commInfo._dstPort = addrs->sin_port;
	this->_commInfo._dstIP = addrs->sin_addr.s_addr;
	this->srtBind(addrs); 
	HandshakeControlPacket handshakeHeaders = HandshakeControlPacket(0, 0, std::time(nullptr), false, 0, /*find the defualt mtu on the pc*/ DEFUALT_MTU_SIZE, 0, /*figure out the max transmission */ DEFUALT_MAX_TRANSMISSION, INDUCTION_1); // todo create the handshake packet
	UdpPacket udpHeaders = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, sizeof(handshakeHeaders) + UDP_HEADER_SIZE, 0);
	//todo connect them all to one char with the inforamtion needed and do the others
	IpPacket ipHeaders = IpPacket(IPV4, MIN_IP_SIZE, 0, MIN_IP_SIZE + UDP_HEADERS_SIZE + sizeof(handshakeHeaders)/*might be wrong*/, 0, 0, DEFAULT_TTL, IP_SRT_PROTOCOL_NUMBER, 0, this->_commInfo._srcIP, this->_commInfo._dstIP, NULL);
	std::vector<const char> sendBuffer = packetParser::packetToBytes(ipHeaders, udpHeaders, handshakeHeaders, nullptr);
	sendto(this->_srtSocket, sendBuffer.data(), sendBuffer.size(), 0, reinterpret_cast<sockaddr*>(addrs), sizeof(addrs));
	char recvBuffer[RECV_BUFFER_SIZE];
	if (!recv(this->_srtSocket, recvBuffer, RECV_BUFFER_SIZE, 0))
	{
		std::cerr << "Error occured while doing handshake" << std::endl;
		throw "Error occured while doing handshake";
	}
	std::string parser = recvBuffer;
	uint8_t headerLength = parser[1] & 0x0F;
	ipHeaders = packetParser::createIpPacketFromString(parser.substr(headerLength));
	if (!isValidIpHeaders(ipHeaders))
	{
		std::cerr << "Error in the ip packet recieved" << std::endl;
		throw "Error in the ip packet recieved";
	}
	udpHeaders = packetParser::createUdpPacketFromString(parser.substr(headerLength, UDP_HEADERS_SIZE));
	if (!isValidUdpHeaders(udpHeaders))
	{
		std::cerr << "Error in the udp packet recieved" << std::endl;
		throw "Error in the udp packet recieved";
	}
	handshakeHeaders = packetParser::createHandshakePacketFromString(parser, headerLength + UDP_HEADERS_SIZE);




	
	




}

void SrtSocket::sendSrt()
{
}

std::string SrtSocket::recvSrt()
{
	return std::string();
}
