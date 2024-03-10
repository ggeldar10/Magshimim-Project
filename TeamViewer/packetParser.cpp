#include "packetParser.h"

IpPacket PacketParser::createIpPacketFromVector(const std::vector<char>& ipPacketBuffer)
{
    if (ipPacketBuffer.size() < MIN_IP_SIZE)
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
    fragmentOffsetIncludingFlags = networkToHost<uint16_t>(ipPacketBuffer, index);
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
    std::vector<char> help(ipPacketBuffer.begin() + index, ipPacketBuffer.end());
    for (int i = 0; i < help.size() && i < MAX_IP_OPTIONS_SIZE; i++)
    {
        options[i] = help[i];
    }
    IpPacket packet = IpPacket(version, lengthOfHeaders, typeOfService, totalLength, identification, fragmentOffsetIncludingFlags, ttl, protocol, headerChecksum, srcAddrs, dstAddrs, options);
    return packet;
}

std::unique_ptr<const DefaultPacket> PacketParser::createDefaultPacketFromVector(const std::vector<char>& defaultPacketBuffer, int& index)
{
    DefaultPacketTypes packetType;
    uint32_t ackSequenceNumber;
    uint32_t packetSequenceNumber;
    time_t timeStamp;
    index = 0;

    packetType = static_cast<DefaultPacketTypes>(networkToHost<uint32_t>(defaultPacketBuffer, index));
    index += sizeof(uint32_t);
    ackSequenceNumber = networkToHost<uint32_t>(defaultPacketBuffer, index);
    index += sizeof(uint32_t);
    packetSequenceNumber = networkToHost<uint32_t>(defaultPacketBuffer, index);
    index += sizeof(uint32_t);
    timeStamp = networkToHost<uint32_t>(defaultPacketBuffer, index);
    index += sizeof(uint32_t);
    std::unique_ptr<const DefaultPacket> packet = std::make_unique<DefaultPacket>(packetType, ackSequenceNumber, packetSequenceNumber, timeStamp);
    return packet;
}

std::unique_ptr<const DefaultDataPacket> PacketParser::createDefaultDataPacketFromVector(const std::vector<char>& defaultDataPacketBuffer, int& index)
{
    DataPacketTypes dataPacketType;
    std::unique_ptr<const DefaultPacket> defaultPacket = createDefaultPacketFromVector(defaultDataPacketBuffer, index);
    dataPacketType = static_cast<DataPacketTypes>((defaultDataPacketBuffer[index] & 0xF0) >> FOUR_BITS);
    index += sizeof(DataPacketTypes);
    std::unique_ptr<const DefaultDataPacket> packet = std::make_unique<DefaultDataPacket>(defaultPacket->getAckSequenceNumber(), defaultPacket->getPacketSequenceNumber(), defaultPacket->getTimeStamp(), dataPacketType);
    return packet;
}

std::unique_ptr<const CursorDataPacket> PacketParser::createCursorDataPacketFromVector(const std::vector<char>& cursorDataPacketBuffer)
{
    CursorActions action;
    int scrollValue;
    uint32_t x;
    uint32_t y;
    int index = 0;
    std::unique_ptr<const DefaultDataPacket> defaultDataPacket = createDefaultDataPacketFromVector(cursorDataPacketBuffer, index);
    action = static_cast<CursorActions>((cursorDataPacketBuffer[index] & 0xF0) >> FOUR_BITS);
    index += sizeof(CursorActions);
    x = networkToHost<uint32_t>(cursorDataPacketBuffer, index);
    index += sizeof(uint32_t);
    y = networkToHost<uint32_t>(cursorDataPacketBuffer, index);
    index += sizeof(uint32_t);
    scrollValue = networkToHost<uint32_t>(cursorDataPacketBuffer, index);
    index += sizeof(uint32_t);
    std::unique_ptr<const CursorDataPacket> packet = std::make_unique<CursorDataPacket>(defaultDataPacket->getAckSequenceNumber(), defaultDataPacket->getPacketSequenceNumber(), defaultDataPacket->getTimeStamp(), action, scrollValue, x, y);
    return packet;
}

