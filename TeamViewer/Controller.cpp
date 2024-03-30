//#include "Controller.h"
//
//// delete it after switching:
//#include "imageCapture.h" 
//#include <fstream>
//
//Controller::Controller(PipeManager* pipeManager) : IUser(pipeManager, MODES::CONTROLLER)
//{
//}
//
///* 
//* Connects the socket to the server
//* input: 
//* portNum - the port number of the server
//* serverIp - the ip of the server as a string
//* output:
//* none
//*/
//void Controller::connectToServer(uint16_t portNum, const std::string& serverIp)
//{
//    //sockaddr_in serverInfo = { 0 };
//    //serverInfo.sin_addr.s_addr = inet_addr(serverIp.c_str());
//    //serverInfo.sin_family = AF_INET;
//    //serverInfo.sin_port = htons(portNum);
//
//    //this->_socket.connectToServer(&serverInfo);
//    //std::cout << "connected" << std::endl;
//}
//
//void Controller::startImageStream()
//{
//    // get the images from a socket
//	std::vector<char> buffer = _pipeManager->readDataFromPipe();
//	if (static_cast<PIPE_CODES>(buffer[0]) != PIPE_CODES::START_SENDING)
//	{
//		throw 1; // add exception
//	}
//	SendImageToGui:
//	while (!_pipeManager->isDataAvail())
//	{
//		/*
//		* Send to the server start sending pictures
//		* Send the data given to the gui to show 
//		* if i got data from the gui to stop or from the server to stop then:
//		* stop sending 
//		* send to the server stop sending
//		* else do it again
//		*/
//
//		//switch captrueScreenShot and get FileFrom Stream with recv from controlled
//#pragma region CaptureScreenShot 
//
//		ImageCapture capturer(NULL);
//		CLSID clsid;
//
//		ImageCapture::getEncoderClsid(L"image/jpeg", &clsid);
//		capturer.captureScreen()->Save(L"C:\\Users\\test0\\team-viewer-project\\TeamViewer\\captureImage.jpg", &clsid, NULL);
//#pragma endregion
//
//#pragma region GetFileFromStream
//
//		std::ifstream stream = std::ifstream("C:\\Users\\test0\\team-viewer-project\\TeamViewer\\captureImage.jpg", std::ios::binary);
//		stream.seekg(0, std::ios::end);
//		int length = stream.tellg();
//		stream.seekg(0, std::ios::beg);
//		std::vector<char> bufferVec(length);
//		stream.read(bufferVec.data(), length);
//
//#pragma endregion
//
//#pragma region SendData
//
//		if (!_pipeManager->isDataAvail())
//		{
//			_pipeManager->sendToPipe(bufferVec);
//		}
//#pragma endregion
//	}
//	buffer = _pipeManager->readDataFromPipe();
//	if (buffer.size() != 1 || static_cast<PIPE_CODES>(buffer[0]) != STOP_SENDING_PICTURES )
//	{
//		std::cout << "Started to send again" << std::endl;
//		goto SendImageToGui; // start the while again
//	}
//}
//
//void Controller::intilizeCursorAndKeyboard()
//{
//}
//
