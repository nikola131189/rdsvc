#pragma once

#include "Packet.h"

#include "PacketHelper.h"
#include "../events/events.h"
#include "../Client.hpp"
#include "../events/Serializer.hpp"

namespace Net
{
	class Session
	{
	public:

		Session() {}
		virtual ~Session() {}

		uint32_t& id() { return _id; }

		int& type() { return _type; }

		int& lastPing() { return _lastPing; }

		Rd::Client& client() { return _client; }


		virtual void write(const Packet& pack) = 0;
		virtual void write(Packet&& pack) = 0;
		virtual uint64_t lastActive() = 0;
		virtual boost::asio::any_io_executor get_executor() = 0;
		virtual boost::asio::ip::tcp::endpoint remote_endpoint() = 0;
		virtual void stop() = 0;
	private:
		uint32_t _id = 0;
		int _type = 0;
		int _lastPing = 0;
		Rd::Client _client;
	};


	typedef std::shared_ptr<Session> session_ptr;



	class SessionHandler
	{
	public:
		virtual void onStart(session_ptr sess) = 0;
		virtual void onError(session_ptr sess, std::string msg) = 0;
		virtual void onRead(session_ptr sess, Packet&& pack) = 0;
		virtual ~SessionHandler() {}
	};

}