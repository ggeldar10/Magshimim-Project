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

void SrtSocket::connectToServer() // bind the client to a port
{
}

void SrtSocket::sendSrt()
{
}

std::string SrtSocket::recvSrt()
{
	return std::string();
}

IpPacket SrtSocket::createIpPacketFromString(const std::string& ipPacketBuffer)
{
	if (ipPacketBuffer.length() > MAX_IP_SIZE || ipPacketBuffer.length() < MIN_IP_SIZE)
	{
		std::cerr << "Error: the buffer that was given is not valid" << std::endl;
		throw std::invalid_argument("Error: the buffer that was given is not valid");
	}
	IpPacket ipPacket;
	memset(&ipPacket, 0, sizeof(IpPacket));
	int index = 0;
	// might be a problem of big and smalle endians so check when the srt is complete
	ipPacket.version = (ipPacketBuffer[index] & 0xF0) >> FOUR_BITS;
	ipPacket.lengthOfHeaders = ipPacketBuffer[index] & 0x0F;
	index += sizeof(uint8_t);
	ipPacket.typeOfService = networkToHost<uint8_t>(ipPacketBuffer, index);
	index += sizeof(uint8_t);
	ipPacket.totalLength = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	ipPacket.identification = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	ipPacket.identification = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	ipPacket.ttl = networkToHost<uint8_t>(ipPacketBuffer, index);
	index += sizeof(uint8_t);
	ipPacket.protocol = networkToHost<uint8_t>(ipPacketBuffer, index);
	index += sizeof(uint8_t);
	ipPacket.headerChecksum = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	ipPacket.srcAddrs = networkToHost<uint32_t>(ipPacketBuffer, index);
	index += sizeof(uint32_t);
	ipPacket.dstAddrs = networkToHost<uint32_t>(ipPacketBuffer, index);
	index += sizeof(uint32_t);
	std::string help = ipPacketBuffer.substr(index);
	for (int i = 0; i < help.size() && i < MAX_IP_OPTIONS_SIZE; i++)
	{
		ipPacket.options[i] = help[i];
	}
	return ipPacket;
}

IpPacket SrtSocket::createIpPacket(IpPacketTypesOfServices serviceType, int totalLength, int packetID, int flags, int checksum, uint32_t srcAddr, uint32_t dstAddr)
{
	IpPacket packet;
	packet.version = IPV4;
	packet.lengthOfHeaders = IP_HEADERS_SIZE;
	packet.typeOfService = static_cast<uint8_t>(serviceType);
	packet.totalLength = htons(static_cast<uint16_t>(totalLength)); 
	packet.identification = htons(static_cast<uint16_t>(packetID)); 
	packet.fragmentOffsetIncludingFlags = htons(static_cast<uint16_t>(flags)); 
	packet.ttl = DEFAULT_TTL;
	packet.protocol = UDP_PROTOCOL_CODE;
	packet.headerChecksum = htons(static_cast<uint16_t>(checksum)); 
	packet.srcAddrs = srcAddr;
	packet.dstAddrs = dstAddr;

	std::memset(packet.options, 0, sizeof(packet.options));

	return packet;
}


template<typename nthSize>
inline nthSize SrtSocket::networkToHost(const std::string& buffer, int index)
{
	if (index + sizeof(nthSize) > buffer.size())
	{
		std::cerr << "Error buffer size is not big enough" << std::endl;
		throw "Error buffer size is not big enough";
	}
	nthSize networkToHostNum = 0;
	for (int i = 0; i < sizeof(nthSize); i++)
	{
		networkToHostNum = networkToHostNum << BYTE_IN_BITS;
		networkToHostNum = networkToHostNum | static_cast<nthSize>(buffer[index + i]);
	}
	if constexpr (sizeof(nthSize) >= sizeof(uint32_t))
	{
		return ntohl(networkToHostNum);
	}
	return ntohs(networkToHostNum);
}
