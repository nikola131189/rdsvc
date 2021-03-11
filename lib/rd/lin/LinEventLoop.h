#pragma once
#include "../Native.h"
#include <functional>

#include <cstdint>
#include <thread>
#include "../Log.h"
#include <signal.h>
#include <boost/asio.hpp>

class LinEventLoop : public Rd::Native::EventLoop
{
public:
	LinEventLoop();
	void loop();
	void blockInput(bool block);

private:
	bool _blockInput = false;
};

