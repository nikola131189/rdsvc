#include "Connection.h"

namespace Net
{
	Connection::Connection(SessionHandler& handler, tcp::socket sock, std::string secret, crypto::RsaEncryptor rsa)
		:
		_sock(std::move(sock)),
		_timer(_sock.get_executor()),
		_handler(handler)
	{
		_ctx.secret = secret;
		_ctx.rsa = rsa;
		_timer.expires_at(std::chrono::steady_clock::time_point::max());
		_lastActive = utility::get_tick_count();
	}

	void Connection::start()
	{
		_lastActive = utility::get_tick_count();
		_sock.set_option(boost::asio::ip::tcp::no_delay(true));
		co_spawn(_sock.get_executor(), [self = shared_from_this()]{ return self->sendHandshake(); }, detached);
	}

	void Connection::start1()
	{
		_lastActive = utility::get_tick_count();
		_sock.set_option(boost::asio::ip::tcp::no_delay(true));
		co_spawn(_sock.get_executor(), [self = shared_from_this()]{ return self->recvHandshake(); }, detached);
	}



	void Connection::write(Packet&& pack)
	{
		if (!_sock.is_open())
			return;

		_writeQueue.push(std::move(pack));
		_timer.cancel_one();
	}


	void Connection::write(const Packet& pack)
	{
		if (!_sock.is_open())
			return;

		_writeQueue.push(pack);
		_timer.cancel_one();
	}

	awaitable<void> Connection::reader()
	{
		try
		{
			while (_sock.is_open())
			{
				uint8_t buff[packet_max_size];
				co_await async_read(_sock, boost::asio::buffer(buff, 16), use_awaitable);
				Packet p;
				_ctx.aes.decrypt(buff, (uint8_t*)&p, 16);

				if (p.size > 0)
				{
					co_await async_read(_sock, boost::asio::buffer(buff, utility::closet_multiple(p.size, 16)), use_awaitable);
					_ctx.aes.decrypt(buff, p.buf, p.size);
				}
				_lastActive = utility::get_tick_count();
				_handler.onRead(shared_from_this(), std::move(p));
			}
		}
		catch (std::exception& e)
		{
			_sock.close();
			_timer.cancel();
			_handler.onError(shared_from_this(), e.what());
		}
	}

	awaitable<void> Connection::writer()
	{
		try
		{
			while (_sock.is_open())
			{
				if (_writeQueue.empty())
				{
					boost::system::error_code ec;
					co_await _timer.async_wait(redirect_error(use_awaitable, ec));
				}
				else
				{
					auto& p = _writeQueue.front();

					uint8_t buff[16 + packet_max_size];
					int s = 16 + utility::closet_multiple(p.size, 16);
					_ctx.aes.encrypt((const uint8_t*)&p, buff, s);

					co_await async_write(_sock, boost::asio::buffer(buff, s), use_awaitable);
					if (p.cbck) p.cbck();
					_writeQueue.pop();
				}
			}
		}
		catch (std::exception& e)
		{
			_sock.close();
			_timer.cancel();
			_handler.onError(shared_from_this(), e.what());
		}
	}

	awaitable<void> Connection::sendHandshake()
	{
		try
		{
			IdentReq identReq = { 0x1 };
			co_await async_write(_sock, boost::asio::buffer(&identReq, sizeof(identReq)), use_awaitable);

			IdentResp identResp;
			co_await async_read(_sock, boost::asio::buffer(&identResp, 131), use_awaitable);

			co_await async_read(_sock, boost::asio::buffer(&identResp.key, identResp.keySize), use_awaitable);

			std::string signature((char*)identResp.signature, 128);
			std::string key((char*)identResp.key, identResp.keySize);

			auto signature1 = HMAC_SHA<512>(_ctx.secret, key).hexDigest();

			if (signature1 != signature)
				throw HandshakeException();

			_ctx.rsa.initPublic(key);
			crypto::rand(_ctx.key.data(), _ctx.key.size());
			_ctx.aes.init(_ctx.key.data(), _ctx.key.size());
			

			HandshakeReq handshakeReq;
			std::vector<uint8_t> buff;
			handshakeReq.keySize = _ctx.rsa.encrypt(_ctx.key, buff);

			signature = HMAC_SHA<512>(_ctx.secret, buff).hexDigest();

			memcpy(handshakeReq.key, buff.data(), buff.size());
			memcpy(handshakeReq.signature, signature.data(), 128);

			co_await async_write(_sock, boost::asio::buffer(&handshakeReq, 128 + 2 + handshakeReq.keySize), use_awaitable);


			HandshakeResp handshakeResp;
			co_await async_read(_sock, boost::asio::buffer(&handshakeResp, sizeof(handshakeResp)), use_awaitable);

			_handler.onStart(shared_from_this());
			co_spawn(_sock.get_executor(), [self = shared_from_this()]{ return self->reader(); }, detached);
			co_spawn(_sock.get_executor(), [self = shared_from_this()]{ return self->writer(); }, detached);

		}
		catch (std::exception& e)
		{
			_sock.close();
			_timer.cancel();
			_handler.onError(shared_from_this(), e.what());
		}
	}

	awaitable<void> Connection::recvHandshake()
	{
		try
		{
			IdentReq identReq;
			co_await async_read(_sock, boost::asio::buffer(&identReq, sizeof(identReq)), use_awaitable);

			auto rsa = _ctx.rsa.pubKey();
			auto signature = HMAC_SHA<512>(_ctx.secret, rsa).hexDigest();

			IdentResp identResp;
			identResp.version = 0x1;
			identResp.keySize = rsa.size();

			memcpy(identResp.key, &rsa[0], rsa.size());
			memcpy(identResp.signature, &signature[0], 128);

			co_await async_write(_sock, boost::asio::buffer(&identResp, 131 + identResp.keySize), use_awaitable);

			HandshakeReq handshakeReq;
			co_await async_read(_sock, boost::asio::buffer(&handshakeReq, 130), use_awaitable);

			co_await async_read(_sock, boost::asio::buffer(handshakeReq.key, handshakeReq.keySize), use_awaitable);


			signature = std::string((char*)handshakeReq.signature, 128);
			std::string key((char*)handshakeReq.key, handshakeReq.keySize);

			if (HMAC_SHA<512>(_ctx.secret, key).hexDigest() != signature)
				throw HandshakeException();


			std::vector<uint8_t> buff;
			_ctx.rsa.decrypt(key, buff);
			_ctx.aes.init(&buff[0], buff.size());

			HandshakeResp handshakeResp;
			handshakeResp.status = 0x0;
			co_await async_write(_sock, boost::asio::buffer(&handshakeResp, sizeof(handshakeResp)), use_awaitable);

			_handler.onStart(shared_from_this());
			co_spawn(_sock.get_executor(), [self = shared_from_this()]{ return self->reader(); }, detached);
			co_spawn(_sock.get_executor(), [self = shared_from_this()]{ return self->writer(); }, detached);

		}
		catch (std::exception& e)
		{		
			_sock.close();
			_timer.cancel();
			_handler.onError(shared_from_this(), e.what());
		}

	}


	void Connection::stop()
	{
		_sock.close();
		_timer.cancel();
	}

	boost::asio::ip::tcp::endpoint Connection::remote_endpoint()
	{
		if(_sock.is_open())
			return _sock.remote_endpoint(); 
		return boost::asio::ip::tcp::endpoint();
	}


}


 