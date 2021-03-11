#include "application.h"


namespace Net
{

	Application::Application() 
	{
	}


	void Application::onStart(session_ptr sess)
	{

	}

	void Application::onError(session_ptr sess, std::string msg)
	{
		//std::cout << "onError: " << msg << std::endl;
		notifyDisconnected(sess->id());
		printSessions();
	}

	void Application::onRead(session_ptr sess, Packet&& pack)
	{
		if (pack.dest == 0)
		{
			if (pack.type == packet_type::packet_hello) {
				onHello(sess, std::move(pack));
				return;
			}

			if (pack.type == packet_type::packet_pong) {
				onPong(sess);
				return;
			}

			if ((uint8_t)pack.type > 0x4)
			{
				if (_packetHelper.push(pack, _buff))
				{
					Rd::Serializer::decode(_buff, [&](auto e) {
						onData(sess, e);
						});
				}
			}
		}
		else
		{
			std::unordered_map<uint32_t, session_ptr>* container = &_sessions1;
			if (sess->type() == 1)
				container = &_sessions2;


			auto it = container->find(pack.dest);
			if (it == container->end())
			{
				notifyDisconnected(pack.dest);
			}
			else
			{
				it->second->write(std::move(pack));
			}
		}
	}


	awaitable<void> Application::pingLoop(int connectionTimeout)
	{
		auto executor = co_await this_coro::executor;
		boost::asio::steady_timer timer(executor);
		while (true)
		{		
			boost::system::error_code ec;
			timer.expires_after(std::chrono::milliseconds(2000));
			co_await timer.async_wait(redirect_error(use_awaitable, ec));

			printSessions();
			auto t = utility::get_tick_count();
			for (auto it = _sessions1.begin(); it != _sessions1.end();)
			{
				if (!it->second->lastPing() > 0)
					sendPing(it->second);

				auto v = abs((int)(t - it->second->lastActive()));

				if (v > connectionTimeout)
				{
					it->second->stop();
					it = _sessions1.erase(it);
				}
				else
					it++;
			}


			for (auto it = _sessions2.begin(); it != _sessions2.end();)
			{
				if (!it->second->lastPing() > 0)
					sendPing(it->second);

				auto v = abs((int)(t - it->second->lastActive()));

				if (v > connectionTimeout)
				{
					it->second->stop();
					it = _sessions2.erase(it);
				}
				else
					it++;
			}

		}
	}



	void Application::sendPing(session_ptr sess)
	{
		sess->lastPing() = utility::get_tick_count();
		sess->write(makePacket(0, 0, sess->id(), packet_type::packet_ping));
	}


	void Application::onPong(session_ptr sess)
	{
		sess->client().ping = utility::get_tick_count() - sess->lastPing();
		sess->lastPing() = 0;
	}



	void Application::notifyDisconnected(uint32_t id)
	{
		for (auto it = _sessions1.begin(); it != _sessions1.end(); it++)
		{
			uint32_t dest = it->second->id();
			it->second->write(makePacket(sizeof(id), 0, dest, packet_type::packet_client_not_found, &id));
		}

		for (auto it = _sessions2.begin(); it != _sessions2.end(); it++)
		{
			uint32_t dest = it->second->id();
			it->second->write(makePacket(sizeof(id), 0, dest, packet_type::packet_client_not_found, &id));
		}
	}

	void Application::onHello(session_ptr sess, Packet&& pack)
	{

		_buff.resize(pack.size);
		memcpy(_buff.data(), pack.buf, pack.size);

		Rd::Serializer::decode(_buff, [&](auto e) {

			if constexpr (std::is_same_v<decltype(e), Rd::HelloEvent>)
			{
				uint32_t id = _imprintStore.genId(e.imprint);
				sess->client() = e.client;
				sess->client().connectionTime = std::time(0);
				sess->client().addressV4 = sess->remote_endpoint().address().to_v4().to_ulong();
				sess->client().id = id;
				sess->id() = id;
				sess->type() = e.connectionType;
				std::cout << e.client.name << " " << e.connectionType << std::endl;
				if (e.connectionType == 0)
					_sessions1[id] = sess;
					
				if (e.connectionType == 1)
					_sessions2[id] = sess;

				sess->write(makePacket(4, 0, id, packet_type::packet_hello, &id));
				//std::cout << "connection start id: " << e.imprint << std::endl;
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				printSessions();
				sendPing(sess);
			}
		});

	}

	void Application::printSessions()
	{
		//return;
		system("cls");
		for (auto it = _sessions1.begin(); it != _sessions1.end(); it++)
		{
			auto conn = it->second;
			std::cout << conn->client().id << "\t " << conn->client().ping   << "\t " << conn->remote_endpoint().address().to_string() <<
				"\t " << conn->client().name << "\n";

		}

		std::cout << "\n";

		for (auto it = _sessions2.begin(); it != _sessions2.end(); it++)
		{
			auto conn = it->second;
			std::cout << conn->client().id << "\t " << conn->client().ping << "\t " << conn->remote_endpoint().address().to_string() <<
				"\t " << conn->client().name << "\n";

		}
	}

}