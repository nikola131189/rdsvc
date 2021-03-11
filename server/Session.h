#pragma once
#include "../rd/Client.hpp"
#include <cstdint>

namespace Serv
{

	class Session
	{
	public:
		enum Type
		{
			main, second
		};
	
		Rd::Client& client() { return _client; }
		Type& type() { return _type; }
		uint64_t& lastPing() { return _lastPing; }
	private:
		Rd::Client _client;
		Type _type;
		uint64_t _lastPing = 0;
	};


}

