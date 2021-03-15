#pragma once
#include <string>
#include <cstdint>

struct ServerInfo
{
	uint64_t id = 0;
	std::string description, connectionType, address1, address2;
	int port1 = 0, port2 = 0;
	std::string imprint, secret;
};
