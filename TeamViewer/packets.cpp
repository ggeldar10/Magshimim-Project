#include "packets.h"

// DefaultPacket Definitions
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

// DefaultDataPacket Definitions
DefaultDataPacket::DefaultDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, DataPacketTypes dataPacketType)
    : DefaultPacket(DataPacket, ackNum, packetNum, time), dataPacketType(dataPacketType) {}

// CursorDataPacket Definitions
CursorDataPacket::CursorDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, CursorActions action, int scrollValue, unsigned int x, unsigned int y)
    : DefaultDataPacket(ackNum, packetNum, time, Cursor), action(action), scrollValue(scrollValue) {
    location.x = x;
    location.y = y;
}

CursorActions CursorDataPacket::getAction(CursorActions action) {
    return action;
}

POINT CursorDataPacket::getLocation() const {
    return location;
}

int CursorDataPacket::getScrollValue() const {
    return scrollValue;
}

// KeyboardDataPacket Definitions
KeyboardDataPacket::KeyboardDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, KeyboardActions action, int keyCode)
    : DefaultDataPacket(ackNum, packetNum, time, Keyboard), action(action), keyCode(keyCode) {
}

KeyboardActions KeyboardDataPacket::getAction() const {
    return action;
}

unsigned int KeyboardDataPacket::getKeyCode() const {
    return keyCode;
}

// DefaultControlPacket Definitions
DefaultControlPacket::DefaultControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, ControlPacketTypes controlPacketType)
    : DefaultPacket(ControlPacket, ackNum, packetNum, time), controlPacketType(controlPacketType) {}

// HandshakeControlPacket Definitions
HandshakeControlPacket::HandshakeControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, bool hasEncryption, uint16_t encryption_key, uint32_t windowSize,
    uint32_t initialPacketSequenceNumber, uint32_t maxTransmission, HandshakePhases phase)
    : DefaultControlPacket(ackNum, packetNum, time, HANDSHAKE), isEncrypted(hasEncryption), encryption_key(encryption_key),
    windowSize(windowSize), initialPacketSequenceNumber(initialPacketSequenceNumber), maxTransmission(maxTransmission), phase(phase) {}

bool HandshakeControlPacket::hasEncryption() const {
    return isEncrypted;
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

uint32_t HandshakeControlPacket::getMaxTransmission() const {
    return maxTransmission;
}

HandshakePhases HandshakeControlPacket::getPhase() const {
    return phase;
}

// NAKControlPacket Definitions
NAKControlPacket::NAKControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums)
    : DefaultControlPacket(ackNum, packetNum, time, NAK), lostSequenceNumbers(lostSeqNums) {
}

const std::vector<unsigned int>& NAKControlPacket::getLostSequenceNumbers() const {
    return lostSequenceNumbers;
}

// MessageDropRequestControlPacket Definitions
MessageDropRequestControlPacket::MessageDropRequestControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums)
    : DefaultControlPacket(ackNum, packetNum, time, DROPREQ), lostSequenceNumbers(lostSeqNums) {
}

const std::vector<unsigned int>& MessageDropRequestControlPacket::getLostSequenceNumbers() const {
    return lostSequenceNumbers;
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
    return _length;
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
