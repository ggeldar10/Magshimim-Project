#include "packets.h"

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

std::vector<uint8_t> DefaultPacket::toBuffer() const {
    std::vector<uint8_t> buffer;
    buffer.push_back(static_cast<uint8_t>(packetType));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&ackSequenceNumber), reinterpret_cast<const uint8_t*>(&ackSequenceNumber) + sizeof(ackSequenceNumber));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&packetSequenceNumber), reinterpret_cast<const uint8_t*>(&packetSequenceNumber) + sizeof(packetSequenceNumber));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&timeStamp), reinterpret_cast<const uint8_t*>(&timeStamp) + sizeof(timeStamp));
    return buffer;
}

DefaultDataPacket::DefaultDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, DataPacketTypes dataPacketType)
    : DefaultPacket(DataPacket, ackNum, packetNum, time), dataPacketType(dataPacketType) {}

DataPacketTypes DefaultDataPacket::getDataType() const {
    return dataPacketType;
}

std::vector<uint8_t> DefaultDataPacket::toBuffer() const {
    std::vector<uint8_t> buffer = DefaultPacket::toBuffer();
    buffer.push_back(static_cast<uint8_t>(dataPacketType));
    return buffer;
}

CursorDataPacket::CursorDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, CursorActions action, int scrollValue, unsigned int x, unsigned int y)
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

std::vector<uint8_t> CursorDataPacket::toBuffer() const {
    std::vector<uint8_t> buffer = DefaultDataPacket::toBuffer();
    buffer.push_back(static_cast<uint8_t>(action));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&scrollValue), reinterpret_cast<const uint8_t*>(&scrollValue) + sizeof(scrollValue));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&location), reinterpret_cast<const uint8_t*>(&location) + sizeof(location));
    return buffer;
}

KeyboardDataPacket::KeyboardDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, KeyboardActions action, int keyCode)
    : DefaultDataPacket(ackNum, packetNum, time, Keyboard), action(action), keyCode(keyCode) {
}

KeyboardActions KeyboardDataPacket::getAction() const {
    return action;
}

unsigned int KeyboardDataPacket::getKeyCode() const {
    return keyCode;
}

std::vector<uint8_t> KeyboardDataPacket::toBuffer() const {
    std::vector<uint8_t> buffer = DefaultDataPacket::toBuffer();
    buffer.push_back(static_cast<uint8_t>(action));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&keyCode), reinterpret_cast<const uint8_t*>(&keyCode) + sizeof(keyCode));
    return buffer;
}

DefaultControlPacket::DefaultControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, ControlPacketTypes controlPacketType)
    : DefaultPacket(ControlPacket, ackNum, packetNum, time), controlPacketType(controlPacketType) {}

ControlPacketTypes DefaultControlPacket::getControlType() const {
    return controlPacketType;
}

std::vector<uint8_t> DefaultControlPacket::toBuffer() const {
    std::vector<uint8_t> buffer = DefaultPacket::toBuffer();
    buffer.push_back(static_cast<uint8_t>(controlPacketType));
    return buffer;
}

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

std::vector<uint8_t> HandshakeControlPacket::toBuffer() const {
    std::vector<uint8_t> buffer = DefaultControlPacket::toBuffer();
    buffer.push_back(static_cast<uint8_t>(isEncrypted));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&encryption_key), reinterpret_cast<const uint8_t*>(&encryption_key) + sizeof(encryption_key));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&windowSize), reinterpret_cast<const uint8_t*>(&windowSize) + sizeof(windowSize));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&initialPacketSequenceNumber), reinterpret_cast<const uint8_t*>(&initialPacketSequenceNumber) + sizeof(initialPacketSequenceNumber));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&maxTransmission), reinterpret_cast<const uint8_t*>(&maxTransmission) + sizeof(maxTransmission));
    buffer.push_back(static_cast<uint8_t>(phase));
    return buffer;
}

NAKControlPacket::NAKControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums)
    : DefaultControlPacket(ackNum, packetNum, time, NAK), lostSequenceNumbers(lostSeqNums) {
}

const std::vector<unsigned int>& NAKControlPacket::getLostSequenceNumbers() const {
    return lostSequenceNumbers;
}

std::vector<uint8_t> NAKControlPacket::toBuffer() const {
    std::vector<uint8_t> buffer = DefaultControlPacket::toBuffer();
    buffer.insert(buffer.end(), lostSequenceNumbers.begin(), lostSequenceNumbers.end());
    return buffer;
}

MessageDropRequestControlPacket::MessageDropRequestControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums)
    : DefaultControlPacket(ackNum, packetNum, time, DROPREQ), lostSequenceNumbers(lostSeqNums) {
}

const std::vector<unsigned int>& MessageDropRequestControlPacket::getLostSequenceNumbers() const {
    return lostSequenceNumbers;
}

std::vector<uint8_t> MessageDropRequestControlPacket::toBuffer() const {
    std::vector<uint8_t> buffer = DefaultControlPacket::toBuffer();
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

std::vector<uint8_t> IpPacket::toBuffer() const {
    std::vector<uint8_t> buffer;
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
