#pragma once
#include "Connector.hpp"
namespace Net
{
	enum class ConnectorSocks5Errc
	{
		Success = 0,  // 0 should not represent an error
		GeneralFailure = 1,
		ConnectionNotAllowed = 2,
		NetworkUnreachable = 3,
		HostUnreachable = 4,
		ConnectionRefusedByDestinationHost = 5,
		TTLExpired = 6,
		CommandNotSupported = 7,
		AddressTypeNotSupported = 8

	};



	class ConnectorSocks5Errc_category : public boost::system::error_category
	{
	public:
		virtual const char* name() const noexcept override final { return "Socks5ErrorCategory"; }


		virtual std::string message(int c) const override final
		{
			switch (static_cast<ConnectorSocks5Errc>(c))
			{
			case ConnectorSocks5Errc::Success: return "Success";
			case ConnectorSocks5Errc::GeneralFailure: return "general failure";
			case ConnectorSocks5Errc::ConnectionNotAllowed: return "connection not allowed by ruleset";
			case ConnectorSocks5Errc::NetworkUnreachable: return "network unreachable";
			case ConnectorSocks5Errc::HostUnreachable: return "host unreachable";
			case ConnectorSocks5Errc::ConnectionRefusedByDestinationHost: return "connection refused by destination host";
			case ConnectorSocks5Errc::TTLExpired: return "TTL expired";
			case ConnectorSocks5Errc::CommandNotSupported: return "command not supported / protocol error";
			case ConnectorSocks5Errc::AddressTypeNotSupported: return "address type not supported";
			default: return "unknown";
			}
		}

		virtual boost::system::error_condition default_error_condition(int c) const noexcept override final
		{
			return boost::system::error_condition(c, *this);
		}

	};


	const ConnectorSocks5Errc_category connectorSocks5Errc_category{};


	inline boost::system::error_code make_error_code(ConnectorSocks5Errc e)
	{
		return { static_cast<int>(e), connectorSocks5Errc_category };
	}



	class ConnectorSocks5 : public Connector
	{







#pragma pack(push, 1)
		struct IdentReq
		{
			uint8_t version = 0x5;
			uint8_t methodsNum = 0x0;
			uint8_t methods[256];
		};


		struct IdentResp
		{
			uint8_t version = 0x0;
			uint8_t method = 0x0;
		};

		struct AuthReq
		{
			uint8_t version = 0x5;
			uint8_t cmd = 0x0;
			uint8_t reserved = 0x0;
			uint8_t addrType = 0x0;
			union {
				uint8_t IPv4[4];
				uint8_t IPv6[16];
				struct {
					uint8_t domainLen;
					uint8_t domain[256];
				};
			} destAddr;
			uint16_t destPort;
		};

		struct AuthResp
		{
			uint8_t version = 0x0;
			uint8_t reply = 0x0;
			uint8_t reserved = 0x0;
			uint8_t addrType = 0x0;
			union {
				uint8_t IPv4[4];
				uint8_t IPv6[16];
				struct {
					uint8_t domainLen;
					uint8_t domain[256];
				};
			} bindAddr;
			uint16_t bindPort;
		};

#pragma pack(pop)

	public:
		ConnectorSocks5(const std::string& adr, uint16_t port, const std::string& adr5, uint16_t port5) :
			_adr(adr), _port(port), _adr5(adr5), _port5(port5)
		{
		}

		ip::tcp::socket connect(boost::asio::io_context& ctx, boost::system::error_code& ec)
		{
			_sock = std::make_shared<ip::tcp::socket>(ctx);
			size_t res = 0;
			boost::asio::ip::tcp::resolver resolver{ ctx };
			auto const results = resolver.resolve(_adr5, std::to_string(_port5), ec);
			if (!results.empty())
			{
				boost::asio::ip::tcp::endpoint ep = *results.begin();
				_sock->connect(ep, ec);
				if (ec)
					return std::move(*_sock);
				login(_sock, ec);
				if (ec)
					return std::move(*_sock);;

				AuthReq req;
				AuthResp resp;

				req.cmd = 0x1;
				req.addrType = 0x3;
				req.destAddr.domainLen = _adr.size();
				memcpy(&req.destAddr.domain, &_adr[0], req.destAddr.domainLen);
				req.destPort = _port;

				socksRequest(_sock, req, resp, ec);
			}
			return std::move(*_sock);
		}
		
		std::string address() { return _adr; }

		void cancel() {
			if (_sock && _sock->is_open())
				_sock->close();
		}
		
		~ConnectorSocks5() {}
	private:
		bool login(std::shared_ptr<ip::tcp::socket>& sock, boost::system::error_code& ec)
		{
			IdentReq req;
			req.methodsNum = 0x1;
			req.methods[0] = 0x0;

			boost::asio::write(*sock, boost::asio::buffer(&req, 2 + req.methodsNum), ec);
			if (ec) return false;

			IdentResp resp;
			boost::asio::read(*sock, boost::asio::buffer(&resp, sizeof(resp)), ec);
			if (ec) return false;

			return true;
		}


		bool socksRequest(std::shared_ptr<ip::tcp::socket>& sock, const AuthReq& req, AuthResp& resp, boost::system::error_code& ec)
		{
			memset(&resp, 0, sizeof(resp));

			uint8_t buff[256];
			uint8_t* ptr = &buff[0];
			int s = 0;
			memcpy(ptr, &req, 4); ptr += 4; s += 4;

			switch (req.addrType)
			{
			case 1:
			{
				memcpy(ptr, &req.destAddr.IPv4, 4); ptr += 4; s += 4;
				break;
			}
			case 3:
			{
				memcpy(ptr, &req.destAddr.domainLen, 1); ptr += 1; s += 1;
				memcpy(ptr, &req.destAddr.domain, req.destAddr.domainLen); ptr += req.destAddr.domainLen; s += req.destAddr.domainLen;
				break;
			}
			case 4:
			{
				memcpy(ptr, &req.destAddr.IPv6, 16); ptr += 16; s += 16;
				break;
			}

			default:
			{
				return false;
			}
			}

			unsigned short port = htons(req.destPort);

			memcpy(ptr, &port, 2); ptr += 2; s += 2;

			boost::asio::write(*sock, boost::asio::buffer(&buff[0], s), ec);
			if (ec) return false;


			boost::asio::read(*sock, boost::asio::buffer(&resp, 4), ec);
			if (ec) return false;


			switch (resp.addrType)
			{
			case 1:
			{
				boost::asio::read(*sock, boost::asio::buffer(&resp.bindAddr.IPv4, 4), ec);
				if (ec) return false;
				break;
			}
			case 3:
			{
				boost::asio::read(*sock, boost::asio::buffer(&resp.bindAddr.domainLen, 1), ec);
				if (ec) return false;
				boost::asio::read(*sock, boost::asio::buffer(&resp.bindAddr.domain, resp.bindAddr.domainLen), ec);
				if (ec) return false;
				break;
			}
			case 4:
			{
				boost::asio::read(*sock, boost::asio::buffer(&resp.bindAddr.IPv6, 16), ec);
				if (ec) return false;
				break;
			}

			default:
			{
				return false;
			}
			}

			boost::asio::read(*sock, boost::asio::buffer(&port, 2), ec);
			if (ec)
				return false;



			if (resp.reply > 0x0)
			{
				ec = make_error_code((ConnectorSocks5Errc)resp.reply);
				return false;
			}

			resp.bindPort = ntohs(port);
			return true;
		}


	private:
		std::string _adr, _adr5;
		uint16_t _port, _port5;

		std::shared_ptr<ip::tcp::socket> _sock;
	};

}


