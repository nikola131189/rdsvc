#pragma once
#include <boost/asio.hpp>

namespace Net
{

	using namespace boost::asio;

	class Connector
	{
	public:
		virtual ip::tcp::socket connect(boost::asio::io_context& ctx, boost::system::error_code& ec) = 0;
		virtual ~Connector() {}
	};

}
