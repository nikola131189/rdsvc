#pragma once
#include <boost/asio.hpp>
#include <queue>
#include <iostream>
#include <tuple>
#include "../crypto/crypto.h"
#include <array>
#include <boost/system/error_code.hpp>

using namespace boost::asio;

template<size_t BSIZE>
struct SecureSocket
{
private:
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
		uint8_t data[BSIZE];
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

	SecureSocket(ip::tcp::socket&& sock, const std::string& secret) :
		_sock(BOOST_ASIO_MOVE_CAST(ip::tcp::socket)(sock)),
		_ctx{ secret, {}, {}, {} },
		_readBuffer{},
		_writeBuffer{}
	{
	}

	~SecureSocket()
	{
	}

	template <typename ConnectHandler>
	void asyncConnect(const ip::tcp::endpoint& peer_endpoint, BOOST_ASIO_MOVE_ARG(ConnectHandler) handler)
	{
		_sock.async_connect(peer_endpoint, handler);
	}







	template <typename ReadHandler>
	void asyncRead(uint8_t* data, uint32_t size, BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
	{
		boost::asio::async_read(_sock, boost::asio::buffer(_readBuffer, size), boost::asio::transfer_all(),
			[this, data, size, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					_ctx.aes.decrypt(_readBuffer.data(), data, bytes_transferred);
				}
				handler(bytes_transferred, error);
			});
	}


	template <typename WriteHandler>
	void asyncWrite(const uint8_t* data, uint32_t size, BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
	{
		_ctx.aes.encrypt(data, _writeBuffer.data(), size);

		boost::asio::async_write(_sock, boost::asio::buffer(_writeBuffer, size), boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred) {
				handler(error, bytes_transferred);
			});
	}

	void cancel()
	{
		_sock.cancel();
	}

	void close()
	{
		_sock.close();
	}

	bool isOpen() { return _sock.is_open(); }

	boost::asio::ip::tcp::endpoint remoteEndpoint() { return _sock.remote_endpoint(); }
private:
	ip::tcp::socket _sock;
	EncryptionContext _ctx;
	std::array<uint8_t, BSIZE> _readBuffer, _writeBuffer;
	IdentReq _identReq;
	IdentResp _identResp;
	HandshakeReq _handshakeReq;
	HandshakeResp _handshakeResp;
























	// - ------------------------------------------------------------- server


public:
	template <typename HandshakeHandler>
	void asyncStartHandshake(HandshakeHandler handler)
	{
		boost::asio::async_read(_sock, boost::asio::buffer(&_identReq, sizeof(_identReq)), boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					sendIdentResp(handler);
				}
				else
				{
					handler(error);
				}

			});
	}


private:

	template <typename HandshakeHandler>
	void sendIdentResp(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{
		_ctx.rsa.genRsa(2048);
		std::string rsa = _ctx.rsa.pubKey();
		HMAC_SHA<512> hmac(_ctx.secret, rsa);
		auto signature = hmac.hexDigest();
		_identResp.version = 0x1;
		_identResp.keySize = rsa.size();
		memcpy(_identResp.key, &rsa[0], rsa.size());
		memcpy(_identResp.signature, &signature[0], 128);

		boost::asio::async_write(_sock, boost::asio::buffer(&_identResp, 131 + _identResp.keySize), boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					readHandshakeReq(handler);
				}
				else
				{
					handler(error);
				}

			});
	}



	template <typename HandshakeHandler>
	void readHandshakeReq(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{
		boost::asio::async_read(_sock, boost::asio::buffer(&_handshakeReq, 130), boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					readHandshakeReq1(handler);
				}
				else
				{
					handler(error);
				}

			});
	}



	template <typename HandshakeHandler>
	void readHandshakeReq1(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{
		
		boost::asio::async_read(_sock, boost::asio::buffer(_handshakeReq.key, _handshakeReq.keySize), boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					std::string signature((char*)_handshakeReq.signature, 128);
					std::string key((char *)_handshakeReq.key, _handshakeReq.keySize);
					HMAC_SHA<512> hmac(_ctx.secret, key);
					auto signature1 = hmac.hexDigest();
					if (signature1 != signature)
					{
						handler(boost::system::errc::make_error_code(boost::system::errc::bad_file_descriptor));
						return;
					}


					std::vector<uint8_t> buff;
					_ctx.rsa.decrypt(key, buff);
					_ctx.aes.init(&buff[0], buff.size());
					writeHandshakeResp(handler);
				}
				else
				{
					handler(error);
				}

			});
	}


	template <typename HandshakeHandler>
	void writeHandshakeResp(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{
		_handshakeResp.status = 0x0;
		boost::asio::async_write(_sock, boost::asio::buffer(&_handshakeResp, sizeof(_handshakeResp)), boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				handler(error);
			});
	}



	// - ------------------------------------------------------------- client

public:
	template <typename HandshakeHandler>
	void asyncHandshake(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{
		_identReq.version = 0x1;

		boost::asio::async_write(_sock, boost::asio::buffer(&_identReq, sizeof(_identReq)), boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					readIdentResp(handler);
				}
				else
				{
					handler(error);
				}

			});
	}


private:


	template <typename HandshakeHandler>
	void readIdentResp(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{
		boost::asio::async_read(_sock, boost::asio::buffer(&_identResp, 131),
			boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{
					readIdentResp1(handler);
				}
				else
				{
					handler(error);
				}
				
			});
	}





	template <typename HandshakeHandler>
	void readIdentResp1(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{
		boost::asio::async_read(_sock, boost::asio::buffer(_identResp.key, _identResp.keySize),
			boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (!error)
				{

					std::string signature((char *)_identResp.signature, 128);
					std::string key((char*)_identResp.key, _identResp.keySize);

					HMAC_SHA<512> hmac(_ctx.secret, key);
					auto signature1 = hmac.hexDigest();

					if (signature1 != signature)
					{
						handler(boost::system::errc::make_error_code(boost::system::errc::bad_file_descriptor));
						return;
					}

	
					_ctx.rsa.initPublic(key);
					crypto::rand(_ctx.key.data(), _ctx.key.size());
					_ctx.aes.init(_ctx.key.data(), _ctx.key.size());
					sendHandshakeReq(handler);
				}
				else
				{
					handler(error);
				}

			});
	}







	template <typename HandshakeHandler>
	void sendHandshakeReq(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{

		std::vector<uint8_t> buff;
		_handshakeReq.keySize = _ctx.rsa.encrypt(_ctx.key, buff);

		HMAC_SHA<512> hmac(_ctx.secret, buff);
		auto signature = hmac.hexDigest();


		memcpy(_handshakeReq.key, buff.data(), buff.size());
		memcpy(_handshakeReq.signature, signature.data(), 128);

		boost::asio::async_write(_sock, boost::asio::buffer(&_handshakeReq, 128 + 2 + _handshakeReq.keySize),
			boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				readHandshakeResp(handler);
			}); 
	}



	template <typename HandshakeHandler>
	void readHandshakeResp(BOOST_ASIO_MOVE_ARG(HandshakeHandler) handler)
	{
		boost::asio::async_read(_sock, boost::asio::buffer(&_handshakeResp, sizeof(_handshakeResp)),
			boost::asio::transfer_all(),
			[this, handler](const boost::system::error_code& error, size_t bytes_transferred)
			{
				handler(error);
			});
	}
};
