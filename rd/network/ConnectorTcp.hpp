#pragma once
#include "Connector.hpp"


class ConnectorTcp : public Connector
{
public:
	ConnectorTcp(boost::asio::io_context& ctx, const std::string& adr, int port) : 
		_ctx(ctx), _sock(ctx), _adr(adr), _port(port)
	{

	}

	bool connect(boost::system::error_code& ec)
	{
		boost::asio::ip::tcp::resolver resolver{ _ctx };
		auto const results = resolver.resolve(_adr, std::to_string(_port), ec);
		if (!results.empty())
		{
			boost::asio::ip::tcp::endpoint ep = *results.begin();
			_sock.connect(ep, ec);
			if (ec)
				return false;
			return true;
		}
		else
		{
			return false;
		}
	}


	operator ip::tcp::socket && ()
	{
		return std::move(_sock);
	}

	~ConnectorTcp() {}
private:
	boost::asio::io_context& _ctx;
	ip::tcp::socket _sock;
	std::string _adr;
	int _port;
};



