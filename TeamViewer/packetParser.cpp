#include "packetParser.h"

template<typename nthSize>
inline nthSize packetParser::networkToHost(const std::string& buffer, int index)
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

IpPacket packetParser::createIpPacketFromString(const std::string& ipPacketBuffer)
{
	if (ipPacketBuffer.length() > MAX_IP_SIZE || ipPacketBuffer.length() < MIN_IP_SIZE)
	{
		std::cerr << "Error: the buffer that was given is not valid" << std::endl;
		throw std::invalid_argument("Error: the buffer that was given is not valid");
	}
	uint8_t version;
	uint8_t lengthOfHeaders;
	uint8_t typeOfService;
	uint16_t totalLength;
	uint16_t identification;
	uint16_t fragmentOffsetIncludingFlags = 0;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t headerChecksum;
	uint32_t srcAddrs;
	uint32_t dstAddrs;
	unsigned char options[MAX_IP_OPTIONS_SIZE];
	int index = 0;
	// might be a problem of big and smalle endians so check when the srt is complete
	version = (ipPacketBuffer[index] & 0xF0) >> FOUR_BITS;
	lengthOfHeaders = ipPacketBuffer[index] & 0x0F;
	index += sizeof(uint8_t);
	typeOfService = networkToHost<uint8_t>(ipPacketBuffer, index);
	index += sizeof(uint8_t);
	totalLength = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	identification = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	ttl = networkToHost<uint8_t>(ipPacketBuffer, index);
	index += sizeof(uint8_t);
	protocol = networkToHost<uint8_t>(ipPacketBuffer, index);
	index += sizeof(uint8_t);
	headerChecksum = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	srcAddrs = networkToHost<uint32_t>(ipPacketBuffer, index);
	index += sizeof(uint32_t);
	dstAddrs = networkToHost<uint32_t>(ipPacketBuffer, index);
	index += sizeof(uint32_t);
	std::string help = ipPacketBuffer.substr(index);
	for (int i = 0; i < help.size() && i < MAX_IP_OPTIONS_SIZE; i++)
	{
		options[i] = help[i];
	}

	IpPacket packet = IpPacket(version, lengthOfHeaders, typeOfService, totalLength, identification, fragmentOffsetIncludingFlags, ttl, protocol, headerChecksum, srcAddrs, dstAddrs, options);
	return packet;
}

DefaultPacket packetParser::createDefaultPacketFromString(const std::string& defaultPacketBuffer)
{
	DefaultPacketTypes packetType;
	uint32_t ackSequenceNumber;
	uint32_t packetSequenceNumber;
	time_t timeStamp;

	packetType;
	ackSequenceNumber;
	packetSequenceNumber;
	timeStamp;
}




/*
* this turns the packets into one packet that we send
* it turns everysingle one of the headers into bytes
* and put it inside the given char array
* input:
* ipHeaders - the headers of the ip
* udpHeaders - the headers of the udp
* handshakeHeaders - the headers of the
* output:
* a vector with all the bytes of the packet in bigendian order
*/
std::vector<const char> packetParser::packetToBytes(const IpPacket& ipHeaders, const UdpPacket& udpHeaders, const DefaultPacket& srtHeaders, const std::vector<const char>* data)
{
	std::vector<const char> buffer;
	buffer.push_back((ipHeaders.getVersion() << FOUR_BITS) | ipHeaders.getLengthOfHeaders());
	buffer.push_back(ipHeaders.getTypeOfService());
	hostToNetworkIntoVector<uint16_t>(&buffer, ipHeaders.getTotalLength());
	hostToNetworkIntoVector<uint16_t>(&buffer, ipHeaders.getIdentification());
	hostToNetworkIntoVector<uint16_t>(&buffer, ipHeaders.getFragmentOffsetIncludingFlags());
	buffer.push_back(ipHeaders.getTtl());
	buffer.push_back(ipHeaders.getProtocol());
	hostToNetworkIntoVector<uint16_t>(&buffer, ipHeaders.getHeaderChecksum());
	hostToNetworkIntoVector<uint32_t>(&buffer, ipHeaders.getSrcAddrs());
	hostToNetworkIntoVector<uint32_t>(&buffer, ipHeaders.getDstAddrs());
	if (ipHeaders.getOptions())
	{
		for (int i = 0; i < ipHeaders.getLengthOfHeaders() * TURN_WORD_TO_BYTES_FACTOR - MIN_IP_SIZE; i++)
		{
			buffer.push_back(ipHeaders.getOptions()[i]);
		}
	}

	// todo srtHeaders.getBytes() to buffer

	for (int i = 0; i < data->size(); i++)
	{
		buffer.push_back(data->at(i));
	}

	return buffer;
}


/*
* insert the value to the vector in a big endian order
* input:
* addVector - a pointer to the vector to insert to
* value - the value to insert to the vector
* output: none
*/
template<typename htnSize>
inline void packetParser::hostToNetworkIntoVector(std::vector<const char>* addVector, htnSize value)
{
	htnSize andFactor = 0xFF << ((sizeof(htnSize) - 1) * BYTE_IN_BITS);
	for (int i = 0; i < sizeof(htnSize); i++)
	{
		addVector->push_back(value & andFactor);
		andFactor >>= BYTE_IN_BITS;
	}
}
