#pragma once
#include "IUser.h"
#include "ImageCapture.h"
#include "cursor.h"
#include "keyboard.h"

class Controlled : public IUser
{
private:
	ImageCapture _screenCapture;

};

