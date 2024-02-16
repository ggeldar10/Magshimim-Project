#include "packetParser.h"

IpPacket PacketParser::createIpPacketFromString(const std::string& ipPacketBuffer)
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
	index += sizeof(uint16_t);
	totalLength = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	identification = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint8_t);
	ttl = networkToHost<uint8_t>(ipPacketBuffer, index);
	index += sizeof(uint8_t);
	protocol = networkToHost<uint8_t>(ipPacketBuffer, index);
	index += sizeof(uint16_t);
	headerChecksum = networkToHost<uint16_t>(ipPacketBuffer, index);
	index += sizeof(uint32_t);
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

DefaultPacket* PacketParser::createDefaultPacketFromString(const std::string& defaultPacketBuffer, int& index)
{
	DefaultPacketTypes packetType;
	uint32_t ackSequenceNumber;
	uint32_t packetSequenceNumber;
	time_t timeStamp;
	index = 0;

	packetType = static_cast<DefaultPacketTypes>((defaultPacketBuffer[index] & 0xF0) >> FOUR_BITS);
	index += sizeof(uint32_t);
	ackSequenceNumber = networkToHost<uint32_t>(defaultPacketBuffer, index);
	index += sizeof(uint32_t);
	packetSequenceNumber = networkToHost<uint32_t>(defaultPacketBuffer, index);
	index += sizeof(uint32_t);
	timeStamp = networkToHost<uint32_t>(defaultPacketBuffer, index);
	DefaultPacket* packet = new DefaultPacket(packetType, ackSequenceNumber, packetSequenceNumber, timeStamp);
	return packet;
}

DefaultDataPacket* PacketParser::createDefaultDataPacketFromString(const std::string& defaultDataPacketBuffer, int& index)
{
	DataPacketTypes dataPacketType;
	DefaultPacket* defaultPacket = createDefaultPacketFromString(defaultDataPacketBuffer, index);
	index += sizeof(DataPacketTypes);
	dataPacketType = static_cast<DataPacketTypes>((defaultDataPacketBuffer[index] & 0xF0) >> FOUR_BITS);
	DefaultDataPacket* packet = new DefaultDataPacket(defaultPacket->getAckSequenceNumber(), defaultPacket->getPacketSequenceNumber(), defaultPacket->getTimeStamp(), dataPacketType);
	return packet;
}

CursorDataPacket* PacketParser::createCursorDataPacketFromString(const std::string& cursorDataPacketBuffer)
{
	CursorActions action;
	int scrollValue;
	uint32_t x;
	uint32_t y;
	int index = 0;
	DefaultDataPacket* defaultDataPacket = createDefaultDataPacketFromString(cursorDataPacketBuffer, index);
	index += sizeof(CursorActions);
	action = static_cast<CursorActions>((cursorDataPacketBuffer[index] & 0xF0) >> FOUR_BITS);
	index += sizeof(uint32_t);
	x = networkToHost<uint32_t>(cursorDataPacketBuffer, index);
	index += sizeof(uint32_t);
	y = networkToHost<uint32_t>(cursorDataPacketBuffer, index);
	index += sizeof(uint32_t);
	scrollValue = networkToHost<uint32_t>(cursorDataPacketBuffer, index);

	CursorDataPacket* packet = new CursorDataPacket(defaultDataPacket->getAckSequenceNumber(), defaultDataPacket->getPacketSequenceNumber(), defaultDataPacket->getTimeStamp(), action, scrollValue, x, y);
	return packet;
}

KeyboardDataPacket* PacketParser::createKeyboardDataPacketFromString(const std::string& keyboardDataPacketBuffer)
{
	KeyboardActions action;
	unsigned int keyCode;
	int index = 0;
	DefaultDataPacket* defaultDataPacket = createDefaultDataPacketFromString(keyboardDataPacketBuffer, index);
	index += sizeof(KeyboardActions);
	action = static_cast<KeyboardActions>((keyboardDataPacketBuffer[index] & 0xF0) >> FOUR_BITS);
	index += sizeof(unsigned int);
	keyCode = networkToHost<unsigned int>(keyboardDataPacketBuffer, index);
	KeyboardDataPacket* packet = new KeyboardDataPacket(defaultDataPacket->getAckSequenceNumber(), defaultDataPacket->getPacketSequenceNumber(), defaultDataPacket->getTimeStamp(), action, keyCode);
	return packet;

}

