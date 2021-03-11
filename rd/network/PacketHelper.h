#pragma once
#include "packet.h"
#include "AbstractConnection.h"
#include <queue>
#include <map>
#include <iostream>




class PacketHelper
{
public:
	bool push(const packet& p, std::vector<uint8_t>& res);
	void clear(uint32_t id);

	template <typename CONN>
	static void send(CONN conn, uint32_t src, uint32_t dest,
		const std::vector<uint8_t>& data, const packet_cbck& cbck);
private:
	std::map<uint32_t, std::queue<packet>> _queue;

};


template <typename CONN>
inline void PacketHelper::send(CONN conn, uint32_t src, uint32_t dest,
	const std::vector<uint8_t>& data, const packet_cbck& cbck)
{
	if (src == dest) return;
	if (data.empty()) return;
	if (data.size() <= packet_max_size)
	{
		auto p = makePacket(data.size(), src, dest, packet_type::packet_data_unit, &data[0]);
		conn->write(std::move(p), cbck);
		return;
	}
	uint32_t offset = 0;
	for (uint32_t i = 0;; i++)
	{
		if (offset + packet_max_size >= data.size())
		{
			auto p = makePacket((uint16_t)(packet_max_size - ((offset + packet_max_size) - data.size())),
				src, dest, packet_type::packet_data_eof, &data[offset]);
			conn->write(std::move(p), cbck);
			break;
		}
		if (i == 0)
		{
			auto p = makePacket(packet_max_size, src, dest, packet_type::packet_data_first, &data[offset]);
			offset += packet_max_size;
			conn->write(std::move(p));
		}
		else
		{
			auto p = makePacket(packet_max_size, src, dest, packet_type::packet_data, &data[offset]);
			offset += packet_max_size;
			conn->write(std::move(p));
		}
	}
}


