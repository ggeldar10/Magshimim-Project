#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SrtSocket.h"
#include "ImageCapture.h"
#include <fstream>


SrtSocket::SrtSocket(PipeManager* pipeManager)
	: _packetManager(&_keepAliveSwitch, &_shutdownSwitch, &_switchesMtx, pipeManager)
{
<<<<<<< HEAD
	int recv_buffer_size = 100000000;  // Specify your desired buffer size
=======
	int recv_buffer_size = 8096;  // Specify your desired buffer size
>>>>>>> f41b11f007adc65696dda9394624bbd5806e93e2
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "Error while trying to intialize startup" << std::endl;
		throw "Error while trying to intialize startup";
	}
	this->_srtSocket = socket(AF_INET, SOCK_RAW, IP_SRT_PROTOCOL_NUMBER);
	if (this->_srtSocket == INVALID_SOCKET)
	{
		std::cerr << "Error while trying to open a socket" << std::endl;
		throw "Error while trying to open a socket";
	}
	this->_commInfo = { 0 };
	this->_shutdownSwitch = false;
	this->_keepAliveSwitch = true;

	
	if (setsockopt(this->_srtSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&recv_buffer_size), sizeof(recv_buffer_size)) == -1) {
		std::cerr << "Failed to set receive buffer size" << std::endl;
	}
}


SrtSocket::~SrtSocket()
{
	this->_keepAliveSwitch = false;
	this->_shutdownSwitch = true;
	this->_packetSendQueue.empty();
	//this->_recviedPacketsQueue.empty();
	closesocket(this->_srtSocket);
	WSACleanup();
}

/*
intiates the communicatations and wait untill a client has
connected only one client at a time
does the handshake and the intilize the missing commInfo
input:
output:
*/
void SrtSocket::listenAndAccept()
{
	std::unique_ptr<IpPacket> ipHeaders;
	std::unique_ptr<UdpPacket> udpHeaders;
	std::unique_ptr<const HandshakeControlPacket> srtHeaders;
	waitForValidPacket(nullptr, [&](char* buffer, int totalLength) -> bool
		{
			std::vector<char> bufferVec;
			bufferVec.assign(buffer, buffer + totalLength);
			ipHeaders = std::make_unique<IpPacket>(PacketParser::createIpPacketFromVector(bufferVec));
			udpHeaders = std::make_unique<UdpPacket>(PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeaders->getLengthOfHeadersInBytes(), bufferVec.begin() + ipHeaders->getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE }));
			if (!isValidHeaders(*ipHeaders, *udpHeaders))
			{
				return false;
			}
			if (udpHeaders->getLength() != HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE)
			{
				return false;
			}
			srtHeaders = PacketParser::createHandshakeControlPacketFromVector({ bufferVec.begin() + ipHeaders->getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE, bufferVec.end() }); // might have a problem
			if (srtHeaders->getPhase() != INDUCTION_1)
			{
				return false;
			}
			return true;
		});

	this->_commInfo._dstIP = ipHeaders->getSrcAddrs();
	this->_commInfo._dstPort = udpHeaders->getSrcPort();
	this->_commInfo._otherComputerMTU = srtHeaders->getMaxTransmissionUnit();
	this->_commInfo._otherComputerMaxWindowSize = srtHeaders->getWindowSize();

	sockaddr_in toAddr = { 0 };
	toAddr.sin_port = htons(this->_commInfo._dstPort);
	toAddr.sin_addr.s_addr = htonl(this->_commInfo._dstIP);
	toAddr.sin_family = AF_INET;

	HandshakeControlPacket handshakeSend = HandshakeControlPacket(2, 0, std::time(nullptr), 0, DEFUALT_MAX_TRANSMISSION, 0, DEFUALT_MTU_SIZE, INDUCTION_2);
	UdpPacket udpPacketSend = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, UDP_HEADERS_SIZE + HANDSHAKE_PACKET_SIZE, 0);
	std::vector<char> sendBufferVector = PacketParser::packetToBytes(udpPacketSend, handshakeSend, nullptr);
	int res = sendto(this->_srtSocket, sendBufferVector.data(), sendBufferVector.size(), 0, reinterpret_cast<sockaddr*>(&toAddr), sizeof(sockaddr_in));
	if (res < 0)
	{
		int error = WSAGetLastError();
		std::cerr << "Error while doing sendto in listenAndAccept: " << error << std::endl;
		throw /*todo throw the right error*/;
	}
	waitForValidPacket(nullptr, [&](char* buffer, int totalLength) -> bool
		{
			std::vector<char> bufferVec;
			bufferVec.assign(buffer, buffer + totalLength);
			ipHeaders = std::make_unique<IpPacket>(PacketParser::createIpPacketFromVector(bufferVec));
			udpHeaders = std::make_unique<UdpPacket>(PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeaders->getLengthOfHeadersInBytes(), bufferVec.begin() + ipHeaders->getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE }));
			if (!isValidHeaders(*ipHeaders, *udpHeaders))
			{
				return false;
			}
			if (udpHeaders->getLength() != HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE)
			{
				return false;
			}
			srtHeaders = PacketParser::createHandshakeControlPacketFromVector({ bufferVec.begin() + ipHeaders->getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE, bufferVec.end() }); // might have a problem
			if (srtHeaders->getPhase() != SUMMARY_1)
			{
				return false;
			}
			return true;
		});

	// other logic here

	handshakeSend = HandshakeControlPacket(4, 0, time(nullptr), 0, DEFUALT_MAX_TRANSMISSION, 0/*Change later*/, DEFUALT_MTU_SIZE, SUMMARY_2);
	udpPacketSend = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, UDP_HEADERS_SIZE + HANDSHAKE_PACKET_SIZE, 0);
	sendBufferVector = PacketParser::packetToBytes(udpPacketSend, handshakeSend, nullptr);
	if (sendto(this->_srtSocket, sendBufferVector.data(), sendBufferVector.size(), 0, reinterpret_cast<sockaddr*>(&toAddr), sizeof(sockaddr_in)) < 0)
	{
		int error = WSAGetLastError();
		std::cerr << "Error while doing sendto in listenAndAccept: " << error << std::endl;
		throw /*todo throw the right error*/;
	}
	std::cout << "Connected" << std::endl;
	initializeThreads(CONTROLLED);
}

