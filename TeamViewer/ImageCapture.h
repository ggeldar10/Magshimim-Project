#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <iostream>

#pragma comment(lib,"gdiplus.lib")

using namespace Gdiplus;

class ImageCapture
{
private:
	HDC _hdcScreen;
	HDC _screenshotMem;
	unsigned int _width;
	unsigned int _height;
	HBITMAP _hbitmap;
	GdiplusStartupInput _gdip;
	ULONG_PTR _gdipToken;

public:
	ImageCapture(HWND hWindow);
	~ImageCapture();
	std::unique_ptr<Bitmap> captureScreen();
	uint32_t getScreenWidth() const;
	uint32_t getScreenHeight() const;
	

	// A function from msdn to get the clsid
	// format can be:
	// image/png, image/jpeg, image/bmp
	static int getEncoderClsid(const WCHAR* format, CLSID* pClsid);

};

