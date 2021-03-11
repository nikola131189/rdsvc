#pragma once
#include <rd/rd.h>


class TransferService
{
public:
	TransferService(boost::asio::io_context& ctx);

	void notify(const Rd::TransferEvent& ev);
private:
	void handleGet(const std::filesystem::path& path, uint32_t src, uint32_t id,
		int64_t offset, int64_t size);
	void handlePost(const std::filesystem::path& path, uint32_t src, uint32_t id,
		int64_t offset, const std::vector<uint8_t>& data);

	void sendError(uint32_t src, uint32_t id, int64_t offset, std::error_code ec);
private:
	file_system::FileBuffer _fileBuffer;
	std::vector<uint8_t>  _buffer;
	boost::asio::io_context& _ctx;
};

