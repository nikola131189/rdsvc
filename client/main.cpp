#include <iostream>


#include <typeinfo>
#include "FilesService.h"
#include "TransferService.h"
#include "DisplayService.h"
#include "TerminalServer.h"

#ifdef _WIN32
#include <rd/win/NativeWin.h>
#include <rd/rd.h>
#endif


#ifdef __linux__ 
#include "../rd/lin/NativeLin.h"
#include "rd/rd.h"
#endif


void networkLoop(Rd::Config& cfg);
void mainLoop(Rd::Native& native);
void loop(boost::asio::io_context& ctx);

int main()
{

	setlocale(LC_ALL, ".UTF8");



#ifdef _WIN32
	WinNative native;
#endif

#ifdef __linux__ 
	LinNative native;
#endif

	Rd::Config cfg("config.cfg");



	DisplayService displayService(native, cfg.imprint);
	EventBus::subscribe<Rd::DisplayEvent>(std::bind(&DisplayService::notify, &displayService, std::placeholders::_1));




	new std::thread(mainLoop, std::ref(native));
	networkLoop(cfg);
	return 0;
}


void networkLoop(Rd::Config& cfg)
{
	Rd::Inet::init(cfg.name, cfg.imprint);


	boost::asio::io_context ctx(1);

	bool running = true;
	boost::asio::signal_set sign(ctx, SIGINT, SIGTERM);
	sign.async_wait([&](auto, auto) { exit(0); });


	std::shared_ptr<Net::Connection> conn1 = nullptr;

	while (running)
	{

		if (!conn1 || utility::get_tick_count() - conn1->lastActive() > cfg.connectionTimeout)
		{
			for (Net::Connector* c : cfg.connectors)
			{
				boost::system::error_code ec;
				auto sock = c->connect(ctx, ec);
				if (!ec)
				{
					conn1 = std::make_shared<Net::Connection>(Rd::Inet::sess1(), std::move(sock), cfg.secret, crypto::RsaEncryptor());
					conn1->start();
					break;
				}
			}
		}
		ctx.run_for(std::chrono::milliseconds(1000));
	}
}




void mainLoop(Rd::Native& native)
{
	boost::asio::io_context ctx;

	FilesService filesService(ctx);
	EventBus::subscribe<Rd::FilesEvent>(std::bind(&FilesService::notify, &filesService, std::placeholders::_1));

	TransferService transferService(ctx);
	EventBus::subscribe<Rd::TransferEvent>(std::bind(&TransferService::notify, &transferService, std::placeholders::_1));

	TerminalServer  terminalServer(ctx, native.system().terminalApp());

	EventBus::subscribe<Rd::ConnectionOpen>([](Rd::ConnectionOpen e) {std::cout << "connectd" << std::endl; });

	EventBus::subscribe<Rd::ConnectionError>([](const Rd::ConnectionError& ev) {std::cout << ev.msg << std::endl; });


	loop(ctx);
}

void loop(boost::asio::io_context& ctx)
{
	for (;;)
	{
		try
		{
			boost::asio::signal_set signals(ctx, SIGINT, SIGTERM);
			signals.async_wait([&](auto, auto) { exit(0); });
			while (true)
			{
				ctx.run_for(std::chrono::milliseconds(3000));
			}
		}
		catch (std::exception const& e)
		{
			LOG(ERR) << "exception: " << utility::localToU8(e.what());
		}
	}
}