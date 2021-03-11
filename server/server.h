#pragma once

#include "Connection.h"

using namespace boost::asio;
namespace Serv
{

	template <typename T>
	class Server
	{
	public:
		Server(boost::asio::io_context& ctx, const ip::tcp::endpoint& endpoint, ConnectionHandler& h, const std::string& secret) :
			connectionHandler(h),
			acceptor_(ctx, endpoint),
			sock_(ctx),
			_secret(secret)
		{
			//_rsa.initPublic(crypto::RsaEncryptor::readKeyFromFile("key/public_key.pem"));
			//_rsa.initPrivate(crypto::RsaEncryptor::readKeyFromFile("key/private_key.pem"));

		}

		void start()
		{
			do_accept();
		}

		~Server()
		{

		}


		void stop()
		{
			acceptor_.close();
		}


	private:
		void do_accept()
		{
			acceptor_.async_accept(sock_,
				[this](boost::system::error_code ec)
				{
					if (!ec)
					{
						auto c = std::make_shared<T>(std::move(sock_), connectionHandler, _secret);
						c->start();
					}
					if (!acceptor_.is_open())
						return;
					do_accept();
				});
		}


		ConnectionHandler& connectionHandler;
		ip::tcp::acceptor acceptor_;

		boost::asio::ip::tcp::socket sock_;
		std::string _secret;
	};
}
