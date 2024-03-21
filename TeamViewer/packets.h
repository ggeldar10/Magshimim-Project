#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "windowsHeaders.h"

#define MAX_IP_OPTIONS_SIZE 10

enum DataPacketTypes {
    Cursor = 0, Keyboard, Screen, Chat
};

enum CursorActions {
    CursorPosition = 0, LeftButtonDown, LeftButtonUp, RightButtonDown, RightButtonUp, MiddleButtonDown, MiddleButtonUp, WheelScroll
};

enum KeyboardActions {
    KeyUp = 0, KeyDown
};

enum ControlPacketTypes {
    HANDSHAKE = 0, KEEPALIVE, ACK, NAK, CongestionWarning, SHUTDOWN, ACKACK, DROPREQ, PEERERROR
};

enum HandshakePhases : uint8_t {
    INDUCTION_1 = 0, INDUCTION_2, SUMMARY_1, SUMMARY_2
};

enum DefaultPacketTypes {
    DataPacket = 0, ControlPacket
};

class UdpPacket
{
private:
    uint16_t _srcPort;
    uint16_t _dstPort;
    uint16_t _length;
    uint16_t _checksum;

public:
    
    UdpPacket(uint16_t srcPort, uint16_t dstPort, uint16_t length, uint16_t checksum);    
    int getSrcPort() const;
    void setSrcPort(uint16_t srcPort);
    int getDstPort() const;
    void setDstPort(uint16_t dstPort);
    int getLength() const;
    void setLength(uint16_t length);
    int getChecksum() const;
    void setChecksum(uint16_t checksum);

    std::vector<char> toBuffer() const;
};

class DefaultPacket {
protected:
    DefaultPacketTypes packetType;
    uint32_t ackSequenceNumber;
    uint32_t packetSequenceNumber;
    time_t timeStamp;

public:
    DefaultPacket(DefaultPacketTypes type, uint32_t ackNum, uint32_t packetNum, time_t time);
    DefaultPacketTypes getPacketType() const;
    uint32_t getAckSequenceNumber() const;
    uint32_t getPacketSequenceNumber() const;
    time_t getTimeStamp() const;

    virtual std::vector<char> toBuffer() const;
};

class DefaultDataPacket : public DefaultPacket {
protected:
    DataPacketTypes dataPacketType;

public:
    DefaultDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, DataPacketTypes dataPacketType);
    DataPacketTypes getDataType() const;

    std::vector<char> toBuffer() const override;
};

class CursorDataPacket : public DefaultDataPacket {
private:
    CursorActions action;
    int scrollValue;
    POINT location;

public:
    CursorDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, CursorActions action, int scrollValue, unsigned int x, unsigned int y);
    CursorActions getAction() const;
    POINT getLocation() const;
    int getScrollValue() const;

    std::vector<char> toBuffer() const override;
};

class KeyboardDataPacket : public DefaultDataPacket {
private:
    KeyboardActions action;
    unsigned int keyCode;

public:
    KeyboardDataPacket(uint32_t ackNum, uint32_t packetNum, time_t time, KeyboardActions action, unsigned int keyCode);
    KeyboardActions getAction() const;
    unsigned int getKeyCode() const;

    std::vector<char> toBuffer() const override;
};

class DefaultControlPacket : public DefaultPacket {
protected:
    ControlPacketTypes controlPacketType;

public:
    DefaultControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, ControlPacketTypes controlPacketType);
    ControlPacketTypes getControlType() const;

    std::vector<char> toBuffer() const override;
};

class HandshakeControlPacket : public DefaultControlPacket {
private:
    uint16_t encryption_key;
    uint32_t windowSize;
    uint32_t initialPacketSequenceNumber;
    uint32_t maxTransmissionUnit;
    HandshakePhases phase;

public:
    HandshakeControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, uint16_t encryption_key, uint32_t windowSize, uint32_t initialPacketSequenceNumber, uint32_t maxTransmissionUnit, HandshakePhases phase);
    bool hasEncryption() const;
    uint16_t getEncryptionKey() const;
    uint32_t getWindowSize() const;
    uint32_t getInitialPacketSequenceNumber() const;
    uint32_t getMaxTransmissionUnit() const;
    HandshakePhases getPhase() const;

    std::vector<char> toBuffer() const override;
};

class NAKControlPacket : public DefaultControlPacket {
private:
    std::vector<unsigned int> lostSequenceNumbers;

public:
    NAKControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums);
    const std::vector<unsigned int>& getLostSequenceNumbers() const;

    std::vector<char> toBuffer() const override;
};

class MessageDropRequestControlPacket : public DefaultControlPacket {
private:
    std::vector<unsigned int> lostSequenceNumbers;

public:
    MessageDropRequestControlPacket(uint32_t ackNum, uint32_t packetNum, time_t time, const std::vector<unsigned int>& lostSeqNums);
    const std::vector<unsigned int>& getLostSequenceNumbers() const;

    std::vector<char> toBuffer() const override;
};

class IpPacket {
private:
    uint8_t version;
    uint8_t lengthOfHeaders;
    uint8_t typeOfService;
    uint16_t totalLength;
    uint16_t identification;
    uint16_t fragmentOffsetIncludingFlags;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t headerChecksum;
    uint32_t srcAddrs;
    uint32_t dstAddrs;
    unsigned char options[MAX_IP_OPTIONS_SIZE];

public:
    IpPacket(uint8_t version, uint8_t lengthOfHeaders, uint8_t typeOfService,
        uint16_t totalLength, uint16_t identification, uint16_t fragmentOffsetIncludingFlags,
        uint8_t ttl, uint8_t protocol, uint16_t headerChecksum, uint32_t srcAddrs, uint32_t dstAddrs,
        const unsigned char* options);

    uint8_t getVersion() const;
    uint8_t getLengthOfHeaders() const;
    uint8_t getLengthOfHeadersInBytes() const;
    uint8_t getTypeOfService() const;
    uint16_t getTotalLength() const;
    uint16_t getIdentification() const;
    uint16_t getFragmentOffsetIncludingFlags() const;
    uint8_t getTtl() const;
    uint8_t getProtocol() const;
    uint16_t getHeaderChecksum() const;
    uint32_t getSrcAddrs() const;
    uint32_t getDstAddrs() const;
    const unsigned char* getOptions() const;

    std::vector<char> toBuffer() const;
};
