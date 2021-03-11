#include "Connection.h"


namespace Serv
{
	Connection::Connection(ip::tcp::socket&& sock, ConnectionHandler& man, const std::string& secret) :
		_sock(BOOST_ASIO_MOVE_CAST(ip::tcp::socket)(sock), secret), _man(man), _connected(false), _packet{}
	{
	}


	Connection::~Connection()
	{
	}


	void Connection::write(packet&& pack, packet_cbck cbck)
	{
		_writeQueue.push(std::move(pack));
		if (_writeQueue.size() == 1 && _sock.isOpen())
			do_write();
	}

	void Connection::write(const packet& pack, packet_cbck cbck)
	{
		_writeQueue.push(pack);
		if (_writeQueue.size() == 1)
			do_write();
	}


	boost::asio::ip::tcp::endpoint Connection::address()
	{
		return _sock.remoteEndpoint();
	}





	void Connection::stop()
	{
		if (_sock.isOpen())
		{
			_sock.cancel();
			_sock.close();
		}
	}

	void Connection::start()
	{
		_lastActive = utility::get_tick_count();
		auto self(shared_from_this());
		_sock.asyncStartHandshake([this, self](const boost::system::error_code& error) {
			if (!error)
			{
				_connected = true;
				_man.onStart(shared_from_this());
				do_read_header();
			}
			else
			{
				_man.onError(shared_from_this(), error);
			}
			});
	}


	void Connection::do_write()
	{
		auto& b = _writeQueue.front();

		auto self(shared_from_this());

		uint8_t* ptr = reinterpret_cast<uint8_t*>(&b);

		_sock.asyncWrite(ptr, 16 + utility::closet_multiple(b.size, 16),
			[this, self](const boost::system::error_code& error, std::size_t bytes_transferred)
			{
				if (!error)
				{

					_writeQueue.pop();
					if (!_writeQueue.empty())
					{
						do_write();
					}
				}
				else
				{
					_man.onError(shared_from_this(), error);
				}
			});
	}



	void Connection::do_read_header()
	{

		uint8_t* ptr = reinterpret_cast<uint8_t*>(&_packet);


		auto self(shared_from_this());
		_sock.asyncRead(ptr, 16,
			[this, self](size_t bytes_transferred, const boost::system::error_code& error)
			{
				if (!error)
				{
					_lastActive = utility::get_tick_count();
					if (_packet.size)
					{
						do_read_data();
					}
					else
					{
						_man.onRead(shared_from_this(), _packet);
						do_read_header();
					}

				}
				else
				{
					_man.onError(shared_from_this(), error);
				}
			});
	}

	void Connection::do_read_data()
	{
		if (_packet.size > packet_max_size)
		{
			_sock.close();
			return;
		}
		auto self(shared_from_this());
		_sock.asyncRead(_packet.buf, utility::closet_multiple(_packet.size, 16),
			[this, self](size_t bytes_transferred, const boost::system::error_code& error)
			{
				if (!error)
				{
					_man.onRead(shared_from_this(), _packet);
					do_read_header();
				}
				else
				{
					_man.onError(shared_from_this(), error);
				}
			});
	}
}