DefaultControlPacket* PacketParser::createDefaultControlPacketFromString(const std::string& defaultControlPacketBuffer, int& index)
{
	ControlPacketTypes controlPacketType;
	DefaultPacket* defaultPacket = createDefaultPacketFromString(defaultControlPacketBuffer, index);
	index += sizeof(DataPacketTypes);
	controlPacketType = static_cast<ControlPacketTypes>((defaultControlPacketBuffer[index] & 0xF0) >> FOUR_BITS);
	DefaultControlPacket* packet = new DefaultControlPacket(defaultPacket->getAckSequenceNumber(), defaultPacket->getPacketSequenceNumber(), defaultPacket->getTimeStamp(), controlPacketType);
	return packet;
}

HandshakeControlPacket* PacketParser::createHandshakeControlPacketFromString(const std::string& handshakeControlPacketBuffer)
{
	bool isEncrypted;
	uint16_t encryption_key;
	uint32_t windowSize;
	uint32_t initialPacketSequenceNumber;
	uint32_t maxTransmission;
	HandshakePhases phase;
	int index = 0;
	DefaultControlPacket* defaultControlPacket = createDefaultControlPacketFromString(handshakeControlPacketBuffer, index);
	index += sizeof(bool);
	isEncrypted = networkToHost<unsigned int>(handshakeControlPacketBuffer, index);
	index += sizeof(uint16_t);
	encryption_key = networkToHost<uint16_t>(handshakeControlPacketBuffer, index);
	index += sizeof(uint32_t);
	windowSize = networkToHost<uint32_t>(handshakeControlPacketBuffer, index);
	index += sizeof(uint32_t);
	initialPacketSequenceNumber = networkToHost<uint32_t>(handshakeControlPacketBuffer, index);
	index += sizeof(uint32_t);
	maxTransmission = networkToHost<uint32_t>(handshakeControlPacketBuffer, index);
	index += sizeof(HandshakePhases);
	phase = static_cast<HandshakePhases>((handshakeControlPacketBuffer[index] & 0xF0) >> FOUR_BITS);
	HandshakeControlPacket* packet = new HandshakeControlPacket(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), isEncrypted, encryption_key, windowSize, initialPacketSequenceNumber, maxTransmission, phase);
	return packet;
}

NAKControlPacket* PacketParser::createNAKControlPacketFromString(const std::string& nakControlPacketBuffer) {
	ControlPacketTypes controlPacketType;
	uint32_t ackSequenceNumber;
	uint32_t packetSequenceNumber;
	time_t timeStamp;

	int index = 0;

	DefaultControlPacket* defaultControlPacket = createDefaultControlPacketFromString(nakControlPacketBuffer, index);
	controlPacketType = static_cast<ControlPacketTypes>((nakControlPacketBuffer[index] & 0xF0) >> FOUR_BITS);
	index += sizeof(ControlPacketTypes);
	ackSequenceNumber = networkToHost<uint32_t>(nakControlPacketBuffer, index);
	index += sizeof(uint32_t);
	packetSequenceNumber = networkToHost<uint32_t>(nakControlPacketBuffer, index);
	index += sizeof(uint32_t);
	timeStamp = networkToHost<uint32_t>(nakControlPacketBuffer, index);

	std::vector<unsigned int> lostSeqNums;
	while (index < nakControlPacketBuffer.size()) {
		lostSeqNums.push_back(networkToHost<unsigned int>(nakControlPacketBuffer, index));
		index += sizeof(unsigned int);
	}

	NAKControlPacket* packet = new NAKControlPacket(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), lostSeqNums);
	return packet;
}

MessageDropRequestControlPacket* PacketParser::createMessageDropRequestControlPacketFromString(const std::string& messageDropRequestControlPacketBuffer) {
	ControlPacketTypes controlPacketType;
	uint32_t ackSequenceNumber;
	uint32_t packetSequenceNumber;
	time_t timeStamp;

	int index = 0;

	DefaultControlPacket* defaultControlPacket = createDefaultControlPacketFromString(messageDropRequestControlPacketBuffer, index);
	controlPacketType = static_cast<ControlPacketTypes>((messageDropRequestControlPacketBuffer[index] & 0xF0) >> FOUR_BITS);
	index += sizeof(ControlPacketTypes);
	ackSequenceNumber = networkToHost<uint32_t>(messageDropRequestControlPacketBuffer, index);
	index += sizeof(uint32_t);
	packetSequenceNumber = networkToHost<uint32_t>(messageDropRequestControlPacketBuffer, index);
	index += sizeof(uint32_t);
	timeStamp = networkToHost<uint32_t>(messageDropRequestControlPacketBuffer, index);

	std::vector<unsigned int> lostSeqNums;
	while (index < messageDropRequestControlPacketBuffer.size()) {
		lostSeqNums.push_back(networkToHost<unsigned int>(messageDropRequestControlPacketBuffer, index));
		index += sizeof(unsigned int);
	}

	MessageDropRequestControlPacket* packet = new MessageDropRequestControlPacket(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), lostSeqNums);
	return packet;
}

