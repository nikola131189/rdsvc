#include "TransferService.h"

TransferService::TransferService(boost::asio::io_context& ctx)
	:
	_ctx(ctx)
{
	_buffer.resize(10 * 1024 * 1024);
}



void TransferService::notify(const Rd::TransferEvent& ev)
{
	boost::asio::post([ev, this]()
		{

			if (auto pval = std::get_if<Rd::TransferEvent::Get>(&ev.val))
			{
				handleGet(pval->path, ev.source, ev.id, pval->offset, pval->size);
			}


			if (auto pval = std::get_if<Rd::TransferEvent::Post>(&ev.val))
			{
				handlePost(pval->path, ev.source, ev.id, pval->offset, pval->data);
			}
		}
	);
}




void TransferService::handleGet(const std::filesystem::path& path, uint32_t src, uint32_t id,
	int64_t offset, int64_t size)
{
	_fileBuffer.read(path, offset, &_buffer[0], size,
		[this, path, src, id, offset](int64_t size, bool eof, const std::error_code& ec) {
			if (ec)
			{
				sendError(src, id, offset, ec);
				return;
			}
			Rd::TransferEvent::GetOk val;
			val.eof = eof;
			val.offset = offset;
			val.data = std::vector<uint8_t>(_buffer.begin(), _buffer.begin() + size);

			Rd::TransferEvent ev;
			ev.id = id;
			ev.val = val;

			Rd::Inet::send(src, ev);	
		});
}

void TransferService::handlePost(const std::filesystem::path& path, uint32_t src, uint32_t id, int64_t offset, const std::vector<uint8_t>& data)
{
	int64_t offs = offset;
	_fileBuffer.write(path, offset, data, [this, src, id, offs](int64_t size, const std::error_code& ec) {
		if (ec)
			sendError(src, id, offs, ec);
	});


	Rd::TransferEvent::PostOk val;
	val.offset = offset;
	val.size = data.size();

	Rd::TransferEvent ev;
	ev.id = id;
	ev.val = val;
	Rd::Inet::send(src, ev);
}




void TransferService::sendError(uint32_t src, uint32_t id, int64_t offset, std::error_code ec)
{
	Rd::TransferEvent::Error val;
	val.message = utility::localToU8(ec.message());
	val.value = ec.value();
	val.offset = offset;

	Rd::TransferEvent ev;
	ev.id = id;
	ev.val = val;
	Rd::Inet::send(src, ev);
}