#pragma once
#include <coroutine>
#include <iostream>
#include <optional>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/steady_timer.hpp>
#include <cstdio>
#include <iostream>
#include "Packet.h"
#include <queue>
#include "Session.h"
#include "../crypto/crypto.h"
#include "../utility.h"


namespace Net
{


	using boost::asio::ip::tcp;
	using boost::asio::awaitable;
	using boost::asio::co_spawn;
	using boost::asio::detached;
	using boost::asio::use_awaitable;
	namespace this_coro = boost::asio::this_coro;






	class Connection : public std::enable_shared_from_this<Connection>, public Session
	{

		struct EncryptionContext
		{
			std::string secret;
			crypto::AesEncryptor<256> aes;
			crypto::RsaEncryptor rsa;
			std::array<uint8_t, 32> key;
		};


#pragma pack(push, 1)
		struct HandshakeMessage
		{
			uint32_t dataSize, signatureSize;
			uint8_t data[4096];
		};



		struct IdentReq
		{
			uint8_t version;
		};


		struct IdentResp
		{
			uint8_t version;
			uint8_t signature[128];
			uint16_t keySize;
			uint8_t key[2048];
		};


		struct HandshakeReq
		{
			uint8_t signature[128];
			uint16_t keySize;
			uint8_t key[2048];
		};


		struct HandshakeResp
		{
			uint8_t status;
		};
#pragma pack(pop)


	public:

		struct HandshakeException : public std::exception
		{
		public:
			const char* what() const throw ()
			{
				return "signatures do not match";
			}
		};

	public:
		Connection(SessionHandler& handler, tcp::socket sock, std::string secret, crypto::RsaEncryptor rsa);
		~Connection() {}
		void start();
		void start1();
		void write(const Packet& pack);
		void write(Packet&& pack);
		void stop();

		uint64_t lastActive() { return _lastActive; }
		boost::asio::any_io_executor get_executor() { return _sock.get_executor(); }
		boost::asio::ip::tcp::endpoint remote_endpoint();
	private:


		awaitable<void> reader();
		awaitable<void> writer();
		awaitable<void> sendHandshake();
		awaitable<void> recvHandshake();

		std::queue<Packet> _writeQueue;
		tcp::socket _sock;
		boost::asio::steady_timer _timer;
		EncryptionContext _ctx;

		SessionHandler& _handler;

		uint64_t _lastActive = 0;
	};
}
