#include "ScreenService.h"


ScreenService::ScreenService(boost::asio::io_context& ctx, Rd::Native& native)
	: _ctx(ctx), _running(false), _dest(0), _native(native)
{

}

bool ScreenService::init(uint32_t dest, int sid, const codec::VideoFormat& fmt)
{
	_native.screenCapture().setResolution(fmt.w, fmt.h);
	
	_ev.screenId = sid;
	
	_dest = dest;
	_ev.videoFormat = fmt;

	_encoder = codec::CodecFactory::makeEncoder(fmt);
	bool ret = _encoder->init(fmt);
	if (!ret)
		return 0;

	_buffer.resize(_ev.videoFormat.dw * _ev.videoFormat.dh * 3);
	_ev.data.resize(_ev.videoFormat.dw * _ev.videoFormat.dh * 3);

	ret = _native.screenCapture().init(_ev.videoFormat.dw, _ev.videoFormat.dh);

	return 1;
}

void ScreenService::start()
{
	_running = true;
	boost::asio::post(_ctx, std::bind(&ScreenService::frame, this));
}

void ScreenService::stop()
{
	_running = false;
}




void ScreenService::frame()
{
	if (!_running) return;
	
	int res = _native.screenCapture().frame(&_buffer[0], _buffer.size(), _ev.cursor);
	if (res <= 0)
	{
		frame();
		return;
	}

	bool ret = _encoder->encode(&_buffer[0], _buffer.size(), &_ev.data[0], _ev.size);
		
	Rd::Inet::send(_dest, _ev, [this]() {
		if (!_running) return;
		boost::asio::post(_ctx, std::bind(&ScreenService::frame, this));
	});
}
