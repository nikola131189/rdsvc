#pragma once
#include <filesystem>
#include "../rd/rd.h"

struct TransferPostEvent
{
	TransferPostEvent(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2)
		:
		client(cl), path1(p1), path2(p2)
	{}
	Rd::Client client;
	std::filesystem::path path1, path2;
};



struct TransferGetEvent
{
	TransferGetEvent(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2)
		:
		client(cl), path1(p1), path2(p2)
	{}
	Rd::Client client;
	std::filesystem::path path1, path2;
};