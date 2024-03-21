#pragma once
#include "IUser.h"
#include <string>


class Controller : public IUser
{
public:
	Controller(PipeManager* pipeManager);
	~Controller() override = default;
	void connectToServer(uint16_t portNum, const std::string& serverIp);
	void startImageStream();
	void intilizeCursorAndKeyboard();
	
};

