#include "IUser.h"

IUser::IUser(PipeManager* pipeManager, MODES mode): _socket(), _pipeManager(pipeManager), _type(mode)
{
	
}