void SrtSocket::connectToServer(sockaddr_in* addrs) //todo add the waitForValidPacket
{
	std::unique_ptr<IpPacket> ipHeadersRecv;
	std::unique_ptr<UdpPacket> udpHeadersRecv;
	std::unique_ptr<const HandshakeControlPacket> srtHeadersRecv;


	this->_commInfo._dstPort = htons(addrs->sin_port);
	this->_commInfo._dstIP = addrs->sin_addr.s_addr;
	sockaddr_in bindToAddr = { 0 };
	bindToAddr.sin_addr.s_addr = INADDR_ANY;
	bindToAddr.sin_port = 0;
	bindToAddr.sin_family = AF_INET;
	this->srtBind(&bindToAddr);
	HandshakeControlPacket handshakeHeaders = HandshakeControlPacket(0, 0, std::time(nullptr), 0, DEFUALT_MAX_TRANSMISSION, 0, DEFUALT_MTU_SIZE, INDUCTION_1);
	UdpPacket udpHeaders = UdpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE, 0);
	std::vector<char> sendBuffer = PacketParser::packetToBytes(udpHeaders, handshakeHeaders, nullptr);
	if (sendto(this->_srtSocket, sendBuffer.data(), sendBuffer.size(), 0, reinterpret_cast<sockaddr*>(addrs), sizeof(sockaddr_in)) < 0)
	{
		std::cerr << "Error while doing sendTo in the handshake" << std::endl;
		throw /*do an exception*/;
	}
	waitForValidPacket(nullptr, [&](char* buffer, int size) -> bool
		{
			std::vector<char> bufferVec;
			bufferVec.assign(buffer, buffer + size);
			ipHeadersRecv = std::make_unique<IpPacket>(PacketParser::createIpPacketFromVector(bufferVec));
			udpHeadersRecv = std::make_unique<UdpPacket>(PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeadersRecv->getLengthOfHeadersInBytes(), bufferVec.begin() + ipHeadersRecv->getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE }));
			if (!isValidHeaders(*ipHeadersRecv, *udpHeadersRecv))
			{
				return false;
			}
			if (udpHeadersRecv->getLength() != HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE)
			{
				return false;
			}
			srtHeadersRecv = PacketParser::createHandshakeControlPacketFromVector({ bufferVec.begin() + ipHeadersRecv->getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE, bufferVec.end() }); // might have a problem
			if (srtHeadersRecv->getPhase() != INDUCTION_2)
			{
				return false;
			}
			return true;
		});
	this->_commInfo._otherComputerMTU = srtHeadersRecv->getMaxTransmissionUnit();
	this->_commInfo._otherComputerMaxWindowSize = srtHeadersRecv->getWindowSize();


	handshakeHeaders = HandshakeControlPacket(4, 0, time(nullptr), 0, DEFUALT_MTU_SIZE, 5, DEFUALT_MAX_TRANSMISSION, SUMMARY_1);
	sendBuffer = PacketParser::packetToBytes(udpHeaders, handshakeHeaders, nullptr);
	if (sendto(this->_srtSocket, sendBuffer.data(), sendBuffer.size(), 0, reinterpret_cast<sockaddr*>(addrs), sizeof(sockaddr_in)) < 0)
	{
		std::cerr << "Error while doing sendTo in the handshake" << std::endl;
		throw /*do an exception*/;
	}
	waitForValidPacket(nullptr, [&](char* buffer, int size) -> bool
		{
			std::vector<char> bufferVec;
			bufferVec.assign(buffer, buffer + size);
			ipHeadersRecv = std::make_unique<IpPacket>(PacketParser::createIpPacketFromVector(bufferVec));
			udpHeadersRecv = std::make_unique<UdpPacket>(PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeadersRecv->getLengthOfHeadersInBytes(), bufferVec.begin() + ipHeadersRecv->getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE }));
			if (!isValidHeaders(*ipHeadersRecv, *udpHeadersRecv))
			{
				return false;
			}
			if (udpHeadersRecv->getLength() != HANDSHAKE_PACKET_SIZE + UDP_HEADERS_SIZE)
			{
				return false;
			}
			srtHeadersRecv = PacketParser::createHandshakeControlPacketFromVector({ bufferVec.begin() + ipHeadersRecv->getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE, bufferVec.end() }); // might have a problem
			if (srtHeadersRecv->getPhase() != SUMMARY_2)
			{
				return false;
			}
			return true;
		});
	initializeThreads(CONTROLLER);
}

