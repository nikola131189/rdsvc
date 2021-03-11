#pragma once

#include "AbstractConnection.h"
#include "PacketHelper.h"
#include <mutex>
#include "SecureConnection.h"
#include "../utility.h"
#include "../EventBus.hpp"
#include "../events/Serializer.hpp"
#include "Connector.hpp"


class Session : public ConnectionHandler
{
public:

	enum Type
	{
		main,
		second
	};


	Session();
	Session(const Rd::Client& client, const std::string& secret, const std::string& imprint, Type t);
	Session(const Session& other);
	Session& operator =(const Session& other);

	template <typename T>
	void write(uint32_t dest, const T& data, packet_cbck cbck)
	{
		if (!_conn || !_id) return;
		std::unique_lock<std::mutex> lk(_mut);
		if (Rd::Serializer::encode(data, _buffer))
		{
			PacketHelper::send(_conn, _id, dest, _buffer, cbck);
		}	
	}

	void start(boost::asio::io_context& ctx, ip::tcp::socket&& sock);
	void stop();
	uint64_t lastActive() {
		if(_conn)
			return _conn->lastActive(); 
		return 0;
	}

	

	~Session() {}	
private:
	void onStart(connection_ptr conn);
	void onError(connection_ptr conn, const boost::system::error_code& ec);
	void onRead(connection_ptr conn, const packet& pack);
private:
	connection_ptr _conn;

	PacketHelper _packetHelper;

	std::vector<uint8_t> _readBuff;

	uint32_t _id = 0;

	mutable std::mutex _mut;

	std::vector<uint8_t> _buffer;

	std::string _imprint, _secret;
	Rd::Client _client;
};


