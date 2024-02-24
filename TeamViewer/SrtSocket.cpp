#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SrtSocket.h"
#include <iostream>
#include "packetParser.h"
#include <vector>

SrtSocket::SrtSocket()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "Error while trying to intialize startup" << std::endl;
		throw "Error while trying to intialize startup";
	}
	this->_srtSocket = socket(AF_INET, SOCK_RAW, IP_SRT_PROTOCOL_NUMBER);
	if (this->_srtSocket == INVALID_SOCKET)
	{
		std::cerr << "Error while trying to open a socket" << std::endl;
		throw "Error while trying to open a socket";
	}
	this->_commInfo = { 0 };

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
	std::unique_ptr<IpPacket> ipHeaders;
	std::unique_ptr<UdpPacket> udpHeaders;
	std::unique_ptr<const HandshakeControlPacket> srtHeaders;
	waitForValidPacket(nullptr, [&](char* buffer, int totalLength) -> bool
		{
			std::vector<char> bufferVec;
			bufferVec.assign(buffer, buffer + totalLength);
			ipHeaders = std::make_unique<IpPacket>(PacketParser::createIpPacketFromVector(bufferVec));
			udpHeaders = std::make_unique<UdpPacket>(PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeaders->getLengthOfHeaders(), bufferVec.begin() + ipHeaders->getLengthOfHeaders() + UDP_HEADERS_SIZE }));
			if (!isValidHeaders(*ipHeaders, *udpHeaders))
			{
				return false;
			}
			if (udpHeaders->getLength() != HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE)
			{
				return false;
			}
			srtHeaders = PacketParser::createHandshakeControlPacketFromVector({ bufferVec.begin() + ipHeaders->getLengthOfHeaders() + UDP_HEADERS_SIZE, bufferVec.end() }); // might have a problem
			if (srtHeaders->getPhase() != INDUCTION_1)
			{
				return false;
			}
			return true;
		});

	this->_commInfo._dstIP = ipHeaders->getSrcAddrs();
	this->_commInfo._dstPort = udpHeaders->getSrcPort();
	this->_commInfo._otherComputerMTU = srtHeaders->getMaxTransmissionUnit();
	this->_commInfo._otherComputerMaxWindowSize = srtHeaders->getWindowSize();

	sockaddr_in toAddr = { 0 };
	toAddr.sin_port = this->_commInfo._dstPort;
	toAddr.sin_addr.s_addr = this->_commInfo._dstIP;
	toAddr.sin_family = AF_INET;

	HandshakeControlPacket handshakeSend = HandshakeControlPacket(2, 0, time(nullptr), false, 0, DEFUALT_MAX_TRANSMISSION, 0, DEFUALT_MTU_SIZE, INDUCTION_2);
	UdpPacket udpPacketSend = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, UDP_HEADERS_SIZE + HANDSHAKE_PACKET_SIZE, 0);
	std::vector<char> sendBufferVector = PacketParser::packetToBytes(udpPacketSend, handshakeSend, nullptr);
	if (sendto(this->_srtSocket, sendBufferVector.data(), sendBufferVector.size(), 0, reinterpret_cast<sockaddr*>(&toAddr), sizeof(toAddr)) < 0)
	{
		std::cerr << "Error while doing sendto in listenAndAccept" << std::endl;
		throw /*todo throw the right error*/;
	}
	waitForValidPacket(nullptr, [&](char* buffer, int totalLength) -> bool
		{
			std::vector<char> bufferVec;
			bufferVec.assign(buffer, buffer + totalLength);
			ipHeaders = std::make_unique<IpPacket>(PacketParser::createIpPacketFromVector(bufferVec));
			udpHeaders = std::make_unique<UdpPacket>(PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeaders->getLengthOfHeaders(), bufferVec.begin() + ipHeaders->getLengthOfHeaders() + UDP_HEADERS_SIZE }));
			if (!isValidHeaders(*ipHeaders, *udpHeaders))
			{
				return false;
			}
			if (udpHeaders->getLength() != HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE)
			{
				return false;
			}
			srtHeaders = PacketParser::createHandshakeControlPacketFromVector({ bufferVec.begin() + ipHeaders->getLengthOfHeaders() + UDP_HEADERS_SIZE, bufferVec.end() }); // might have a problem
			if (srtHeaders->getPhase() != SUMMARY_1)
			{
				return false;
			}
			return true;
		});

	// other logic here

	handshakeSend = HandshakeControlPacket(4, 0, time(nullptr), false, 0, DEFUALT_MAX_TRANSMISSION, 0/*Change later*/, DEFUALT_MTU_SIZE, SUMMARY_2);
	udpPacketSend = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, UDP_HEADERS_SIZE + HANDSHAKE_PACKET_SIZE, 0);
	sendBufferVector = PacketParser::packetToBytes(udpPacketSend, handshakeSend, nullptr);
	sendto(this->_srtSocket, sendBufferVector.data(), sendBufferVector.size(), 0, reinterpret_cast<sockaddr*>(&toAddr), sizeof(toAddr));

}