UdpPacket PacketParser::createUdpPacketFromString(const std::string& udpPacketBuffer)
{
	uint16_t srcPort;
	uint16_t dstPort;
	uint16_t length;
	uint16_t checksum;

	int index = 0;

	srcPort = networkToHost<uint16_t>(udpPacketBuffer, index);
	index += sizeof(uint16_t);
	dstPort = networkToHost<uint16_t>(udpPacketBuffer, index);
	index += sizeof(uint16_t);
	length = networkToHost<uint16_t>(udpPacketBuffer, index);
	index += sizeof(uint16_t);
	checksum = networkToHost<uint16_t>(udpPacketBuffer, index);
	index += sizeof(uint16_t);

	UdpPacket udpPacket(srcPort, dstPort, length, checksum);

	return udpPacket;
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
std::vector<char> PacketParser::packetToBytes(const UdpPacket& udpHeaders, const DefaultPacket& srtHeaders, const std::vector<char>* data)
{
	std::vector<char> buffer = udpHeaders.toBuffer();

	for (char c : srtHeaders.toBuffer())
	{
		buffer.push_back(c);
	}

	for (int i = 0; i < data->size(); i++)
	{
		buffer.push_back(data->at(i));
	}

	return buffer;
}

DefaultPacket* PacketParser::createPacketFromStringGlobal(const std::string& globalPacketBuffer)
{
	int index = 0;
	DefaultPacket* packet = createDefaultPacketFromString(globalPacketBuffer, index);
	DefaultDataPacket* dataPacket;
	DefaultControlPacket* controlPacket;
	CursorDataPacket* cursorPacket;
	KeyboardDataPacket* keyboardPacket;
	HandshakeControlPacket* handshakePacket;
	NAKControlPacket* nakPacket;
	MessageDropRequestControlPacket* dropRequestPacket;

	switch (packet->getPacketType())
	{
	case DataPacket:
		index = 0;
		dataPacket = createDefaultDataPacketFromString(globalPacketBuffer, index);
		switch (dataPacket->getDataType())
		{
		case Cursor:
			cursorPacket = createCursorDataPacketFromString(globalPacketBuffer);
			return cursorPacket;
			break;
		case Keyboard:
			keyboardPacket = createKeyboardDataPacketFromString(globalPacketBuffer);
			return keyboardPacket;
			break;
		case Screen:
			//To Do
			break;
		case Chat:
			break;
		default:
			throw PacketParserException("Error: Invalid data type");
			break;
		}
		break;
	case ControlPacket:
		index = 0;
		controlPacket = createDefaultControlPacketFromString(globalPacketBuffer, index);
		switch (controlPacket->getControlType())
		{
		case HANDSHAKE:
			handshakePacket = createHandshakeControlPacketFromString(globalPacketBuffer);
			return handshakePacket;
			break;
		case KEEPALIVE:
			return controlPacket;
			break;
		case ACK:
			return controlPacket;
			break;
		case NAK:
			nakPacket = createNAKControlPacketFromString(globalPacketBuffer);
			return nakPacket;
			break;
		case CongestionWarning:
			return controlPacket;
			break;
		case SHUTDOWN:
			return controlPacket;
			break;
		case ACKACK:
			return controlPacket;
			break;
		case DROPREQ:
			dropRequestPacket = createMessageDropRequestControlPacketFromString(globalPacketBuffer);
			return dropRequestPacket;
			break;
		case PEERERROR:
			return controlPacket;
			break;
		default:
			throw PacketParserException("Error: Invalid control type");
			break;
		}
		break;
	default:
		throw PacketParserException("Error: Invalid packet type");
		break;
	}
	return nullptr;
}
