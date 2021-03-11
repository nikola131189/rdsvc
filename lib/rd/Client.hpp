#pragma once
#include <cstdint>
#include <string>


namespace Rd
{
	struct Client
	{
		uint32_t id = 0;
		std::string name;
		uint32_t ping = 0;
		uint64_t connectionTime = 0;
		uint32_t addressV4 = 0;
		std::array<uint8_t, 16> addressV6 = {};
	};
	//typedef Inet<bool> Inet;
}