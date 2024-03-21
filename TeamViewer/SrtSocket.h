#pragma once
#include "packetParser.h"
#include "packetManager.h"
#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <thread>
#include <string>
#include <functional>
#include <mutex>

#define RECV_BUFFER_SIZE 1024
#define MAX_PORT_NUM 65535
#define MIN_PORT_NUM 1000

enum Modes
{
	Controller, Controled
};

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

	PacketManager _packetManager;

	/*std::queue<std::unique_ptr<DefaultPacket>> _recviedPacketsQueue;
	std::mutex _recviedPacketsQueueMtx;*/

	std::queue<std::vector<char>> _packetSendQueue;
	std::mutex _packetSendQueueMtx;

	bool _shutdownSwitch;
	bool _keepAliveSwitch;
	std::mutex _switchesMtx;

	std::thread _keepAliveMonitoringThread;
	std::thread _keepAliveTimerThread;

	std::thread _cursorListenerThread;
	std::thread _keyboardListenerThread;
	std::thread _screenListenerThread;

	std::thread _sendPacketsThread;
	std::thread _recivedPacketsThread;
	
	void keepAliveMonitoring();
	void keepAliveTimer();
	void initializeThreads(Modes mode);

	void waitForValidPacket(std::vector<char>* buffer, std::function<bool(char*, int)> checkFunction);
	bool isValidIpv4Checksum(const IpPacket& ipPacket);
	bool isValidIpHeaders(const IpPacket& ipHeaders);
	bool isValidUdpHeaders(const UdpPacket& udpHeaders);
	bool isValidHeaders(const IpPacket& ipHeaders, const UdpPacket& udpHeaders);

	void sendMonitoring();
	void recvMonitoring();
	void sendSrt();
	std::unique_ptr<const DefaultPacket> recvSrt();
	const UdpPacket recvUdp();

public:
	SrtSocket();
	~SrtSocket();
	void listenAndAccept();
	void connectToServer(sockaddr_in* addrs);
	void srtBind(sockaddr_in* sockaddr);
	
};