/*
waits for the packet with the srt protocol and with the right port and ip
input: isValid - the function that checks if the packet is valid it gets 
a buffer and the size of it 
and returns true if the packet is valid false if else
buffer - the buffer to get the valid packet into 
output: returns the valid packet inside the given buffer
*/
void SrtSocket::waitForValidPacket(std::vector<char>* buffer, std::function<bool(char*, int)> isValid)
{
	const int ipFirstHeaderRowLowerInArray = 2;
	const int ipFirstHeaderRowHigherInArray = 3;
	char ipFirstRow[MAX_IP_SIZE + 1] = { 0 };
	bool notValid = true;
	int totalLength = 0;
	while (notValid)
	{
		recv(this->_srtSocket, ipFirstRow, IP_FIRST_HEADER_ROW, MSG_PEEK);
		if (WSAEMSGSIZE != WSAGetLastError())
		{
			std::cerr << "Error while doing recv. error number from wsagetlasterror: " << WSAGetLastError() << std::endl;
			throw;// excetpion
		}

		totalLength = (ipFirstRow[ipFirstHeaderRowLowerInArray] << FOUR_BITS) | ipFirstRow[ipFirstHeaderRowHigherInArray];
		std::unique_ptr<char[]> allBuffer = std::make_unique<char[]>(totalLength);

		if(recv(this->_srtSocket, allBuffer.get(), totalLength, 0) < 0)
		{
			std::cerr << "Error while doing recv. error number from wsagetlasterror: " << WSAGetLastError() << std::endl;
			//throw excetpions
		}
		if (isValid(allBuffer.get(), totalLength))
		{
			if (buffer != nullptr)
			{
				for (int i = 0; i < totalLength; i++)
				{
					buffer->push_back(allBuffer[i]);
				}
			}
			notValid = false;
		}
	}
}

