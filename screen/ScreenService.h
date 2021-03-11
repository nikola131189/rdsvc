#pragma once

#include <rd/rd.h>
#include <rd/codec/CodecFactory.h>

class ScreenService
{
public:
	ScreenService(boost::asio::io_context& ctx, Rd::Native& native);
	bool init(uint32_t dest, int sid, const codec::VideoFormat& fmt);
	void start();
	void stop();
private:
	void frame();

	bool _running;
	std::vector<uint8_t> _buffer, _buffer1;
	Rd::Native& _native;

	std::shared_ptr<codec::Encoder> _encoder;
	uint32_t _dest;

	Rd::VideoEvent _ev;
	boost::asio::io_context& _ctx;
};

