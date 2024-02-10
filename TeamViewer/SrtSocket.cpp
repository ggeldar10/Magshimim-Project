#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SrtSocket.h"
#include <iostream>
#include "packetParser.h"
#include <vector>

/*
waits for the packet with the srt protocol and with the right port and ip
input: none
output: none
*/
void SrtSocket::waitForValidPacket()
{

}

void SrtSocket::controlThreadFunction()
{
	while (true)
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		DefaultControlPacket packet = DefaultControlPacket(-1, -1, currentTime, KEEPALIVE);
		sendSrt(&packet);
		Sleep(1000);
	}
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
	this->_commInfo = {0};

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
	char buffer[RECV_BUFFER_SIZE] = {0};
	std::string bufferString;
	while (packetNotFound)
	{
		if (recv(this->_srtSocket, buffer, UDP_HEADERS_SIZE + IP_HEADERS_SIZE + 1, MSG_PEEK) < 0)
		{
			std::cerr << "Error while trying to get connection" << std::endl;
			throw "Error while trying to get connection";
		}
		bufferString = buffer;
		IpPacket ipPacketRecv = PacketParser::createIpPacketFromString(bufferString.substr(0, IP_HEADERS_SIZE));
		if (!isValidIpHeaders(ipPacketRecv))
		{
			continue;
		}
		UdpPacket udpPacketRecv = PacketParser::createUdpPacketFromString(bufferString.substr(IP_HEADERS_SIZE, UDP_HEADERS_SIZE));

		HandshakeControlPacket handshakePacketRecv = PacketParser::createHandshakeControlPacketFromString(bufferString.substr(IP_HEADERS_SIZE + UDP_HEADERS_SIZE));

		this->_commInfo._otherComputerMaxTransmission = htonl(handshakePacketRecv.getMaxTransmission());
		this->_commInfo._otherComputerMtu = htonl(handshakePacketRecv.getWindowSize());
		this->_commInfo._dstIP = htonl(ipPacketRecv.getSrcAddrs());
		this->_commInfo._dstPort = htons(udpPacketRecv.getDstPort());
		packetNotFound = false;
	}

	HandshakeControlPacket handshakeSend = HandshakeControlPacket(2, 0, time(nullptr), false, 0, DEFUALT_MTU_SIZE, 0/*Change later*/, DEFUALT_MAX_TRANSMISSION, INDUCTION_2);
	UdpPacket udpPacketSend = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, UDP_HEADERS_SIZE/*not the right one*/, 0);
	IpPacket ipPacketSend = IpPacket(IPV4, IP_HEADERS_SIZE, 0, /*not the right one*/0, 0, 0, DEFAULT_TTL, IP_SRT_PROTOCOL_NUMBER, 0, this->_commInfo._srcIP, this->_commInfo._dstIP, nullptr);
	std::vector<char> sendBufferVector = PacketParser::packetToBytes(ipPacketSend, udpPacketSend, handshakeSend, nullptr);
	char* help = new char[sendBufferVector.size()];
	std::copy(sendBufferVector.begin(), sendBufferVector.end(), help);
	send(this->_srtSocket, help, sendBufferVector.size(), 0);// check if i need sendto or not
	delete[] help;

	memset(buffer, 0, RECV_BUFFER_SIZE);
	waitForValidPacket();
	recv(this->_srtSocket, buffer, RECV_BUFFER_SIZE, 0);
	bufferString = buffer;
	IpPacket ipPacketRecv = PacketParser::createIpPacketFromString(bufferString.substr(0, IP_HEADERS_SIZE));
	UdpPacket udpPacketRecv = PacketParser::createUdpPacketFromString(bufferString.substr(IP_HEADERS_SIZE, UDP_HEADERS_SIZE));
	HandshakeControlPacket handshakePacketRecv = PacketParser::createHandshakeControlPacketFromString(bufferString.substr(IP_HEADERS_SIZE + UDP_HEADERS_SIZE));
	
	// other logic here

	handshakeSend = HandshakeControlPacket(4, 0, time(nullptr), false, 0, DEFUALT_MTU_SIZE, 0/*Change later*/, DEFUALT_MAX_TRANSMISSION, SUMMARY_2);
	udpPacketSend = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, UDP_HEADERS_SIZE/*not the right one*/, 0);
	ipPacketSend = IpPacket(IPV4, IP_HEADERS_SIZE, 0, /*not the right one*/0, 0, 0, DEFAULT_TTL, IP_SRT_PROTOCOL_NUMBER, 0, this->_commInfo._srcIP, this->_commInfo._dstIP, nullptr);
	sendBufferVector = PacketParser::packetToBytes(ipPacketSend, udpPacketSend, handshakeSend, nullptr);
	help = new char[sendBufferVector.size()];
	std::copy(sendBufferVector.begin(), sendBufferVector.end(), help);
	send(this->_srtSocket, help, sendBufferVector.size(), 0);// check if i need sendto or not
	delete[] help;


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