void SrtSocket::connectToServer(sockaddr_in* addrs) //todo add the waitForValidPacket
{
	std::unique_ptr<IpPacket> ipHeadersRecv;
	std::unique_ptr<UdpPacket> udpHeadersRecv;
	std::unique_ptr<const HandshakeControlPacket> srtHeadersRecv;


	this->_commInfo._dstPort = addrs->sin_port;
	this->_commInfo._dstIP = addrs->sin_addr.s_addr;
	sockaddr_in bindToAddr = { 0 };
	bindToAddr.sin_addr.s_addr = INADDR_ANY;
	bindToAddr.sin_port = 0;
	bindToAddr.sin_family = AF_INET;
	this->srtBind(&bindToAddr);
	HandshakeControlPacket handshakeHeaders = HandshakeControlPacket(0, 0, std::time(nullptr), false, 0, DEFUALT_MAX_TRANSMISSION, 0, DEFUALT_MTU_SIZE, INDUCTION_1);
	UdpPacket udpHeaders = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, sizeof(handshakeHeaders) + UDP_HEADER_SIZE, 0);
	std::vector<char> sendBuffer = PacketParser::packetToBytes(udpHeaders, handshakeHeaders, nullptr);
	if (sendto(this->_srtSocket, sendBuffer.data(), sendBuffer.size(), 0, reinterpret_cast<sockaddr*>(addrs), sizeof(addrs)) < 0)
	{
		std::cerr << "Error while doing sendTo in the handshake" << std::endl;
		throw /*do an exception*/;
	}
	waitForValidPacket(nullptr, [&](char* buffer, int size) -> bool
		{
			std::vector<char> bufferVec;
			bufferVec.assign(buffer, buffer + size);
			ipHeadersRecv = std::make_unique<IpPacket>(PacketParser::createIpPacketFromVector(bufferVec));
			udpHeadersRecv = std::make_unique<UdpPacket>(PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeadersRecv->getLengthOfHeaders(), bufferVec.begin() + ipHeadersRecv->getLengthOfHeaders() + UDP_HEADERS_SIZE }));
			if (!isValidHeaders(*ipHeadersRecv, *udpHeadersRecv))
			{
				return false;
			}
			if (udpHeadersRecv->getLength() != HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE)
			{
				return false;
			}
			srtHeadersRecv = PacketParser::createHandshakeControlPacketFromVector({ bufferVec.begin() + ipHeadersRecv->getLengthOfHeaders() + UDP_HEADERS_SIZE, bufferVec.end() }); // might have a problem
			if (srtHeadersRecv->getPhase() != INDUCTION_2)
			{
				return false;
			}
			return true;
		});
	this->_commInfo._otherComputerMTU = handshakeHeaders.getMaxTransmissionUnit();
	this->_commInfo._otherComputerMaxWindowSize = handshakeHeaders.getWindowSize();


	handshakeHeaders = HandshakeControlPacket(4, 0, time(nullptr), false, 0, DEFUALT_MTU_SIZE, 5, DEFUALT_MAX_TRANSMISSION, SUMMARY_1);
	sendBuffer = PacketParser::packetToBytes(udpHeaders, handshakeHeaders, nullptr);
	if (sendto(this->_srtSocket, sendBuffer.data(), sendBuffer.size(), 0, reinterpret_cast<sockaddr*>(addrs), sizeof(addrs)) < 0)
	{
		std::cerr << "Error while doing sendTo in the handshake" << std::endl;
		throw /*do an exception*/;
	}
	waitForValidPacket(nullptr, [&](char* buffer, int size) -> bool
		{
			std::vector<char> bufferVec;
			bufferVec.assign(buffer, buffer + size);
			ipHeadersRecv = std::make_unique<IpPacket>(PacketParser::createIpPacketFromVector(bufferVec));
			udpHeadersRecv = std::make_unique<UdpPacket>(PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeadersRecv->getLengthOfHeaders(), bufferVec.begin() + ipHeadersRecv->getLengthOfHeaders() + UDP_HEADERS_SIZE }));
			if (!isValidHeaders(*ipHeadersRecv, *udpHeadersRecv))
			{
				return false;
			}
			if (udpHeadersRecv->getLength() != HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE)
			{
				return false;
			}
			srtHeadersRecv = PacketParser::createHandshakeControlPacketFromVector({ bufferVec.begin() + ipHeadersRecv->getLengthOfHeaders() + UDP_HEADERS_SIZE, bufferVec.end() }); // might have a problem
			if (srtHeadersRecv->getPhase() != INDUCTION_2)
			{
				return false;
			}
			return true;
		});
	// start control thread
}

void SrtSocket::sendSrt(const DefaultPacket * packet) {
	std::vector<uint8_t> buffer = packet->toBuffer();
}

const UdpPacket SrtSocket::recvUdp()
{
	// Assuming createUdpPacketFromVector is a static function in PacketParser
	UdpPacket udpPacketRecv = PacketParser::createUdpPacketFromVector(bufferVector);

	if (udpPacketRecv.getLength() != UDP_HEADERS_SIZE + HANDSHAKE_PACKET_SIZE)
	{
		throw std::runtime_error("Invalid UDP packet length");
	}

	return udpPacketRecv;
}

