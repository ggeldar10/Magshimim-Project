#pragma once
#include "IUser.h"
#include "ImageCapture.h"
#include "cursor.h"
#include "keyboard.h"

class Controlled : public IUser
{
private:
	ImageCapture _screenCapture;
	
public:
	Controlled(PipeManager* pipeManager);
	~Controlled() override = default;
	void startSendingImages();
	void createServer(unsigned int serverPort);

};

