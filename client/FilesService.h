#pragma once
#include <rd/rd.h>


class FilesService
{
public:
	FilesService(boost::asio::io_context& ctx);
	void notify(const Rd::FilesEvent& e);
private:
	boost::asio::io_context& _ctx;
	bool checkError(Rd::FilesEvent& ev, std::error_code ec);
};

