//#include "ImageCapture.h"
//#include <iostream>
//#include <fstream>
//
//int main()
//{
//	ImageCapture capturer = ImageCapture(NULL);
//	std::unique_ptr<Bitmap> bitmap = capturer.captureScreen();
//	CLSID clsid;
//	if (ImageCapture::getEncoderClsid(L"image/jpeg", &clsid) < 0)
//	{
//		return 1;
//	}
//	bitmap->Save(L"C:\\Users\\test0\\team-viewer-project\\TeamViewer\\captureImage.jpg", &clsid, NULL);
//	std::ifstream stream = std::ifstream("C:\\Users\\test0\\team-viewer-project\\TeamViewer\\captureImage.jpg");
//	stream.seekg(0, std::ios::end);
//	int length = stream.tellg();
//	stream.seekg(0, std::ios::beg);
//	char* buffer = new char[length];
//	stream.read(buffer, length);
//
//	delete[] buffer;
//	return 0;
//}
