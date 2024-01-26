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

IpPacket createIpPacketFromString(const std::string& ipPacketBuffer);
IpPacket createIpPacket(IpPacketTypesOfServices serviceType, int totalLength, int packetID, int flags, int checksum, uint32_t srcAddr, uint32_t dstAddr);

HandshakeControlPacket createHandshakePacketFromString(const std::string& handshakePacketBuffer);

template<typename nthSize>
inline nthSize networkToHost(const std::string& buffer, int index);