bool SrtSocket::isValidIpv4Checksum(const IpPacket& ipPacket)
{
	return true; // todo make the check for the valid checksum
}

bool SrtSocket::isValidIpHeaders(const IpPacket& ipHeaders)
{
	bool isSrtProtocol = ipHeaders.getProtocol() == IP_SRT_PROTOCOL_NUMBER;
	if ((this->_commInfo._dstIP == 0 || this->_commInfo._srcIP == 0) && isSrtProtocol)
	{
		return true;
	}
	if (!isSrtProtocol && this->_commInfo._srcIP != ipHeaders.getDstAddrs() && this->_commInfo._dstIP == ipHeaders.getSrcAddrs())
	{
		return false;
	}
	return true;
}

bool SrtSocket::isValidUdpHeaders(const UdpPacket& udpHeaders)
{
	//if (udpHeaders.getChecksum() == /*calculate checksum*/)
	//{

	//}
	if (this->_commInfo._srcPort == 0 || this->_commInfo._dstPort == 0)
	{
		return true;
	}
	if (udpHeaders.getDstPort() != this->_commInfo._srcPort || udpHeaders.getSrcPort() != this->_commInfo._dstPort)
	{
		return false;
	}
	return true;
}

bool SrtSocket::isValidHeaders(const IpPacket& ipHeaders, const UdpPacket& udpHeaders)
{
	return isValidIpHeaders(ipHeaders) && isValidUdpHeaders(udpHeaders);
}

void SrtSocket::sendMonitoring()
{
	bool runLoop = true;

	std::unique_lock<std::mutex> switchLock(this->_switchesMtx);
	switchLock.unlock();

	while (runLoop)
	{
		sendSrt();
		Sleep(200);
		switchLock.lock();
		runLoop = !this->_shutdownSwitch;
		switchLock.unlock();
	}
	
}

void SrtSocket::recvMonitoring()
{
	bool runLoop = true;
	std::unique_lock<std::mutex> switchLock(this->_switchesMtx);
	switchLock.unlock();


	while (runLoop)
	{
		std::unique_ptr<const DefaultPacket> packet = recvSrt();
		this->_packetManager.handlePacket(std::move(packet));
		Sleep(100);
		switchLock.lock();
		runLoop = !this->_shutdownSwitch;
		switchLock.unlock();
	}
}


/*
binds the socket to a specific port and ip on the computer
input: a ptr to the addrs to bind to and the port to bind to
using the struct sockaddr_in
output:
*/
void SrtSocket::srtBind(sockaddr_in* addrs)
{
	if (bind(this->_srtSocket, reinterpret_cast<sockaddr*>(addrs), sizeof(sockaddr_in)) < 0)
	{
		std::cerr << "Error while trying to bind" << std::endl;
		std::cerr << WSAGetLastError() << std::endl;
		throw "Error while trying to bind";
	}
	sockaddr_in output;
	int addrlen = sizeof(sockaddr_in);
	if (getsockname(this->_srtSocket, reinterpret_cast<sockaddr*>(&output), &addrlen) != 0)
	{
		std::cerr << "Error while trying to bind" << std::endl;
		std::cerr << WSAGetLastError() << std::endl;
		throw "Error while trying to bind";
	}
	this->_commInfo._srcIP = output.sin_addr.s_addr;
	if (addrs->sin_port == 0)
	{
		std::srand(std::time(nullptr));
		this->_commInfo._srcPort = htons(MIN_PORT_NUM + std::rand() % (MAX_PORT_NUM - MIN_PORT_NUM));
	}
	else
	{
		this->_commInfo._srcPort = htons(addrs->sin_port);
	}
}

