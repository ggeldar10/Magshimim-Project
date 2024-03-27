#include "ImageCapture.h"
#include <memory>


ImageCapture::ImageCapture(HWND hWindow)
{
	SetProcessDPIAware();
	this->_hdcScreen = GetDC(hWindow);
	this->_screenshotMem = CreateCompatibleDC(this->_hdcScreen);
	GdiplusStartup(&this->_gdipToken, &this->_gdip, NULL);
	this->_width = GetSystemMetrics(SM_CXSCREEN);
	this->_height = GetSystemMetrics(SM_CYSCREEN);
	this->_hbitmap = CreateCompatibleBitmap(this->_hdcScreen, this->_width, this->_height);
}

ImageCapture::~ImageCapture()
{
	GdiplusShutdown(this->_gdipToken);
	DeleteObject(this->_hbitmap);
	DeleteObject(this->_screenshotMem);
	ReleaseDC(NULL, this->_hdcScreen);
}

std::unique_ptr<Bitmap> ImageCapture::captureScreen()
{
	SelectObject(this->_screenshotMem, this->_hbitmap);

	if (!BitBlt(this->_screenshotMem, 0, 0, this->_width, this->_height, this->_hdcScreen, 0, 0, SRCCOPY))
	{
		std::cerr << "Error while trying to capture a screenshot" << std::endl;
		throw;
	}
	CURSORINFO cursor = { 0 };
	cursor.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&cursor);
	if (cursor.flags == CURSOR_SHOWING)
	{
		DrawIconEx(this->_screenshotMem, cursor.ptScreenPos.x, cursor.ptScreenPos.y, cursor.hCursor, 0, 0, 0, NULL, DI_NORMAL);
	}
	return std::unique_ptr<Bitmap>(new Bitmap(this->_hbitmap, (HPALETTE)0));
}

uint32_t ImageCapture::getScreenWidth() const
{
	return _width;
}

uint32_t ImageCapture::getScreenHeight() const 
{
	return _height;
}

int ImageCapture::getEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}