/*
waits for the packet with the srt protocol and with the right port and ip
input: isValid - the function that checks if the packet is valid it gets 
a buffer and the size of it 
and returns true if the packet is valid false if else
buffer - the buffer to get the valid packet into 
output: returns the valid packet inside the given buffer
*/
void SrtSocket::waitForValidPacket(std::vector<char>* buffer, std::function<bool(char*, int)> isValid)
{
	const int ipFirstHeaderRowLowerInArray = 2;
	const int ipFirstHeaderRowHigherInArray = 3;
	char ipFirstRow[IP_FIRST_HEADER_ROW] = { 0 };
	bool notValid = true;
	int totalLength = 0;
	while (notValid)
	{
		if (recv(this->_srtSocket, ipFirstRow, IP_FIRST_HEADER_ROW, MSG_PEEK) < 0)
		{
			std::cerr << "Error while doing recv. error number from wsagetlasterror: " << WSAGetLastError() << std::endl;
			//throw excetpion
		}

		totalLength = (ipFirstRow[ipFirstHeaderRowLowerInArray] << FOUR_BITS) | ipFirstRow[ipFirstHeaderRowHigherInArray];
		std::unique_ptr<char[]> allBuffer = std::make_unique<char[]>(totalLength);

		if(recv(this->_srtSocket, allBuffer.get(), totalLength, 0) < 0)
		{
			std::cerr << "Error while doing recv. error number from wsagetlasterror: " << WSAGetLastError() << std::endl;
			//throw excetpions
		}
		if (isValid(allBuffer.get(), totalLength))
		{
			if (buffer != nullptr)
			{
				for (int i = 0; i < totalLength; i++)
				{
					buffer->push_back(allBuffer[i]);
				}
			}
			notValid = false;
		}
	}
}

bool SrtSocket::isValidIpv4Checksum(const IpPacket& ipPacket)
{
	return true; // todo make the check for the valid checksum
}

bool SrtSocket::isValidIpHeaders(const IpPacket& ipHeaders)
{
	bool isSrtProtocol = ipHeaders.getProtocol() == IP_SRT_PROTOCOL_NUMBER;
	if ((this->_commInfo._dstIP == 0 || this->_commInfo._srcIP == 0) && isSrtProtocol)
	{
		return true;
	}
	if (!isSrtProtocol && this->_commInfo._srcIP != ipHeaders.getDstAddrs() && this->_commInfo._dstIP == ipHeaders.getSrcAddrs())
	{
		return false;
	}
	return true;
}

bool SrtSocket::isValidUdpHeaders(const UdpPacket& udpHeaders)
{
	//if (udpHeaders.getChecksum() == /*calculate checksum*/)
	//{

	//}
	if (this->_commInfo._srcPort == 0 || this->_commInfo._dstPort == 0)
	{
		return true;
	}
	if (udpHeaders.getDstPort() != this->_commInfo._srcPort || udpHeaders.getSrcPort() != this->_commInfo._dstPort)
	{
		return false;
	}
	return true;
}

bool SrtSocket::isValidHeaders(const IpPacket& ipHeaders, const UdpPacket& udpHeaders)
{
	return isValidIpHeaders(ipHeaders) && isValidUdpHeaders(udpHeaders);
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
	this->_commInfo._srcIP = addrs->sin_port != 0 ? addrs->sin_addr.s_addr : output.sin_addr.s_addr;

}




void SrtSocket::controlThreadFunction()
{
	DefaultControlPacket* packetPtr;
	while (true)
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		packetPtr = new DefaultControlPacket(-1, -1, currentTime, KEEPALIVE);
		std::unique_lock<std::mutex> lock(this->_packetSendQueueMtx);
		this->_packetSendQueue.push(packetPtr->toBuffer());
		lock.unlock();
		Sleep(1000);
	}
}

void SrtSocket::sendSrt() {
	std::lock_guard<std::mutex> lock(this->_packetSendQueueMtx);

	if (this->_packetSendQueue.size() != 0) {
		std::vector<uint8_t> dataBuffer = this->_packetSendQueue.front();
		this->_packetSendQueue.pop();

		UdpPacket udpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, dataBuffer.size(), 0);
		//udpPacket.setChecksum(calculateChecksum(udpBuffer, dataBuffer));
		std::vector<char> udpBuffer = udpPacket.toBuffer();

		std::vector<char> packetBuffer;
		packetBuffer.insert(packetBuffer.end(), udpBuffer.begin(), udpBuffer.end());
		packetBuffer.insert(packetBuffer.end(), dataBuffer.begin(), dataBuffer.end());

		char* charArray = new char[packetBuffer.size()];

		std::memcpy(charArray, packetBuffer.data(), packetBuffer.size());

		struct sockaddr_in dest_addr;
		std::memset(&dest_addr, 0, sizeof(dest_addr));
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(this->_commInfo._dstPort);
		dest_addr.sin_addr.s_addr = htonl(this->_commInfo._dstIP);

		sendto(this->_srtSocket, charArray, packetBuffer.size(), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
	}
}
