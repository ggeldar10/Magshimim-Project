#include "packets.h"
#include "packetParser.h"

DefaultPacket::DefaultPacket(DefaultPacketTypes type, uint32_t ackNum, uint32_t packetNum, time_t time)
    : packetType(type), ackSequenceNumber(ackNum), packetSequenceNumber(packetNum), timeStamp(time) {}

DefaultPacketTypes DefaultPacket::getPacketType() const {
    return packetType;
}

uint32_t DefaultPacket::getAckSequenceNumber() const {
    return ackSequenceNumber;
}

uint32_t DefaultPacket::getPacketSequenceNumber() const {
    return packetSequenceNumber;
}

time_t DefaultPacket::getTimeStamp() const {
    return timeStamp;
}

std::vector<char> DefaultPacket::toBuffer() const {
    std::vector<char> buffer;
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, packetType);
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, ackSequenceNumber);
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, packetSequenceNumber);
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, timeStamp);
    return buffer;
}

DefaultDataPacket::DefaultDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, DataPacketTypes dataPacketType)
    : DefaultPacket(DataPacket, ackNum, packetNum, time), dataPacketType(dataPacketType) {}

DataPacketTypes DefaultDataPacket::getDataType() const {
    return dataPacketType;
}

std::vector<char> DefaultDataPacket::toBuffer() const {
    std::vector<char> buffer = DefaultPacket::toBuffer();
    buffer.push_back(static_cast<uint8_t>(dataPacketType));
    return buffer;
}

CursorDataPacket::CursorDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, CursorActions action, uint8_t scrollValue, LONG x, LONG y)
    : DefaultDataPacket(ackNum, packetNum, time, Cursor), action(action), scrollValue(scrollValue) {
    location.x = x;
    location.y = y;
}

CursorActions CursorDataPacket::getAction() const {
    return action;
}

POINT CursorDataPacket::getLocation() const {
    return location;
}

int CursorDataPacket::getScrollValue() const {
    return scrollValue;
}

std::vector<char> CursorDataPacket::toBuffer() const {
    std::vector<char> buffer = DefaultDataPacket::toBuffer();
    buffer.push_back(static_cast<uint8_t>(action));
    PacketParser::hostToNetworkIntoVector<uint8_t>(&buffer, scrollValue);
    PacketParser::hostToNetworkIntoVector<LONG>(&buffer, location.x);
    PacketParser::hostToNetworkIntoVector<LONG>(&buffer, location.y);
    //buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&scrollValue), reinterpret_cast<const uint8_t*>(&scrollValue) + sizeof(scrollValue));
    //buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&location), reinterpret_cast<const uint8_t*>(&location) + sizeof(location));
    return buffer;
}

KeyboardDataPacket::KeyboardDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, KeyboardActions action, unsigned int keyCode)
    : DefaultDataPacket(ackNum, packetNum, time, Keyboard), action(action), keyCode(keyCode) {
}

KeyboardActions KeyboardDataPacket::getAction() const {
    return action;
}

unsigned int KeyboardDataPacket::getKeyCode() const {
    return keyCode;
}

std::vector<char> KeyboardDataPacket::toBuffer() const {
    std::vector<char> buffer = DefaultDataPacket::toBuffer();
    buffer.push_back(static_cast<uint8_t>(action));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&keyCode), reinterpret_cast<const uint8_t*>(&keyCode) + sizeof(keyCode));
    return buffer;
}

DefaultControlPacket::DefaultControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, ControlPacketTypes controlPacketType)
    : DefaultPacket(ControlPacket, ackNum, packetNum, time), controlPacketType(controlPacketType) {}

ControlPacketTypes DefaultControlPacket::getControlType() const {
    return controlPacketType;
}

std::vector<char> DefaultControlPacket::toBuffer() const {
    std::vector<char> buffer = DefaultPacket::toBuffer();
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, controlPacketType);
    return buffer;
}

