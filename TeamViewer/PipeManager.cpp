#include "PipeManager.h"
#include <iostream>


PipeManager::PipeManager()
{
	this->_pipe = CreateFileA(this->serverName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (this->_pipe == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Error while creating pipe " << GetLastError() << std::endl;
		throw "Error while creating pipe";
	}

}

PipeManager::~PipeManager()
{
	CloseHandle(this->_pipe);

}

/*
sends to the server pipe
input: only the data needed to be sent
output: none
*/
void PipeManager::sendToPipe(const std::vector<char>& data) const
{
	IntCharUnion convertor;
	convertor.num = data.size();
	std::vector<char> dataWithLength;
	dataWithLength.insert(dataWithLength.begin(), convertor.bytes, convertor.bytes + sizeof(int));
	dataWithLength.insert(dataWithLength.end(), data.begin(), data.end());
	if (!WriteFile(this->_pipe, dataWithLength.data(), dataWithLength.size(), NULL, NULL))
	{
		std::cerr << "Error while sending" << std::endl;
		throw "Error while sending";
	}
}

std::vector<char> PipeManager::readDataFromPipe() const
{
	IntCharUnion convertor;
	if (!ReadFile(this->_pipe, convertor.bytes, sizeof(int), NULL, NULL))
	{
		std::cerr << "Error while reading" << std::endl;
		throw "Error while reading";
	}
	std::vector<char> buffer = std::vector<char>(convertor.num);
	if (!ReadFile(this->_pipe, buffer.data(), convertor.num, NULL, NULL))
	{
		std::cerr << "Error while reading" << std::endl;
		throw "Error while reading";
	}
	return buffer;
}

bool PipeManager::isDataAvail()
{
	DWORD bytesAvailable = 0;
	if (!PeekNamedPipe(this->_pipe, NULL, NULL, NULL, &bytesAvailable, NULL))
	{
		std::cerr << "Error while peeking " << std::endl;
		throw "Error while peeking";
	}
	
	return bytesAvailable;
}

