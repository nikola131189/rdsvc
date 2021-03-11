#pragma once

#include "net/ClientSessionHandler.h"
#include "net/Session.h"
#include "net/ConnectorTcp.h"
#include "events/events.h"
#include "Client.hpp"
#include "events/Serializer.hpp"
#include "net/Connection.h"

#include "net/ConnectorSocks5.hpp"

#include "Log.h"
#include <array>
#include "utility.h"

#include "Client.hpp"
#include "EventBus.hpp"
#include "Native.h"
#include "ConfigReader.hpp"
#include "files/file_system.h"
#include "files/FileBuffer.h"
#include "keyboard/ScanCodeTranslator.h"

namespace Rd
{

	class Inet
	{
	public:

		template <typename T>
		static void send(uint32_t dest, const T& t, const Net::packet_cbck& cbck = Net::packet_cbck());

		static Net::ClientSessionHandler& sess1();
		static Net::ClientSessionHandler& sess2();
		static void init(const std::string& name, const std::string& imprint);
	private:
		static Inet& inst();
		Net::ClientSessionHandler _session1, _session2;
	};




	inline Inet& Inet::inst()
	{
		static Inet _inst;
		return _inst;
	}



	inline Net::ClientSessionHandler& Inet::sess1()
	{
		return inst()._session1;
	}

	inline Net::ClientSessionHandler& Inet::sess2()
	{
		return inst()._session2;
	}


	inline void Inet::init(const std::string& name, const std::string& imprint)
	{
		inst()._session1 = Net::ClientSessionHandler(name, imprint, 0);
		inst()._session2 = Net::ClientSessionHandler(name, imprint, 1);
	}




	template <typename T>
	inline void Inet::send(uint32_t dest, const T& t, const Net::packet_cbck& cbck)
	{
		if constexpr (std::is_same_v<T, Rd::ActionEvent>)
		{
			inst()._session2.write(dest, t, cbck);
			return;
		}

		if constexpr (std::is_same_v<T, Rd::ClipboardEvent>)
		{
			inst()._session2.write(dest, t, cbck);
			return;
		}


		if constexpr (std::is_same_v<T, Rd::VideoEvent>)
		{
			inst()._session2.write(dest, t, cbck);
			return;
		}
		inst()._session1.write(dest, t, cbck);
	}
}