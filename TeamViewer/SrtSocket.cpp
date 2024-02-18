#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SrtSocket.h"
#include <iostream>
#include "packetParser.h"
#include <vector>

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

		delete[] charArray;
	}
}

const std::unique_ptr<const DefaultPacket> SrtSocket::recvSrt()
{
	UdpPacket udpPacket = recvUdp();
	const int length = udpPacket.getLength();
	std::vector<char> buffer(length);

	int bytesReceived = recv(this->_srtSocket, buffer.data(), length, 0);

	if (bytesReceived < 0)
	{
		std::cerr << "Error while trying to get SRT packet" << std::endl;
		throw std::runtime_error("Error while trying to get SRT packet");
	}
	else if (bytesReceived == 0)
	{
		throw std::runtime_error("Connection closed while trying to receive SRT packet");
	}

	std::unique_ptr<const DefaultPacket> packet = PacketParser::createPacketFromVectorGlobal(buffer);

	return packet;
}

const UdpPacket SrtSocket::recvUdp()
{
	char buffer[UDP_HEADERS_SIZE] = { 0 };
	std::vector<char> bufferVector(UDP_HEADERS_SIZE + IP_HEADERS_SIZE);

	int bytesReceived = recv(this->_srtSocket, bufferVector.data(), UDP_HEADERS_SIZE + IP_HEADERS_SIZE, 0);

	if (bytesReceived < 0)
	{
		std::cerr << "Error while trying to get UDP header" << std::endl;
		throw std::runtime_error("Error while trying to get UDP header");
	}
	else if (bytesReceived == 0)
	{
		throw std::runtime_error("Connection closed while trying to receive UDP header");
	}

	// Assuming createUdpPacketFromVector is a static function in PacketParser
	UdpPacket udpPacketRecv = PacketParser::createUdpPacketFromVector(bufferVector);

	if (udpPacketRecv.getLength() != UDP_HEADERS_SIZE + HANDSHAKE_PACKET_SIZE)
	{
		throw std::runtime_error("Invalid UDP packet length");
	}

	return udpPacketRecv;
}
