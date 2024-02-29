#pragma once
#include "packetParser.h"
#include <queue>
#include <thread>
#include <string>
#include <functional>
#include <mutex>

#define RECV_BUFFER_SIZE 1024
#define MAX_PORT_NUM 65535
#define MIN_PORT_NUM 1000

class SrtSocket
{
private:
	SOCKET _srtSocket;
	struct
	{
		uint16_t _srcPort;
		uint32_t _srcIP;
		uint16_t _dstPort;
		uint32_t _dstIP;
		uint32_t _seqNum;
		unsigned int _otherComputerMaxWindowSize;
		unsigned int _otherComputerMTU;

	} _commInfo;

	std::queue<const std::unique_ptr<const DefaultPacket>> _recviedPacketsQueue;
	std::mutex _recviedPacketsQueueMtx;

	std::queue<std::vector<uint8_t>> _packetSendQueue;
	std::mutex _packetSendQueueMtx;

	bool _shutdownSwitch;
	bool _keepAliveSwitch;
	std::mutex _switchesMtx;

	std::thread _keepAliveMonitoringThread;
	std::thread _cursorListenerThread;
	std::thread _keyboardListenerThread;
	std::thread _screenCaptureThread;
	std::thread _sendPacketsThread;
	std::thread _recivedPacketsThread;
	
	void waitForValidPacket(std::vector<char>* buffer, std::function<bool(char*, int)> checkFunction);
	void keepAliveMonitoring();
	

	bool isValidIpv4Checksum(const IpPacket& ipPacket);
	bool isValidIpHeaders(const IpPacket& ipHeaders);
	bool isValidUdpHeaders(const UdpPacket& udpHeaders);
	bool isValidHeaders(const IpPacket& ipHeaders, const UdpPacket& udpHeaders);

	void sendSrt();
	const std::unique_ptr<const DefaultPacket> recvSrt();
	const UdpPacket recvUdp();

public:
	SrtSocket();
	~SrtSocket();
	void listenAndAccept();
	void connectToServer(sockaddr_in* addrs);
	void srtBind(sockaddr_in* sockaddr);
	
};

