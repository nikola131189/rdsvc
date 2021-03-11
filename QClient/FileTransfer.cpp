#include "FileTransfer.h"


GetTransfer::GetTransfer(Rd::Client cl, uint32_t id_, const std::filesystem::path& p1, const std::filesystem::path& p2,
	file_system::FileBuffer& fb, TransferHandler& transferHandler)
    : AbstractTransfer(cl, id_, p1, p2, fb, transferHandler)
{}



void GetTransfer::start()
{
	_lastActive = utility::get_tick_count();
	_state = AbstractTransfer::running;
	sendGet();
	
}


void GetTransfer::onTransfer(const Rd::TransferEvent& ev)
{
	if (_state == AbstractTransfer::error)
		return;
	_lastActive = utility::get_tick_count();


	if (auto pval = std::get_if<Rd::TransferEvent::Error>(&ev.val))
	{
		if (_state != AbstractTransfer::error)
		{
			_transferHandler.onError(shared_from_this());
			_state = AbstractTransfer::error;
			_ec.value = pval->value;
			_ec.message = pval->message;
			_offset = pval->offset;
		}
	}

	if (auto pval = std::get_if<Rd::TransferEvent::GetOk>(&ev.val))
	{
		onGetOk(pval->data, pval->offset, pval->eof);
	}
}

void GetTransfer::sendGet()
{
	if (_state == AbstractTransfer::error)
		return;
	Rd::TransferEvent ev;
	ev.id = _id;
	Rd::TransferEvent::Get v;

	v.path = _path2;
	v.size = _buffer.size();
	v.offset = _offset;
	ev.val = v;
	Rd::Inet::send(_client.id, ev);
}

void GetTransfer::onGetOk(const std::vector<uint8_t>& data, int64_t offset, bool eof)
{
	_offset = offset + data.size();
	if (eof)
	{
		_transferHandler.onSuccess(shared_from_this());
		_state = AbstractTransfer::success;
	}
	else
	{	
		
		sendGet();
		
	}
	_fileBuffer.write(_path1, offset, data, [this, offset](int64_t size, const std::error_code& ec) {
		if (ec)
		{
			if (_state != AbstractTransfer::error)
			{
				_transferHandler.onError(shared_from_this());
				_state = AbstractTransfer::error;
				_ec.message = utility::localToU8(ec.message());
				_ec.value = ec.value();
				_offset = offset;
			}
		}
		else
		{
			
		}
	});	
}








PostTransfer::PostTransfer(Rd::Client cl, uint32_t id_, const std::filesystem::path& p1, const std::filesystem::path& p2,
	file_system::FileBuffer& fb, TransferHandler& transferHandler)
	: AbstractTransfer(cl, id_, p1, p2, fb, transferHandler), _eof(0)
{
}



void PostTransfer::start()
{
	_lastActive = utility::get_tick_count();
	_state = AbstractTransfer::running;
	post();

}


void PostTransfer::onTransfer(const Rd::TransferEvent& ev)
{
	if (_state == AbstractTransfer::error)
		return;

	_lastActive = utility::get_tick_count();

	if (auto pval = std::get_if<Rd::TransferEvent::PostOk>(&ev.val))
	{
		_offset = pval->offset + pval->size;
		if (_eof)
		{
			_transferHandler.onSuccess(shared_from_this());
			_state = success;
		}
		else
			post();
	}

	if (auto pval = std::get_if<Rd::TransferEvent::Error>(&ev.val))
	{
		if (_state != AbstractTransfer::error)
		{
			_transferHandler.onError(shared_from_this());
			_state = AbstractTransfer::error;
			_ec.value = pval->value;
			_ec.message = pval->message;
			_offset = pval->offset;
		}
	}
}

void PostTransfer::post()
{
	_fileBuffer.read(_path1, _offset, &_buffer[0], _buffer.size(),
		[this](int64_t size, bool eof, const std::error_code& ec) {

			_eof = eof;

			if (ec)
			{
				if (_state != AbstractTransfer::error)
				{
					_ec.message = utility::localToU8(ec.message());
					_ec.value = ec.value();
					_state = AbstractTransfer::error;
					_transferHandler.onError(shared_from_this());
				}
				return;
			}


			Rd::TransferEvent ev;
			ev.id = _id;
			Rd::TransferEvent::Post v;
			v.data = std::vector<uint8_t>(_buffer.begin(), _buffer.begin() + size);
			v.path = _path2;
			v.offset = _offset;
			ev.val = v;
			Rd::Inet::send(_client.id, ev);
		}); 
}


