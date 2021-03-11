#pragma once
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <cstdio>
#include <iostream>
#include <string>
#include <queue>
#include "Connector.hpp"

namespace Net
{
	using boost::asio::ip::tcp;
	using boost::asio::awaitable;
	using boost::asio::co_spawn;
	using boost::asio::detached;
	using boost::asio::use_awaitable;
	namespace this_coro = boost::asio::this_coro;


	class ConnectorTcp : public Connector
	{
	public:
		ConnectorTcp(std::string adr, int port)
			:
			_adr(adr), _port(port)
		{

		}

		tcp::socket connect(boost::asio::io_context& ctx, boost::system::error_code& ec)
		{
			tcp::socket sock(ctx);
			boost::asio::ip::tcp::resolver resolver{ ctx };
			auto const results = resolver.resolve(_adr, std::to_string(_port), ec);
			if (!results.empty())
			{
				boost::asio::ip::tcp::endpoint ep = *results.begin();
				sock.connect(ep, ec);
			}
			return std::move(sock);
		}


	private:
		std::string _adr;
		int _port;
	};

}
