#pragma once

#include "network/Session.h"
#include <boost/asio/signal_set.hpp>

namespace Rd
{
	class Inet
	{
	public:

		template <typename T>
		static void send(uint32_t dest, const T& t, const packet_cbck& cbck = packet_cbck());

		static Session& sess1();
		static Session& sess2();

		static void init(const Rd::Client& client, const std::string& secret, const std::string& imprint);
	private:
		static Inet& inst();
		Session _session1, _session2;
	};




	inline Inet& Inet::inst()
	{
		static Inet _inst;
		return _inst;
	}

	inline void Inet::init(const Rd::Client& client, const std::string& secret, const std::string& imprint)
	{
		inst()._session1 = Session(client, secret, imprint, Session::Type::main);
		inst()._session2 = Session(client, secret, imprint, Session::Type::second);
	}


	inline Session& Inet::sess1()
	{
		return inst()._session1;
	}

	inline Session& Inet::sess2()
	{
		return inst()._session2;
	}


	template <typename T>
	inline void Inet::send(uint32_t dest, const T& t, const packet_cbck& cbck)
	{
		std::vector<uint8_t> buff;


			auto &sess = inst()._session1;
			/*if constexpr (std::is_same_v<T, Rd::ActionEvent>)
			{
				sess = inst()._session2;
			}

			if constexpr (std::is_same_v<T, Rd::ClipboardEvent>)
			{
				sess = inst()._session2;
			}


			if constexpr (std::is_same_v<T, Rd::VideoEvent>)
			{
				sess = inst()._session2;
			}*/
			sess.write(dest, t, cbck);
	}

	/*
	template<typename Serializer>
	inline void Inet<Serializer>::run(uint32_t time)
	{

		if (inst()._session1)
		{
			uint64_t dist = utility::get_tick_count() - inst()._session1->lastActive();

			if (dist > time)
			{
				inst()._session1->close();
				inst()._session1->open();
			}
		}

		if (inst()._session2)
		{
			uint64_t dist = utility::get_tick_count() - inst()._session2->lastActive();

			if (dist > time)
			{
				inst()._session2->close();
				inst()._session2->open();
			}
		}


	}
		*/
}

