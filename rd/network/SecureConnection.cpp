#include "SecureConnection.h"


SecureConnection::SecureConnection(ip::tcp::socket&& sock, ConnectionHandler& man, boost::asio::io_context& ctx, const std::string& secret) :
	 _ctx(ctx), _sock(std::move(sock), secret), _man(man), _packet{}, _connected(false)
{
}


void SecureConnection::write(packet&& pack, packet_cbck cbck)
{
	if (!_connected) return;
	if (!_sock.isOpen()) return;

	auto self(shared_from_this());

	auto fn = [this, self, p = std::move(pack), cbck]() {
		_writeQueue.push({ p, cbck });

		if (_writeQueue.size() == 1)
		{
			do_write();
		}
	};

	boost::asio::post(_ctx, std::move(fn));
}

void SecureConnection::write(const packet& pack, packet_cbck cbck)
{
	if (!_sock.isOpen()) return;

	auto self(shared_from_this());

	auto fn = [this, self, pack, cbck]() {
		_writeQueue.push({ pack, cbck });

		if (_writeQueue.size() == 1)
		{
			do_write();
		}
	};

	boost::asio::post(_ctx, std::move(fn));
}


boost::asio::ip::tcp::endpoint SecureConnection::address() 
{ 
	return _sock.remoteEndpoint(); 
}

void SecureConnection::startHandshake()
{
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



void SecureConnection::stop()
{
	if (_sock.isOpen())
	{
		_sock.cancel();
		_sock.close();
	}
}

void SecureConnection::start()
{
	do_handshake();
	_lastActive = utility::get_tick_count();
}


void SecureConnection::do_write()
{
	auto& b = _writeQueue.front();

	auto self(shared_from_this());

	uint8_t* ptr = reinterpret_cast<uint8_t*>(&b.first);

	_sock.asyncWrite(ptr, 16 + utility::closet_multiple(b.first.size, 16),
		[this, self](const boost::system::error_code& error, size_t bytes_transferred)
		{
			if (!error)
			{
				if (_writeQueue.front().second != nullptr) _writeQueue.front().second.operator()();

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



void SecureConnection::do_read_header()
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

void SecureConnection::do_read_data()
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


void SecureConnection::do_handshake()
{
	auto self(shared_from_this());
	_sock.asyncHandshake([this, self](const boost::system::error_code& error) {
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