void SrtSocket::keepAliveMonitoring()
{
	bool runLoop = true;

	std::chrono::system_clock::time_point now;
	std::time_t currentTime;

	std::unique_lock<std::mutex> switchLock(this->_switchesMtx);
	switchLock.unlock();
	std::unique_lock<std::mutex> sendLock(this->_packetSendQueueMtx);
	sendLock.unlock();

	this->_keepAliveTimerThread = std::thread(&SrtSocket::keepAliveTimer, this);

	while (runLoop)
	{
		now = std::chrono::system_clock::now();
		currentTime = std::chrono::system_clock::to_time_t(now);
		std::unique_ptr<DefaultControlPacket> packetPtr = std::make_unique<DefaultControlPacket>(-1, -1, currentTime, KEEPALIVE);
		sendLock.lock();
		this->_packetSendQueue.push(packetPtr->toBuffer());
		sendLock.unlock();
		switchLock.lock();
		runLoop = this->_keepAliveSwitch && !this->_shutdownSwitch;
		switchLock.unlock();
		Sleep(2000);
	}

	now = std::chrono::system_clock::now();
	currentTime = std::chrono::system_clock::to_time_t(now);
	std::unique_ptr<DefaultControlPacket> packetPtr = std::make_unique<DefaultControlPacket>(-1, -1, currentTime, SHUTDOWN);
	sendLock.lock();
	this->_packetSendQueue.push(packetPtr->toBuffer());
	sendLock.unlock();
	switchLock.lock();
	this->_keepAliveSwitch = false;
	this->_keepAliveTimerThread.join();
	this->_shutdownSwitch = true;
	switchLock.unlock();
}

void SrtSocket::keepAliveTimer()
{
	bool runLoop = true;

	std::unique_lock<std::mutex> switchLock(this->_switchesMtx);
	switchLock.unlock();
	std::chrono::seconds keepAliveTimeout(5);

	while (runLoop)
	{
		switchLock.lock();
		this->_keepAliveSwitch = false;
		switchLock.unlock();
		std::this_thread::sleep_for(keepAliveTimeout);
		switchLock.lock();
		runLoop = this->_keepAliveSwitch;
		switchLock.unlock();
	}
}

void SrtSocket::sendSrt() {
	std::lock_guard<std::mutex> lock(this->_packetSendQueueMtx);

	if (this->_packetSendQueue.size() != 0) 
	{
		std::vector<char> dataBuffer = this->_packetSendQueue.front();
		this->_packetSendQueue.pop();
		sockaddr_in destAddr = { 0 };
		destAddr.sin_addr.s_addr = this->_commInfo._dstIP;
		destAddr.sin_family = AF_INET;
		UdpPacket udpPacket(this->_commInfo._srcPort, this->_commInfo._dstPort, dataBuffer.size(), 0);
		std::vector<char> packetBytes = PacketParser::packetToBytes(udpPacket, dataBuffer);
		if (sendto(this->_srtSocket, packetBytes.data(), packetBytes.size(), 0, (sockaddr*)&destAddr, sizeof(sockaddr_in)) < 0)
		{
			std::cerr << "Error while doing sendSrt. error number from wsagetlasterror: " << WSAGetLastError() << std::endl;
		}

	}
}

