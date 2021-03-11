#pragma once

#include "AbstractTransfer.h"
#include <algorithm>



class GetTransfer : public AbstractTransfer
{
public:
	GetTransfer(Rd::Client cl, uint32_t id_, const std::filesystem::path& p1, const std::filesystem::path& p2,
		file_system::FileBuffer& fb, TransferHandler& transferHandler);
	void start();
	void onTransfer(const Rd::TransferEvent& ev);
private:
	void sendGet();
	void onGetOk(const std::vector<uint8_t>& data, int64_t offset, bool eof);
};



class PostTransfer : public AbstractTransfer
{
public:
	PostTransfer(Rd::Client cl, uint32_t id_, const std::filesystem::path& p1, const std::filesystem::path& p2,
		file_system::FileBuffer& fb, TransferHandler& transferHandler);
	void start();
	void onTransfer(const Rd::TransferEvent& ev);
private:
	void post();
	bool _eof;
};