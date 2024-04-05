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
    
    return std::move(std::make_unique<DefaultPacket>(packetType, ackSequenceNumber, packetSequenceNumber, timeStamp));
}

std::unique_ptr<const DefaultDataPacket> PacketParser::createDefaultDataPacketFromVector(const std::vector<char>& defaultDataPacketBuffer, int& index)
{
    DataPacketTypes dataPacketType;

    std::unique_ptr<const DefaultPacket> defaultPacket = createDefaultPacketFromVector(defaultDataPacketBuffer, index);
    dataPacketType = static_cast<DataPacketTypes>(networkToHost<uint8_t>(defaultDataPacketBuffer, index));
    index += sizeof(uint8_t);

    return std::move(std::make_unique<DefaultDataPacket>(defaultPacket->getAckSequenceNumber(), defaultPacket->getPacketSequenceNumber(), defaultPacket->getTimeStamp(), dataPacketType));
}

std::unique_ptr<const CursorDataPacket> PacketParser::createCursorDataPacketFromVector(const std::vector<char>& cursorDataPacketBuffer)
{
    CursorActions action;
    uint8_t scrollValue;
    LONG x;
    LONG y;
    int index = 0;

    std::unique_ptr<const DefaultDataPacket> defaultDataPacket = createDefaultDataPacketFromVector(cursorDataPacketBuffer, index);
    action = static_cast<CursorActions>(networkToHost<uint8_t>(cursorDataPacketBuffer, index));
    index += sizeof(uint8_t);
    scrollValue = networkToHost<uint8_t>(cursorDataPacketBuffer, index);
    index += sizeof(uint8_t);
    x = networkToHost<LONG>(cursorDataPacketBuffer, index);
    index += sizeof(LONG);
    y = networkToHost<LONG>(cursorDataPacketBuffer, index);
    index += sizeof(LONG);

    return std::move(std::make_unique<CursorDataPacket>(defaultDataPacket->getAckSequenceNumber(), defaultDataPacket->getPacketSequenceNumber(), defaultDataPacket->getTimeStamp(), action, scrollValue, x, y));
}

std::unique_ptr<const KeyboardDataPacket> PacketParser::createKeyboardDataPacketFromVector(const std::vector<char>& keyboardDataPacketBuffer)
{
    KeyboardActions action;
    unsigned int keyCode;
    int index = 0;
    std::unique_ptr<const DefaultDataPacket> defaultDataPacket = createDefaultDataPacketFromVector(keyboardDataPacketBuffer, index);
    action = static_cast<KeyboardActions>(networkToHost<uint8_t>(keyboardDataPacketBuffer, index));
    index += sizeof(uint8_t);
    keyCode = networkToHost<uint8_t>(keyboardDataPacketBuffer, index);
    index += sizeof(uint8_t);

    return std::move(std::make_unique<KeyboardDataPacket>(defaultDataPacket->getAckSequenceNumber(), defaultDataPacket->getPacketSequenceNumber(), defaultDataPacket->getTimeStamp(), action, keyCode));
}

std::unique_ptr<const ScreenDataPacket> PacketParser::createScreenDataPacketFromVector(const std::vector<char>& screenDataPacketBuffer)
{
    int index = 0;
    std::unique_ptr<const DefaultDataPacket> defaultDataPacket = createDefaultDataPacketFromVector(screenDataPacketBuffer, index);
    uint32_t startSequenceNum = networkToHost<uint32_t>(screenDataPacketBuffer, index);
    index += sizeof(uint32_t);
    uint32_t endSequenceNum = networkToHost<uint32_t>(screenDataPacketBuffer, index);
    index += sizeof(uint32_t);
    uint32_t width = networkToHost<uint32_t>(screenDataPacketBuffer, index);
    index += sizeof(uint32_t);
    uint32_t height = networkToHost<uint32_t>(screenDataPacketBuffer, index);
    index += sizeof(uint32_t);
    std::vector<char> imageBytes(screenDataPacketBuffer.begin() + index, screenDataPacketBuffer.end());
    return std::move(std::make_unique<ScreenDataPacket>(defaultDataPacket->getAckSequenceNumber(), defaultDataPacket->getPacketSequenceNumber(), defaultDataPacket->getTimeStamp(), width, height, imageBytes, startSequenceNum, endSequenceNum));
}



std::unique_ptr<const DefaultControlPacket> PacketParser::createDefaultControlPacketFromVector(const std::vector<char>& defaultControlPacketBuffer, int& index)
{
    ControlPacketTypes controlPacketType;
    std::unique_ptr<const DefaultPacket> defaultPacket = createDefaultPacketFromVector(defaultControlPacketBuffer, index);
    controlPacketType = static_cast<ControlPacketTypes>(networkToHost<uint32_t>(defaultControlPacketBuffer, index));
    index += sizeof(uint32_t);

   return std::move(std::make_unique<DefaultControlPacket>(defaultPacket->getAckSequenceNumber(), defaultPacket->getPacketSequenceNumber(), defaultPacket->getTimeStamp(), controlPacketType));
}

std::unique_ptr<const HandshakeControlPacket> PacketParser::createHandshakeControlPacketFromVector(const std::vector<char>& handshakeControlPacketBuffer)
{
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

    return std::move(std::make_unique<HandshakeControlPacket>(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), encryption_key, windowSize, initialPacketSequenceNumber, maxTransmission, phase));
}