void SrtSocket::connectToServer(sockaddr_in* addrs) //todo add the waitForValidPacket
{
	this->_commInfo._dstPort = addrs->sin_port;
	this->_commInfo._dstIP = addrs->sin_addr.s_addr;
	this->srtBind(addrs); 
	HandshakeControlPacket handshakeHeaders = HandshakeControlPacket(0, 0, std::time(nullptr), false, 0, /*find the defualt mtu on the pc*/ DEFUALT_MTU_SIZE, 0, /*figure out the max transmission */ DEFUALT_MAX_TRANSMISSION, INDUCTION_1); // todo create the handshake packet
	UdpPacket udpHeaders = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, sizeof(handshakeHeaders) + UDP_HEADER_SIZE, 0);
	//todo connect them all to one char with the inforamtion needed and do the others
	IpPacket ipHeaders = IpPacket(IPV4, MIN_IP_SIZE, 0, MIN_IP_SIZE + UDP_HEADERS_SIZE + sizeof(handshakeHeaders)/*might be wrong*/, 0, 0, DEFAULT_TTL, IP_SRT_PROTOCOL_NUMBER, 0, this->_commInfo._srcIP, this->_commInfo._dstIP, NULL);
	std::vector<char> sendBuffer = PacketParser::packetToBytes(ipHeaders, udpHeaders, handshakeHeaders, nullptr);
	sendto(this->_srtSocket, sendBuffer.data(), sendBuffer.size(), 0, reinterpret_cast<sockaddr*>(addrs), sizeof(addrs));
	char recvBuffer[RECV_BUFFER_SIZE];
	if (!recv(this->_srtSocket, recvBuffer, RECV_BUFFER_SIZE, 0))
	{
		std::cerr << "Error occured while doing handshake" << std::endl;
		throw "Error occured while doing handshake";
	}
	std::string parser = recvBuffer;
	uint8_t headerLength = parser[1] & 0x0F;
	ipHeaders = PacketParser::createIpPacketFromString(parser.substr(headerLength));
	if (!isValidIpHeaders(ipHeaders)) // maybe do a while loop to make sure it will get the right response
	{
		std::cerr << "Error in the ip packet recieved" << std::endl;
		throw "Error in the ip packet recieved";
	}
	udpHeaders = PacketParser::createUdpPacketFromString(parser.substr(headerLength, UDP_HEADERS_SIZE));
	if (!isValidUdpHeaders(udpHeaders))
	{
		std::cerr << "Error in the udp packet recieved" << std::endl;
		throw "Error in the udp packet recieved";
	}
	
	handshakeHeaders = PacketParser::createHandshakeControlPacketFromString(parser.substr(headerLength + UDP_HEADERS_SIZE));
	//do a check for the handshake headers 
	this->_commInfo._otherComputerMaxTransmission = handshakeHeaders.getMaxTransmission();
	this->_commInfo._otherComputerMtu = handshakeHeaders.getWindowSize();
	handshakeHeaders = HandshakeControlPacket(4, 0, time(nullptr), false, 0, DEFUALT_MTU_SIZE, 5, DEFUALT_MAX_TRANSMISSION, SUMMARY_1);
	if (!recv(this->_srtSocket, recvBuffer, RECV_BUFFER_SIZE, 0))
	{
		std::cerr << "Error occured while doing handshake" << std::endl;
		throw "Error occured while doing handshake";
	}
	parser = recvBuffer;
	headerLength = parser[1] & 0x0F;
	ipHeaders = PacketParser::createIpPacketFromString(parser.substr(headerLength));
	if (!isValidIpHeaders(ipHeaders)) // maybe do a while loop to make sure it will get the right response
	{
		std::cerr << "Error in the ip packet recieved" << std::endl;
		throw "Error in the ip packet recieved";
	}
	udpHeaders = PacketParser::createUdpPacketFromString(parser.substr(headerLength, UDP_HEADERS_SIZE));
	if (!isValidUdpHeaders(udpHeaders))
	{
		std::cerr << "Error in the udp packet recieved" << std::endl;
		throw "Error in the udp packet recieved";
	}
}

void SrtSocket::sendSrt(const DefaultPacket* packet) {
	std::vector<uint8_t> buffer = packet->toBuffer();
}


std::string SrtSocket::recvSrt()
{
	return std::string();
}
