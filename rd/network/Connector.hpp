#pragma once
#include <boost/asio.hpp>

using namespace boost::asio;

class Connector
{
public:
	virtual bool connect(boost::system::error_code &ec) = 0;
	virtual operator ip::tcp::socket && () = 0;
	virtual ~Connector() {}
};


