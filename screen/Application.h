#pragma once

#include "rd/rd.h"
#include "ScreenService.h"



class Application
{
public:
	Application(Rd::Native& native, const Rd::Config& cfg, uint32_t dest, int sid, const codec::VideoFormat& fmt);
private:
	void loop(boost::asio::io_context& ctx);
	void inetLoop();
private:

	boost::asio::io_context _ctx1, _ctx2;
	
	std::thread thr1, thr2, thr3;
private:
	Rd::Native& _native;
	ScreenService _screenService;
	uint32_t _dest;
	Rd::Config _cfg;
};

