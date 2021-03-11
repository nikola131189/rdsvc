#include "ClientSessionHandler.h"


namespace Net
{

	ClientSessionHandler::ClientSessionHandler()
	{
	}

	ClientSessionHandler::ClientSessionHandler(const std::string& name, const std::string& imprint, int connectionType)
	{
		_helloEvent.client.name = name;
		_helloEvent.imprint = imprint;
		_helloEvent.connectionType = connectionType;
		_writeBuff.reserve(1024 * 1024 * 5);
	}


	ClientSessionHandler::~ClientSessionHandler()
	{}

	ClientSessionHandler& ClientSessionHandler::operator=(const ClientSessionHandler & other)
	{
		_sess = other._sess;
		_packetHelper = other._packetHelper;
		_helloEvent = other._helloEvent;
		return *this;
	}

	void ClientSessionHandler::onStart(session_ptr sess)
	{	
		_sess = sess;
		_sess->type() = _helloEvent.connectionType;
	
		std::vector<uint8_t> buff;
		Rd::Serializer::encode(_helloEvent, buff);

		sess->write(makePacket(buff.size(), 0, 0, packet_type::packet_hello, buff.data()));
	}


	void ClientSessionHandler::onError(session_ptr sess, std::string msg) {
		EventBus::post(Rd::ConnectionError(msg));
	}


	void ClientSessionHandler::onRead(session_ptr sess, Packet&& pack) {
		if (pack.type == packet_type::packet_hello)
		{
			memcpy(&sess->id(), pack.buf, 4);
			EventBus::post(Rd::ConnectionOpen());
		}

		if (pack.type == packet_type::packet_ping)
		{
			sess->write(makePacket(0, sess->id(), 0, packet_type::packet_pong));
		}

		if (pack.type == packet_type::packet_client_not_found)
		{
			uint32_t id_ = 0;
			memcpy(&id_, pack.buf, 4);
			EventBus::post(Rd::ClientNotFound(id_));
		}

		if ((uint8_t)pack.type > 0x4)
		{
			if (_packetHelper.push(pack, _readBuff))
			{
				Rd::Serializer::decode(_readBuff, [&](auto e) {
					e.source = pack.src;
					EventBus::post(e);
					}
				);
			}
		}
	}


}