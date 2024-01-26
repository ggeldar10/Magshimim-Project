#include "packetParser.h"

IpPacket createIpPacketFromString(const std::string& ipPacketBuffer)
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

IpPacket createIpPacket(IpPacketTypesOfServices serviceType, int totalLength, int packetID, int flags, int checksum, uint32_t srcAddr, uint32_t dstAddr)
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

HandshakeControlPacket createHandshakePacketFromString(const std::string& handshakePacketBuffer)
{
	if (handshakePacketBuffer.length() > MAX_IP_SIZE || handshakePacketBuffer.length() < MIN_IP_SIZE)
	{
		std::cerr << "Error: the buffer that was given is not valid" << std::endl;
		throw std::invalid_argument("Error: the buffer that was given is not valid");
	}

	HandshakeControlPacket handshakePacket;
	std::memset(&handshakePacket, 0, sizeof(handshakePacket));

	int index = 0;
	handshakePacket.hasEncryption = networkToHost<unsigned int>(handshakePacketBuffer, index);
	index += sizeof(handshakePacket.hasEncryption);

	handshakePacket.encryption_key = networkToHost<uint16_t>(handshakePacketBuffer, index);
	index += sizeof(handshakePacket.encryption_key);

	handshakePacket.windowSize = networkToHost<uint32_t>(handshakePacketBuffer, index);
	index += sizeof(handshakePacket.windowSize);

	handshakePacket.initialPacketSequenceNumber = networkToHost<uint32_t>(handshakePacketBuffer, index);
	index += sizeof(handshakePacket.initialPacketSequenceNumber);

	handshakePacket.maxTransmission = networkToHost<uint32_t>(handshakePacketBuffer, index);
	index += sizeof(handshakePacket.maxTransmission);

	handshakePacket.phase = static_cast<HandshakePhases>(networkToHost<uint8_t>(handshakePacketBuffer, index));

	return handshakePacket;
}


template<typename nthSize>
inline nthSize networkToHost(const std::string& buffer, int index)
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

IpPacket createIpPacketFromString(const std::string& ipPacketBuffer)
{
	return IpPacket();
}
