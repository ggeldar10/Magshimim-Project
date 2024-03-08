#pragma once
#include <windows.h>
#include <vector>

typedef union {
	int num;
	char bytes[sizeof(int)]; // might have a problem if it does add a constant
} ConvertIntToChar;

class PipeManager
{
private:
	HANDLE _pipe;
	const char* serverName = "\\\\.\\pipe\\srtGuiPipe";
public:
	PipeManager();
	~PipeManager();
	void sendToPipe(const std::vector<char>& data);
	std::vector<char> readFromPipe(int numOfBytesToRead);

};

