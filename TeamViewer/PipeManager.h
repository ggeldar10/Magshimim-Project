#pragma once
#include <windows.h>
#include <vector>

typedef union {
	int num;
	char bytes[sizeof(int)];
} ConvertIntToChar;

class PipeManager
{
private:
	HANDLE _pipe;
	const char* serverName = "\\\\.\\pipe\\srtGuiPipe";
public:
	PipeManager();
	~PipeManager();
	void sendToPipe(const std::vector<char>& data) const;
	std::vector<char> readDataFromPipe() const;

};

