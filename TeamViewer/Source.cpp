#include "PipeManager.h"
#include "ImageCapture.h"
#include <iostream>
#include <fstream>

int main()
{
	PipeManager manager;
	while (true)
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

		manager.sendToPipe(bufferVec);

		#pragma endregion
	}
return 0;
}
