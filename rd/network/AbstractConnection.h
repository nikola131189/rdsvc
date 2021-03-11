#pragma once
#include "packet.h"


class AbstractConnection
{
public:
	virtual void write(packet&& pack, packet_cbck cbck = nullptr) = 0;
	virtual void write(const packet& pack, packet_cbck cbck = nullptr) = 0;
	virtual boost::asio::ip::tcp::endpoint address() = 0;
	virtual void stop() = 0;
	virtual void start() = 0;
	virtual uint64_t lastActive() = 0;
	virtual ~AbstractConnection() {}
};


typedef std::shared_ptr<AbstractConnection> connection_ptr;


class ConnectionHandler
{
public:
	virtual void onStart(connection_ptr conn) = 0;
	virtual void onError(connection_ptr conn, const boost::system::error_code& ec) = 0;
	virtual void onRead(connection_ptr conn, const packet& pack) = 0;
	virtual ~ConnectionHandler() {}
};