HandshakeControlPacket::HandshakeControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, uint16_t encryption_key, uint32_t windowSize,
    uint32_t initialPacketSequenceNumber, uint32_t maxTransmissionUnit, HandshakePhases phase)
    : DefaultControlPacket(ackNum, packetNum, time, HANDSHAKE), encryption_key(encryption_key),
    windowSize(windowSize), initialPacketSequenceNumber(initialPacketSequenceNumber), maxTransmissionUnit(maxTransmissionUnit), phase(phase) {
}

bool HandshakeControlPacket::hasEncryption() const {
    return encryption_key == 0;
}

uint16_t HandshakeControlPacket::getEncryptionKey() const {
    return encryption_key;
}

uint32_t HandshakeControlPacket::getWindowSize() const {
    return windowSize;
}

uint32_t HandshakeControlPacket::getInitialPacketSequenceNumber() const {
    return initialPacketSequenceNumber;
}

uint32_t HandshakeControlPacket::getMaxTransmissionUnit() const {
    return maxTransmissionUnit;
}

HandshakePhases HandshakeControlPacket::getPhase() const {
    return phase;
}

std::vector<char> HandshakeControlPacket::toBuffer() const {
    std::vector<char> buffer = DefaultControlPacket::toBuffer();
    PacketParser::hostToNetworkIntoVector<uint16_t>(&buffer, encryption_key);
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, windowSize);
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, initialPacketSequenceNumber);
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, maxTransmissionUnit);
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, phase);
    return buffer;
}

NAKControlPacket::NAKControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums)
    : DefaultControlPacket(ackNum, packetNum, time, NAK), lostSequenceNumbers(lostSeqNums) {
}

const std::vector<unsigned int>& NAKControlPacket::getLostSequenceNumbers() const {
    return lostSequenceNumbers;
}

std::vector<char> NAKControlPacket::toBuffer() const {
    std::vector<char> buffer = DefaultControlPacket::toBuffer();
    buffer.insert(buffer.end(), lostSequenceNumbers.begin(), lostSequenceNumbers.end());
    return buffer;
}

MessageDropRequestControlPacket::MessageDropRequestControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums)
    : DefaultControlPacket(ackNum, packetNum, time, DROPREQ), lostSequenceNumbers(lostSeqNums) {
}

const std::vector<unsigned int>& MessageDropRequestControlPacket::getLostSequenceNumbers() const {
    return lostSequenceNumbers;
}

std::vector<char> MessageDropRequestControlPacket::toBuffer() const {
    std::vector<char> buffer = DefaultControlPacket::toBuffer();
    buffer.insert(buffer.end(), lostSequenceNumbers.begin(), lostSequenceNumbers.end());
    return buffer;
}

IpPacket::IpPacket(uint8_t version, uint8_t lengthOfHeaders, uint8_t typeOfService,
    uint16_t totalLength, uint16_t identification, uint16_t fragmentOffsetIncludingFlags,
    uint8_t ttl, uint8_t protocol, uint16_t headerChecksum, uint32_t srcAddrs, uint32_t dstAddrs,
    const unsigned char* options) :
    version(version),
    lengthOfHeaders(lengthOfHeaders),
    typeOfService(typeOfService),
    totalLength(totalLength),
    identification(identification),
    fragmentOffsetIncludingFlags(fragmentOffsetIncludingFlags),
    ttl(ttl),
    protocol(protocol),
    headerChecksum(headerChecksum),
    srcAddrs(srcAddrs),
    dstAddrs(dstAddrs) {

    std::memcpy(this->options, options, MAX_IP_OPTIONS_SIZE);
}

uint8_t IpPacket::getVersion() const {
    return version;
}

uint8_t IpPacket::getLengthOfHeaders() const {
    return lengthOfHeaders;
}

uint8_t IpPacket::getLengthOfHeadersInBytes() const
{
    return lengthOfHeaders * FOUR_BITS;
}

uint8_t IpPacket::getTypeOfService() const {
    return typeOfService;
}

uint16_t IpPacket::getTotalLength() const {
    return totalLength;
}

uint16_t IpPacket::getIdentification() const {
    return identification;
}

uint16_t IpPacket::getFragmentOffsetIncludingFlags() const {
    return fragmentOffsetIncludingFlags;
}

uint8_t IpPacket::getTtl() const {
    return ttl;
}

