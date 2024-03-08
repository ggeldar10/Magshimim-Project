#include "PipeManager.h"
#include <iostream>

int main()
{
	PipeManager manager;
	for (char c : manager.readDataFromPipe())
	{
		std::cout << c;
	}
	std::cout << std::endl;
	std::string msg = "Hello back to you too";
	std::vector<char> a(msg.begin(), msg.end());
	manager.sendToPipe(a);
	return 0;
}
