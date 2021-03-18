#pragma once

#include <string>
#include "ServerInfo.h"
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>
#include <sstream>

class Storage
{
public:
	Storage(const std::string &db);
	void init(const std::string& secret);
	void rekey(const std::string& secret);
	void load(std::vector<ServerInfo>& arg);
	uint64_t insert(const ServerInfo& arg);
	void update(const ServerInfo& arg);
	void remove(uint64_t id);
private:
	
	std::string _secret;
	SQLite::Database _db;
};