uint8_t IpPacket::getProtocol() const {
    return protocol;
}

uint16_t IpPacket::getHeaderChecksum() const {
    return headerChecksum;
}

uint32_t IpPacket::getSrcAddrs() const {
    return srcAddrs;
}

uint32_t IpPacket::getDstAddrs() const {
    return dstAddrs;
}

const unsigned char* IpPacket::getOptions() const {
    return options;
}

std::vector<char> IpPacket::toBuffer() const {
    std::vector<char> buffer;
    buffer.push_back(static_cast<uint8_t>(version));
    buffer.push_back(static_cast<uint8_t>(lengthOfHeaders));
    buffer.push_back(static_cast<uint8_t>(typeOfService));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&totalLength), reinterpret_cast<const uint8_t*>(&totalLength) + sizeof(totalLength));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&identification), reinterpret_cast<const uint8_t*>(&identification) + sizeof(identification));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&fragmentOffsetIncludingFlags), reinterpret_cast<const uint8_t*>(&fragmentOffsetIncludingFlags) + sizeof(fragmentOffsetIncludingFlags));
    buffer.push_back(static_cast<uint8_t>(ttl));
    buffer.push_back(static_cast<uint8_t>(protocol));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&headerChecksum), reinterpret_cast<const uint8_t*>(&headerChecksum) + sizeof(headerChecksum));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&srcAddrs), reinterpret_cast<const uint8_t*>(&srcAddrs) + sizeof(srcAddrs));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&dstAddrs), reinterpret_cast<const uint8_t*>(&dstAddrs) + sizeof(dstAddrs));
    buffer.insert(buffer.end(), options, options + MAX_IP_OPTIONS_SIZE);
    return buffer;
}

UdpPacket::UdpPacket(uint16_t srcPort, uint16_t dstPort, uint16_t length, uint16_t checksum) : _srcPort(srcPort), _dstPort(dstPort), _length(length), _checksum(checksum)
{
}

int UdpPacket::getSrcPort() const
{
    return this->_srcPort;
}

void UdpPacket::setSrcPort(uint16_t srcPort)
{
    this->_srcPort = srcPort;
}

int UdpPacket::getDstPort() const
{
    return this->_dstPort;
}

void UdpPacket::setDstPort(uint16_t dstPort)
{
    this->_dstPort = dstPort;
}

int UdpPacket::getLength() const
{
    return this->_length;
}

void UdpPacket::setLength(uint16_t length)
{
    this->_length = length;
}

int UdpPacket::getChecksum() const
{
    return this->_checksum;
}

void UdpPacket::setChecksum(uint16_t checksum)
{
    this->_checksum = checksum;
}

std::vector<char> UdpPacket::toBuffer() const
{
    std::vector<char> buffer;
    buffer.push_back((this->_srcPort & 0xFF00) >> BYTE_IN_BITS);
    buffer.push_back(this->_srcPort & 0xFF);
    buffer.push_back((this->_dstPort & 0xFF00) >> BYTE_IN_BITS);
    buffer.push_back(this->_dstPort & 0xFF);
    PacketParser::hostToNetworkIntoVector<uint16_t>(&buffer, this->_length);
    PacketParser::hostToNetworkIntoVector<uint16_t>(&buffer, this->_checksum);
    return buffer;
}

ScreenDataPacket::ScreenDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, uint32_t width, uint32_t height, const std::vector<char>& imageBytes) 
    : DefaultDataPacket(ackNum, packetNum, time, Screen), _width(width), _height(height), _imageBytes(imageBytes)
{
}

std::vector<char> ScreenDataPacket::getImageBytes() const
{
    return this->_imageBytes;
}

unsigned int ScreenDataPacket::getWidth() const
{
    return _width;
}

unsigned int ScreenDataPacket::getHeight() const
{
    return _height;
}

std::vector<char> ScreenDataPacket::toBuffer() const
{
    std::vector<char> buffer = DefaultDataPacket::toBuffer();
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, _width);
    PacketParser::hostToNetworkIntoVector<uint32_t>(&buffer, _height);
    buffer.insert(buffer.end(), this->_imageBytes.begin(), this->_imageBytes.end());
    return buffer;
}
