#pragma once
#include "../rd/rd.h"
#include <map>
#include "AbstractTransfer.h"
#include "FileTransfer.h"
#include "../rd/files/FileBuffer.h"


class TransferService
{
public:
	TransferService(boost::asio::io_context& ctx);
	~TransferService();
	void post(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2);
	void get(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2);
	void remove(uint32_t id);
	void getTransfers(std::list<TransferPtr>& transfers);
private:
	void notify(const Rd::TransferEvent& ev);
	void tick();
private:
	deadline_timer _timer;
	boost::asio::io_context& _ctx;
	std::map<uint32_t, TransferPtr> _transfers;
	std::list<TransferPtr> _transfers1;
	file_system::FileBuffer _fileBuffer;
	uint32_t _id;
	std::mutex _mut;
	TransferPtr _current;
	const int timeout = 40;
	
};

