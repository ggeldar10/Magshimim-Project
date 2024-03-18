#include "ImageCapture.h"
#include <iostream>
#include <fstream>

int main()
{
	ImageCapture capturer = ImageCapture(NULL);
	std::unique_ptr<Bitmap> bitmap = capturer.captureScreen();
	CLSID clsid;
	if (ImageCapture::getEncoderClsid(L"image/jpeg", &clsid) < 0)
	{
		return 1;
	}
	bitmap->Save(L"C:\\Users\\test0\\team-viewer-project\\TeamViewer\\captureImage.jpg", &clsid, NULL);
	return 0;
}
