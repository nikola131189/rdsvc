#pragma once
#include <boost/asio.hpp>
#include <queue>
#include <iostream>
#include <tuple>
#include "SecureSocket.h"
#include "AbstractConnection.h"
#include "../utility.h"


struct SecureConnection : public AbstractConnection, public std::enable_shared_from_this<SecureConnection>
{
	SecureConnection(ip::tcp::socket&& sock, ConnectionHandler& man, boost::asio::io_context& ctx, const std::string& secret);
	void write(packet&& pack, packet_cbck cbck);
	void write(const packet& pack, packet_cbck cbck);
	boost::asio::ip::tcp::endpoint address();
	void startHandshake();
	void stop();
	void start();
	uint64_t lastActive() { return _lastActive; }
private:
	void do_write();
	void do_read_header();
	void do_read_data();
	void do_handshake();
private:
	boost::asio::io_context& _ctx;
	std::queue<std::pair<packet, packet_cbck>> _writeQueue;
	SecureSocket<16 + packet_max_size> _sock;
	ConnectionHandler& _man;
	packet _packet;
	bool _connected;
	uint64_t _lastActive = 0;
};




