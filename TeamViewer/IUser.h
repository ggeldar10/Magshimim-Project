#pragma once
#include "SrtSocket.h"
#include "PipeManager.h"

class IUser
{
protected:
	SrtSocket _socket;
	PipeManager* _pipeManager;
	MODES _type;

public:
	IUser(PipeManager* pipeManager, MODES mode);
	virtual ~IUser() = default;
}; 

