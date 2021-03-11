#include "Session.h"

Session::Session()
{
	_imprint = utility::gen_uuid();
}

Session::Session(const Rd::Client& client, const std::string& secret, const std::string& imprint, Type t) 
	:
	_client(client), _secret(secret), _imprint(imprint)
{

}

Session::Session(const Session& other)
{
	_conn = other._conn;
	_packetHelper = other._packetHelper;
	_readBuff = other._readBuff;
	_id = other._id;
	_buffer = other._buffer;
	_imprint = other._imprint;
	_secret = other._secret;
	_client = other._client;
}


Session& Session::operator =(const Session& other)
{
	_conn = other._conn;
	_packetHelper = other._packetHelper;
	_readBuff = other._readBuff;
	_id = other._id;
	_buffer = other._buffer;
	_imprint = other._imprint;
	_secret = other._secret;
	_client = other._client;
	return *this;
}

void Session::start(boost::asio::io_context &ctx, ip::tcp::socket&& sock)
{
	_conn = std::make_shared<SecureConnection>(std::move(sock), *this, ctx, _secret);
	_conn->start();
}


void Session::stop()
{
	if(_conn)
		_conn->stop();
}


void Session::onStart(connection_ptr conn)
{
	
	Rd::HelloEvent ev;
	ev.imprint = _imprint;
	ev.client = _client;
	std::vector<uint8_t> buff;
	Rd::Serializer::encode(ev, buff);
	_conn->write(makePacket(buff.size(), _id, 0, packet_type::packet_hello, buff.data()));

	EventBus::post(Rd::ConnectionOpen());
}

void Session::onError(connection_ptr conn, const boost::system::error_code& ec)
{
	EventBus::post(Rd::ConnectionError(ec));
	//_sessionHandler.onError(*this, ec);
}

void Session::onRead(connection_ptr conn, const packet& p)
{
	if (p.type == packet_type::packet_hello)
	{
		memcpy(&_id, p.buf, 4);
		//_sessionHandler.onStart(*this);
	}

	if (p.type == packet_type::packet_ping)
	{
		_conn->write(makePacket(0, _id, 0, packet_type::packet_pong));
	}

	if (p.type == packet_type::packet_client_not_found)
	{
		uint32_t id;
		memcpy(&id, p.buf, 4);
		_packetHelper.clear(id);
		//_sessionHandler.onClientNotFound(*this, id);
	}


	if (p.type == packet_type::packet_data ||
		p.type == packet_type::packet_data_unit || 
		p.type == packet_type::packet_data_eof || 
		p.type == packet_type::packet_data_first)
	{
		if (_packetHelper.push(p, _readBuff))
		{
			Rd::Serializer::decode(_readBuff, [&](auto e) {
				e.source = p.src;
				EventBus::post(e);
			});
		}
	}
}
