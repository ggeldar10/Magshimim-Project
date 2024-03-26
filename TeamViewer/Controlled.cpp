#include "Controlled.h"
#include <fstream>

Controlled::Controlled(PipeManager* pipeManager) : IUser(pipeManager, MODES::CONTROLLED), _screenCapture(NULL)
{
}

void Controlled::startSendingImages()
{
SendImage:
	while (!_pipeManager->isDataAvail())
	{
#pragma region CaptureScreenShot 

		CLSID clsid;
		ImageCapture::getEncoderClsid(L"image/jpeg", &clsid);
		this->_screenCapture.captureScreen()->Save(L"..\\captureImage.jpg", &clsid, NULL);
#pragma endregion

#pragma region GetFileFromStream

		std::ifstream stream = std::ifstream("..\\captureImage.jpg", std::ios::binary);
		stream.seekg(0, std::ios::end);
		int length = stream.tellg();
		stream.seekg(0, std::ios::beg);
		std::vector<char> bufferVec(length);
		stream.read(bufferVec.data(), length);

#pragma endregion

#pragma region SendData
		//checks if we got data from the user 
		//todo add a check if the controller disconnected
		if (!_pipeManager->isDataAvail())
		{
			//this->_socket. send something in the socket 
		}
#pragma endregion
	}
	// add a check to get from the user 
	std::vector<char> buffer = _pipeManager->readDataFromPipe();
	if (buffer.size() != 1 || static_cast<PIPE_CODES>(buffer[0]) != STOP_SENDING_PICTURES)
	{
		std::cout << "Started to send again" << std::endl;
		goto SendImage; // start the while again
	}
}

void Controlled::createServer(unsigned int serverPort)
{
    sockaddr_in serverInfo = { 0 };
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(serverPort);
    this->_socket.srtBind(&serverInfo);
    this->_socket.listenAndAccept();
}

