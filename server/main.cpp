#include <rd/rd.h>



#include "application.h"


#include <conio.h>

#include <libconfig.h++>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <cstdio>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;




awaitable<void> listener(tcp::acceptor acceptor, std::string secret, int connectionTimeout)
{
	auto executor = co_await this_coro::executor;

	crypto::RsaEncryptor rsa;
	rsa.genRsa(2048);

	Net::Application app;

	co_spawn(executor, app.pingLoop(connectionTimeout), detached);

	for (;;)
	{
		std::make_shared<Net::Connection>(app, co_await acceptor.async_accept(use_awaitable), secret, rsa)->start1();
	}
}



int main() 
{
	
	setlocale(LC_ALL, "Russian");
	for (;;)
	{

		try
		{

			libconfig::Config cfg;
			cfg.readFile("config2.cfg");
			std::string secret = cfg.lookup("secret").c_str();
			int port = 0;
			cfg.lookupValue("port", port);
			int connectionTimeout = 0;
			cfg.lookupValue("connectionTimeout", connectionTimeout);




			boost::asio::io_context io_context(1);

			co_spawn(io_context, listener(tcp::acceptor(io_context, { tcp::v4(), (unsigned short)port }), secret, connectionTimeout), detached);


			boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
			signals.async_wait([&](auto, auto) { io_context.stop(); });

			io_context.run();
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}

	return 0;
}



