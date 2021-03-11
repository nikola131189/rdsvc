#include "PacketHelper.h"


bool PacketHelper::push(const packet& p, std::vector<uint8_t>& res)
{
	if (p.type == packet_type::packet_data_first)
	{
		_queue[p.src] = std::queue<packet>();
		_queue[p.src].push(p);
		return false;
	}

	if (p.type == packet_type::packet_data_unit)
	{
		_queue[p.src] = std::queue<packet>();
		res.resize(p.size);
		uint8_t* ptr = res.data();
		memcpy(ptr, p.buf, p.size);
		return true;
	}


	if (p.type == packet_type::packet_data)
	{
		_queue[p.src].push(p);
		return false;
	}

	if (p.type == packet_type::packet_data_eof)
	{
		_queue[p.src].push(p);
		res.resize((_queue[p.src].size() - 1) * packet_max_size + p.size);
		uint8_t* ptr = res.data();
		while (!_queue[p.src].empty())
		{
			memcpy(ptr, _queue[p.src].front().buf, _queue[p.src].front().size);
			ptr += _queue[p.src].front().size;
			_queue[p.src].pop();
		}
		_queue[p.src] = std::queue<packet>();
		return true;
	}
	return false;
}

void PacketHelper::clear(uint32_t id)
{
	_queue[id] = std::queue<packet>();
}



