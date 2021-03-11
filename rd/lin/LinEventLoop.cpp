#include "LinEventLoop.h"

LinEventLoop::LinEventLoop()

{
}


void LinEventLoop::loop()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

}


void LinEventLoop::blockInput(bool block)
{
	_blockInput = block;
}






