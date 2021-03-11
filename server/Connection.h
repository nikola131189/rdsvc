#pragma once
#include <boost/asio.hpp>
#include <queue>
#include <iostream>
#include <tuple>
#include "../rd/network/SecureSocket.h"
#include "../rd/utility.h"
#include "../rd/network/packet.h"
#include "Session.h"

namespace Serv
{
	class ConnectionHandler;

	class Connection : public Session, public std::enable_shared_from_this<Connection>
	{
	public:
		Connection(ip::tcp::socket&& sock, ConnectionHandler& man, const std::string& secret);
		~Connection();
		void write(packet&& pack, packet_cbck cbck = nullptr);
		void write(const packet& pack, packet_cbck cbck = nullptr);
		boost::asio::ip::tcp::endpoint address();
		void stop();
		void start();
		uint64_t lastActive() { return _lastActive; }
	private:
		void do_write();
		void do_read_header();
		void do_read_data();
	private:

		std::queue<packet> _writeQueue;
		SecureSocket<16 + packet_max_size> _sock;
		ConnectionHandler& _man;

		packet _packet;

		bool _connected;

		uint64_t _lastActive = 0;
	};

	typedef std::shared_ptr<Connection> connection_ptr;


	class ConnectionHandler
	{
	public:
		virtual void onStart(connection_ptr conn) = 0;
		virtual void onError(connection_ptr conn, const boost::system::error_code& ec) = 0;
		virtual void onRead(connection_ptr conn, const packet& pack) = 0;
		virtual ~ConnectionHandler(){}
	};

}