std::unique_ptr<const KeyboardDataPacket> PacketParser::createKeyboardDataPacketFromVector(const std::vector<char>& keyboardDataPacketBuffer)
{
    KeyboardActions action;
    unsigned int keyCode;
    int index = 0;
    std::unique_ptr<const DefaultDataPacket> defaultDataPacket = createDefaultDataPacketFromVector(keyboardDataPacketBuffer, index);
    action = static_cast<KeyboardActions>((keyboardDataPacketBuffer[index] & 0xF0) >> FOUR_BITS);
    index += sizeof(KeyboardActions);
    keyCode = networkToHost<unsigned int>(keyboardDataPacketBuffer, index);
    index += sizeof(unsigned int);
    std::unique_ptr<const KeyboardDataPacket> packet = std::make_unique<KeyboardDataPacket>(defaultDataPacket->getAckSequenceNumber(), defaultDataPacket->getPacketSequenceNumber(), defaultDataPacket->getTimeStamp(), action, keyCode);
    return packet;
}

std::unique_ptr<const DefaultControlPacket> PacketParser::createDefaultControlPacketFromVector(const std::vector<char>& defaultControlPacketBuffer, int& index)
{
    ControlPacketTypes controlPacketType;
    std::unique_ptr<const DefaultPacket> defaultPacket = createDefaultPacketFromVector(defaultControlPacketBuffer, index);
    controlPacketType = static_cast<ControlPacketTypes>(networkToHost<uint16_t>(defaultControlPacketBuffer, index));
    index += sizeof(uint16_t);
    std::unique_ptr<const DefaultControlPacket> packet = std::make_unique<DefaultControlPacket>(defaultPacket->getAckSequenceNumber(), defaultPacket->getPacketSequenceNumber(), defaultPacket->getTimeStamp(), controlPacketType);
    return packet;
}

std::unique_ptr<const HandshakeControlPacket> PacketParser::createHandshakeControlPacketFromVector(const std::vector<char>& handshakeControlPacketBuffer)
{
    bool isEncrypted;
    uint16_t encryption_key;
    uint32_t windowSize;
    uint32_t initialPacketSequenceNumber;
    uint32_t maxTransmission;
    HandshakePhases phase;
    int index = 0;
    std::unique_ptr<const DefaultControlPacket> defaultControlPacket = createDefaultControlPacketFromVector(handshakeControlPacketBuffer, index);
    
    encryption_key = networkToHost<uint16_t>(handshakeControlPacketBuffer, index);
    index += sizeof(uint16_t);
    windowSize = networkToHost<uint32_t>(handshakeControlPacketBuffer, index);
    index += sizeof(uint32_t);
    initialPacketSequenceNumber = networkToHost<uint32_t>(handshakeControlPacketBuffer, index);
    index += sizeof(uint32_t);
    maxTransmission = networkToHost<uint32_t>(handshakeControlPacketBuffer, index);
    index += sizeof(uint32_t);
    phase = static_cast<HandshakePhases>(networkToHost<uint32_t>(handshakeControlPacketBuffer, index));
    index += sizeof(uint32_t);
    std::unique_ptr<const HandshakeControlPacket> packet = std::make_unique<HandshakeControlPacket>(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), encryption_key != 0, encryption_key, windowSize, initialPacketSequenceNumber, maxTransmission, phase);
    return packet;
}

std::unique_ptr<const NAKControlPacket> PacketParser::createNAKControlPacketFromVector(const std::vector<char>& nakControlPacketBuffer)
{
    ControlPacketTypes controlPacketType;
    uint32_t ackSequenceNumber;
    uint32_t packetSequenceNumber;
    time_t timeStamp;

    int index = 0;

    std::unique_ptr<const DefaultControlPacket> defaultControlPacket = createDefaultControlPacketFromVector(nakControlPacketBuffer, index);
    controlPacketType = static_cast<ControlPacketTypes>((nakControlPacketBuffer[index] & 0xF0) >> FOUR_BITS);
    ackSequenceNumber = networkToHost<uint32_t>(nakControlPacketBuffer, index);
    index += sizeof(ControlPacketTypes);
    packetSequenceNumber = networkToHost<uint32_t>(nakControlPacketBuffer, index);
    index += sizeof(uint32_t);
    timeStamp = networkToHost<uint32_t>(nakControlPacketBuffer, index);
    index += sizeof(uint32_t);

    std::vector<unsigned int> lostSeqNums;
    while (index < nakControlPacketBuffer.size())
    {
        lostSeqNums.push_back(networkToHost<unsigned int>(nakControlPacketBuffer, index));
        index += sizeof(unsigned int);
    }

    std::unique_ptr<const NAKControlPacket> packet = std::make_unique<NAKControlPacket>(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), lostSeqNums);
    return packet;
}

