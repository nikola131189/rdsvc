#include "TransferService.h"

TransferService::TransferService(boost::asio::io_context& ctx)
	: _ctx(ctx),
	_id(0),
	_timer(ctx, boost::posix_time::milliseconds(timeout))
{
	EventBus::subscribe<Rd::TransferEvent>(std::bind(&TransferService::notify, this, std::placeholders::_1));
	tick();
}

TransferService::~TransferService()
{
	_timer.cancel();
}

void TransferService::post(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2)
{
	boost::asio::post(_ctx, [this, cl, p1, p2]() {
		auto t =  std::make_shared<PostTransfer>(cl, _id, p1, p2, _fileBuffer);
		_transfers[_id] = t;
		_id++;
		
		std::unique_lock<std::mutex> lk(_mut);
		_transfers1.push_back(t);
		});	

}

void TransferService::get(const Rd::Client& cl, const std::filesystem::path& p1, const std::filesystem::path& p2)
{
	boost::asio::post(_ctx, [this, cl, p1, p2]() {
		auto t = std::make_shared<GetTransfer>(cl, _id, p1, p2, _fileBuffer);
		_transfers[_id] = t;
		_id++;

		std::unique_lock<std::mutex> lk(_mut);
		_transfers1.push_back(t);
		
	});
}

void TransferService::remove(uint32_t id)
{
	boost::asio::post(_ctx, [this, id]() {	
		if (_current)
		{
			if (_current->id() == id)
				_current = nullptr;
		}
		_transfers.erase(id);
		std::unique_lock<std::mutex> lk(_mut);
		_transfers1.erase(std::find_if(std::begin(_transfers1), std::end(_transfers1),
			[id](const TransferPtr& t) { return t->id() == id; }));
	});
}

void TransferService::getTransfers(std::list<TransferPtr>& transfers)
{
	std::unique_lock<std::mutex> lk(_mut);
	transfers = _transfers1;
}


void TransferService::notify(const Rd::TransferEvent& ev)
{
	auto it = _transfers.find(ev.id);
	if (it == _transfers.end()) return;
	it->second->onTransfer(ev);
}

void TransferService::tick()
{
	if (_current == nullptr)
	{
		for (auto it = _transfers.begin(); it != _transfers.end(); it++)
		{
			auto f = it->second;
			if (f->state() == AbstractTransfer::ready)
			{
				_current = f;
				_current->start();
				break;
			}
		}
	}
	else
	{
		AbstractTransfer::State s = _current->state();
		if (s == AbstractTransfer::success)
		{
			_current = nullptr;
		}

		if (s == AbstractTransfer::running || s == AbstractTransfer::error)
		{
			if (utility::get_tick_count() - _current->lastActive() > 5000)
			{
				_current->start();
			}
		}
	}

	_timer = deadline_timer(_ctx, boost::posix_time::milliseconds(timeout));
	_timer.async_wait(std::bind(&TransferService::tick, this));
}

