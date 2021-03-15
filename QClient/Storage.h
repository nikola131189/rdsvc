#pragma once
#include <sqlite_modern_cpp.h>
#include <string>
#include "ServerInfo.h"
#include <vector>


class Storage
{
public:
	Storage();
	void load(std::vector<ServerInfo>& arg);
	uint64_t insert(const ServerInfo& arg);
	void update(const ServerInfo& arg);
	void remove(uint64_t id);
private:
	sqlite::database _db;
};