std::unique_ptr<const NAKControlPacket> PacketParser::createNAKControlPacketFromVector(const std::vector<char>& nakControlPacketBuffer)
{

    int index = 0;
    std::vector<unsigned int> lostSeqNums;

    std::unique_ptr<const DefaultControlPacket> defaultControlPacket = createDefaultControlPacketFromVector(nakControlPacketBuffer, index);

    while (index < nakControlPacketBuffer.size())
    {
        lostSeqNums.push_back(networkToHost<unsigned int>(nakControlPacketBuffer, index));
        index += sizeof(unsigned int);
    }

    return std::move(std::make_unique<NAKControlPacket>(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), lostSeqNums));
}

std::unique_ptr<const MessageDropRequestControlPacket> PacketParser::createMessageDropRequestControlPacketFromVector(const std::vector<char>& messageDropRequestControlPacketBuffer)
{
   
    int index = 0;
    std::vector<unsigned int> lostSeqNums;

    std::unique_ptr<const DefaultControlPacket> defaultControlPacket = createDefaultControlPacketFromVector(messageDropRequestControlPacketBuffer, index);
    
    while (index < messageDropRequestControlPacketBuffer.size())
    {
        lostSeqNums.push_back(networkToHost<unsigned int>(messageDropRequestControlPacketBuffer, index));
        index += sizeof(unsigned int);
    }

    return std::move(std::make_unique<MessageDropRequestControlPacket>(defaultControlPacket->getAckSequenceNumber(), defaultControlPacket->getPacketSequenceNumber(), defaultControlPacket->getTimeStamp(), lostSeqNums));
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

    return UdpPacket(srcPort, dstPort, length, checksum);
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

std::vector<char> PacketParser::packetToBytes(const UdpPacket& udpHeaders, const std::vector<char>& data)
{
    std::vector<char> buffer = udpHeaders.toBuffer();
    for (int i = 0; i < data.size(); i++)
    {
        buffer.push_back(data.at(i));
    }
    return buffer;
}

std::unique_ptr<const DefaultPacket> PacketParser::createPacketFromVectorGlobal(const std::vector<char>& globalPacketBuffer)
{
    int index = 0;
    std::unique_ptr<const DefaultPacket> packet = createDefaultPacketFromVector(globalPacketBuffer, index);
    std::unique_ptr<const DefaultDataPacket> dataPacket;
    std::unique_ptr<const DefaultControlPacket> controlPacket;

    switch (packet->getPacketType())
    {
    case DataPacket:
        index = 0;
        dataPacket = createDefaultDataPacketFromVector(globalPacketBuffer, index);
        switch (dataPacket->getDataType())
        {
        case Cursor:
            return std::move(createCursorDataPacketFromVector(globalPacketBuffer));
            break;
        case Keyboard:
            return std::move(createKeyboardDataPacketFromVector(globalPacketBuffer));
            break;
        case Screen:
            return std::move(createScreenDataPacketFromVector(globalPacketBuffer));
            break;
        case Chat:
            //To Do
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
            return std::move(createHandshakeControlPacketFromVector(globalPacketBuffer));
            break;
        case KEEPALIVE:
            return std::move(controlPacket);
            break;
        case ACK:
            return std::move(controlPacket);
            break;
        case NAK:
            return std::move(createNAKControlPacketFromVector(globalPacketBuffer));
            break;
        case CongestionWarning:
            return std::move(controlPacket);
            break;
        case SHUTDOWN:
            return std::move(controlPacket);
            break;
        case ACKACK:
            return std::move(controlPacket);
            break;
        case DROPREQ:
            return std::move(createMessageDropRequestControlPacketFromVector(globalPacketBuffer));
            break;
        case PEERERROR:
            return std::move(controlPacket);
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



std::unique_ptr<const DefaultPacket> PacketParser::createPacketFromVectorGlobal(const std::vector<char>& globalPacketBuffer, std::unique_ptr<const DefaultPacket> packet)
{
    int index;
    std::unique_ptr<const DefaultDataPacket> dataPacket;
    std::unique_ptr<const DefaultControlPacket> controlPacket;

    switch (packet->getPacketType())
    {
    case DataPacket:
        index = 0;
        dataPacket = createDefaultDataPacketFromVector(globalPacketBuffer, index);
        switch (dataPacket->getDataType())
        {
        case Cursor:
            return std::move(createCursorDataPacketFromVector(globalPacketBuffer));
            break;
        case Keyboard:
            return std::move(createKeyboardDataPacketFromVector(globalPacketBuffer));
            break;
        case Screen:
            return std::move(createScreenDataPacketFromVector(globalPacketBuffer));
            break;
        case Chat:
            //To Do
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
            return std::move(createHandshakeControlPacketFromVector(globalPacketBuffer));
            break;
        case KEEPALIVE:
            return std::move(controlPacket);
            break;
        case ACK:
            return std::move(controlPacket);
            break;
        case NAK:
            return std::move(createNAKControlPacketFromVector(globalPacketBuffer));
            break;
        case CongestionWarning:
            return std::move(controlPacket);
            break;
        case SHUTDOWN:
            return std::move(controlPacket);
            break;
        case ACKACK:
            return std::move(controlPacket);
            break;
        case DROPREQ:
            return std::move(createMessageDropRequestControlPacketFromVector(globalPacketBuffer));
            break;
        case PEERERROR:
            return std::move(controlPacket);
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