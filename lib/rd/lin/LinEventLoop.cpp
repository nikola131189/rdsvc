#include "LinEventLoop.h"

LinEventLoop::LinEventLoop()

{
}


void LinEventLoop::loop()
{
	
	boost::asio::io_context ctx;
	boost::asio::signal_set signals(ctx, SIGINT, SIGTERM);
	signals.async_wait([&](auto, auto) { exit(0); });
	ctx.run();
}


void LinEventLoop::blockInput(bool block)
{
	_blockInput = block;
}