void SrtSocket::sendImageStream()
{
	ImageCapture capturer(NULL);
	CLSID clsid;
	ImageCapture::getEncoderClsid(L"image/jpeg", &clsid);
	std::unique_lock<std::mutex> switchLock(this->_switchesMtx);
	switchLock.unlock();
	std::unique_lock<std::mutex> sendLock(this->_packetSendQueueMtx);
	sendLock.unlock();
	bool runLoop = true;
	std::chrono::system_clock::time_point now;
	std::time_t currentTime;
	while (runLoop)
	{
		capturer.captureScreen()->Save(L"..\\captureImage.jpg", &clsid, NULL);
		std::ifstream stream = std::ifstream("..\\captureImage.jpg", std::ios::binary);
		stream.seekg(0, std::ios::end);
		int length = stream.tellg();
		stream.seekg(0, std::ios::beg);
		std::vector<char> bufferVec(length);
		stream.read(bufferVec.data(), length);
		now = std::chrono::system_clock::now();
		currentTime = std::chrono::system_clock::to_time_t(now);
		std::unique_ptr<ScreenDataPacket> packetPtr = std::make_unique<ScreenDataPacket>(-1, -1, currentTime, capturer.getScreenWidth(), capturer.getScreenHeight(), bufferVec);
		sendLock.lock();
		this->_packetSendQueue.push(packetPtr->toBuffer());
		sendLock.unlock();

		switchLock.lock();
		runLoop = !_shutdownSwitch;
		switchLock.unlock();
		Sleep(100);
	}

}

std::unique_ptr<const DefaultPacket> SrtSocket::recvSrt()
{
	int srtLength = 0;
	std::unique_ptr<const DefaultPacket> packet;
	waitForValidPacket(nullptr, [&](char* buffer, int totalLength) -> bool {
		std::vector<char> bufferVec;
		bufferVec.assign(buffer, buffer + totalLength);
		IpPacket ipHeadersRecv = PacketParser::createIpPacketFromVector(bufferVec);
		UdpPacket udpHeadersRecv = PacketParser::createUdpPacketFromVector({ bufferVec.begin() + ipHeadersRecv.getLengthOfHeadersInBytes(), bufferVec.begin() + ipHeadersRecv.getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE });
		if (!isValidHeaders(ipHeadersRecv, udpHeadersRecv))
		{
			return false;
		}
		srtLength = udpHeadersRecv.getLength() - UDP_HEADERS_SIZE;
		packet = PacketParser::createPacketFromVectorGlobal({ bufferVec.begin() + ipHeadersRecv.getLengthOfHeadersInBytes() + UDP_HEADERS_SIZE, bufferVec.end() });
		return true;
	});

	return std::move(packet);
}

void SrtSocket::initializeThreads(MODES mode)
{
	this->_recivedPacketsThread = std::thread(&SrtSocket::recvMonitoring, this);
	this->_recivedPacketsThread.detach();
	this->_sendPacketsThread = std::thread(&SrtSocket::sendMonitoring, this);
	this->_sendPacketsThread.detach();
	//this->_keepAliveMonitoringThread = std::thread(&SrtSocket::keepAliveMonitoring, this);
	//this->_keepAliveMonitoringThread.detach();
	if (mode == CONTROLLER)
	{
		this->_cursorListenerThread = std::thread(listenToCursor, &_shutdownSwitch, &_switchesMtx, std::ref(_packetSendQueue), &_packetSendQueueMtx, this->_packetManager.getOriginScreenPoint(), this->_packetManager.getEndScreenPoint(), this->_packetManager.getPointsMtx());
		this->_cursorListenerThread.detach();
<<<<<<< HEAD
		/*this->_keyboardListenerThread = std::thread(listenToKeyboard, &_shutdownSwitch, &_switchesMtx, std::ref(_packetSendQueue), &_packetSendQueueMtx);
		this->_keyboardListenerThread.detach();*/
=======
		//this->_keyboardListenerThread = std::thread(listenToKeyboard, &_shutdownSwitch, &_switchesMtx, std::ref(_packetSendQueue), &_packetSendQueueMtx);
		//this->_keyboardListenerThread.detach();
>>>>>>> f41b11f007adc65696dda9394624bbd5806e93e2
	}
	else
	{
		//this->_screenListenerThread = std::thread(&SrtSocket::sendImageStream, this);
		//this->_screenListenerThread.detach();
	}
}