std::unique_ptr<const MessageDropRequestControlPacket> PacketParser::createMessageDropRequestControlPacketFromVector(const std::vector<char>& messageDropRequestControlPacketBuffer)
{
    ControlPacketTypes controlPacketType;
    uint32_t ackSequenceNumber;
    uint32_t packetSequenceNumber;
    time_t timeStamp;

    int index = 0;

    std::unique_ptr<const DefaultControlPacket> defaultControlPacket = createDefaultControlPacketFromVector(messageDropRequestControlPacketBuffer, index);
    controlPacketType = static_cast<ControlPacketTypes>((messageDropRequestControlPacketBuffer[index] & 0xF0) >> FOUR_BITS);
    ackSequenceNumber = networkToHost<uint32_t>(messageDropRequestControlPacketBuffer, index);
    index += sizeof(ControlPacketTypes);
    packetSequenceNumber = networkToHost<uint32_t>(messageDropRequestControlPacketBuffer, index);
    index += sizeof(uint32_t);
    timeStamp = networkToHost<uint32_t>(messageDropRequestControlPacketBuffer, index);
    index += sizeof(uint32_t);

    std::vector<unsigned int> lostSeqNums;
    while (index < messageDropRequestControlPacketBuffer.size())
    {
        lostSeqNums.push_back(networkToHost<unsigned int>(messageDropRequestControlPacketBuffer, index));
        index += sizeof(unsigned int);
    }

    std::unique_ptr<const MessageDropRequestControlPacket> packet = std::make_unique<MessageDropRequestControlPacket>(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), lostSeqNums);
    return packet;
}

UdpPacket PacketParser::createUdpPacketFromVector(const std::vector<char>& udpPacketBuffer)
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

std::vector<char> PacketParser::packetToBytes(const UdpPacket& udpHeaders, const DefaultPacket& srtHeaders, const std::vector<char>* data)
{
    std::vector<char> buffer = udpHeaders.toBuffer();

    for (char c : srtHeaders.toBuffer())
    {
        buffer.push_back(c);
    }
    if (data != nullptr)
    {
        for (int i = 0; i < data->size(); i++)
        {
            buffer.push_back(data->at(i));
        }
    }
   

    return buffer;
}

std::unique_ptr<const DefaultPacket> PacketParser::createPacketFromVectorGlobal(const std::vector<char>& globalPacketBuffer)
{
    int index = 0;
    std::unique_ptr<const DefaultPacket> packet = createDefaultPacketFromVector(globalPacketBuffer, index);
    std::unique_ptr<const DefaultDataPacket> dataPacket;
    std::unique_ptr<const DefaultControlPacket> controlPacket;
    std::unique_ptr<const CursorDataPacket> cursorPacket;
    std::unique_ptr<const KeyboardDataPacket> keyboardPacket;
    std::unique_ptr<const HandshakeControlPacket> handshakePacket;
    std::unique_ptr<const NAKControlPacket> nakPacket;
    std::unique_ptr<const MessageDropRequestControlPacket> dropRequestPacket;

    switch (packet->getPacketType())
    {
    case DataPacket:
        index = 0;
        dataPacket = createDefaultDataPacketFromVector(globalPacketBuffer, index);
        switch (dataPacket->getDataType())
        {
        case Cursor:
            cursorPacket = createCursorDataPacketFromVector(globalPacketBuffer);
            return cursorPacket;
            break;
        case Keyboard:
            keyboardPacket = createKeyboardDataPacketFromVector(globalPacketBuffer);
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
        controlPacket = createDefaultControlPacketFromVector(globalPacketBuffer, index);
        switch (controlPacket->getControlType())
        {
        case HANDSHAKE:
            handshakePacket = createHandshakeControlPacketFromVector(globalPacketBuffer);
            return handshakePacket;
            break;
        case KEEPALIVE:
            return controlPacket;
            break;
        case ACK:
            return controlPacket;
            break;
        case NAK:
            nakPacket = createNAKControlPacketFromVector(globalPacketBuffer);
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
            dropRequestPacket = createMessageDropRequestControlPacketFromVector(globalPacketBuffer);
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
