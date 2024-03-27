#include "IUser.h"

IUser::IUser(PipeManager* pipeManager, MODES mode): _socket(pipeManager), _pipeManager(pipeManager), _type(mode)
{
	
}
