#include "Application.h"

Application::Application(Rd::Native& native, const Rd::Config& cfg, uint32_t dest, int sid, const codec::VideoFormat& fmt)
	:
	_dest(dest),
	_native(native),
	_cfg(cfg),
	_screenService(_ctx1, _native)
{

	_screenService.init(dest, sid, fmt);
	_native.clipBoard().setCbck([this](const std::string& arg) {
		Rd::ClipboardEvent ev;
		ev.text = arg;
		Rd::Inet::send(_dest, ev);
	});

	native.action().init();


	EventBus::subscribe<Rd::ConnectionOpen>([&](const Rd::ConnectionOpen& ev) {
			 _screenService.start();
		});

	EventBus::subscribe<Rd::ConnectionError>([&](const Rd::ConnectionError& ev) {
			_screenService.stop();
		});

	EventBus::subscribe<Rd::ClientNotFound>([&](const Rd::ClientNotFound& ev) {	
			if (_dest == ev.id)
				_screenService.stop();  
		});

	EventBus::subscribe<Rd::ActionEvent>([&](const Rd::ActionEvent& ev) {
		_ctx2.post([&, ev]() {
			if (auto pval = std::get_if<Rd::ActionEvent::Keybd>(&ev.val))
				_native.action().onKeybd(*pval);


			if (auto pval = std::get_if<Rd::ActionEvent::Mouse>(&ev.val))
				_native.action().onMouse(*pval);
			
		});

	});


	EventBus::subscribe<Rd::ClipboardEvent>([&](const Rd::ClipboardEvent& ev) {
			_native.clipBoard().setData(ev.text);
		});


	thr1 = std::thread(&Application::inetLoop, this);
	thr2 = std::thread(&Application::loop, this, std::ref(_ctx1));
	thr3 = std::thread(&Application::loop, this, std::ref(_ctx2));

	thr1.detach();
	thr2.detach();
	thr3.detach();
}




void Application::loop(boost::asio::io_context& ctx)
{
#ifdef _WIN32
	HDESK hdesk = OpenInputDesktop(0, true, GENERIC_ALL);
	SetThreadDesktop(hdesk);
	CloseDesktop(hdesk);
#endif

	try
	{
		boost::asio::signal_set signals(ctx, SIGINT, SIGTERM);
		signals.async_wait([&](auto, auto) { exit(0); });
		ctx.run();
	}
	catch (std::exception const& e)
	{
		LOG(ERR) << "exception: " << utility::localToU8(e.what());
	}
}

void Application::inetLoop()
{
#ifdef _WIN32
	HDESK hdesk = OpenInputDesktop(0, true, GENERIC_ALL);
	SetThreadDesktop(hdesk);
	CloseDesktop(hdesk);
#endif


	Rd::Inet::init(_cfg.name, _cfg.imprint);


	boost::asio::io_context ctx(1);

	bool running = true;
	boost::asio::signal_set sign(ctx, SIGINT, SIGTERM);
	sign.async_wait([&](auto, auto) {
		exit(0);
		});


	std::shared_ptr<Net::Connection> conn2 = nullptr;

	while (1)
	{

		if (!conn2 || utility::get_tick_count() - conn2->lastActive() > _cfg.connectionTimeout)
		{
			for (Net::Connector* c : _cfg.connectors)
			{
				boost::system::error_code ec;
				auto sock = c->connect(ctx, ec);
				if (!ec)
				{
					conn2 = std::make_shared<Net::Connection>(Rd::Inet::sess2(), std::move(sock), _cfg.secret, crypto::RsaEncryptor());
					conn2->start();
					break;
				}
			}
		}
		ctx.run_for(std::chrono::milliseconds(1000));
	}
}
