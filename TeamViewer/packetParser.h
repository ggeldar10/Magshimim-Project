#pragma once
#include "packets.h"
#include "SrtSocket.h"

#define UDP_HEADERS_SIZE 8
#define UDP_HEADER_SIZE 16
#define IP_HEADERS_SIZE 20
#define IP_SRT_PROTOCOL_NUMBER 160
#define IP_VERSION_HEADER_SIZE 4
#define MAX_IP_SIZE 60
#define MIN_IP_SIZE 40
#define FOUR_BITS 4
#define BYTE_IN_BITS 8
#define IPV4 4
#define DEFAULT_TTL 64
#define UDP_PROTOCOL_CODE 17

class packetParser
{
public:
	template<typename nthSize>
	inline static nthSize networkToHost(const std::string& buffer, int index);

	IpPacket static createIpPacketFromString(const std::string& ipPacketBuffer);
	IpPacket static createIpPacket(IpPacketTypesOfServices serviceType, int totalLength, int packetID, int flags, int checksum, uint32_t srcAddr, uint32_t dstAddr);

	DefaultPacket static createDefualtPacket(DefaultPacketTypes type, uint32_t ackSequenceNumber, uint32_t packetSequenceNumber, time_t timeStamp);

	HandshakeControlPacket static createHandshakePacketFromString(const std::string& handshakePacketBuffer);

private:

};

