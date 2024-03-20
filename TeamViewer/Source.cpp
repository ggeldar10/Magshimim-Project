#include "PipeManager.h"
#include "ImageCapture.h"
#include <iostream>
#include <fstream>

int main()
{
	PipeManager manager;
	std::vector<char> buffer = manager.readDataFromPipe();
	if (static_cast<PIPE_CODES>(buffer[0]) != PIPE_CODES::START_SENDING)
	{
		return 1;
	}
	while (!manager.isDataAvail())
	{
		#pragma region CaptureScreenShot

		ImageCapture capturer(NULL);
		CLSID clsid;
		
		ImageCapture::getEncoderClsid(L"image/jpeg", &clsid);
		capturer.captureScreen()->Save(L"C:\\Users\\test0\\team-viewer-project\\TeamViewer\\captureImage.jpg", &clsid, NULL);
		#pragma endregion

		#pragma region GetFileFromStream

		std::ifstream stream = std::ifstream("C:\\Users\\test0\\team-viewer-project\\TeamViewer\\captureImage.jpg", std::ios::binary);
		stream.seekg(0, std::ios::end);
		int length = stream.tellg();
		stream.seekg(0, std::ios::beg);
		std::vector<char> bufferVec(length);
		stream.read(bufferVec.data(), length);

		#pragma endregion

		#pragma region SendData

		if (!manager.isDataAvail())
		{
			manager.sendToPipe(bufferVec);
		}
		#pragma endregion
	}
	buffer = manager.readDataFromPipe();
	if (!static_cast<PIPE_CODES>(buffer[0]) == STOP_SENDING_PICTURES)
	{
		return 1;
	}
return 0;
}
