#pragma once
#include "Packet.h"
#include <queue>
#include <map>
#include <iostream>


namespace Net
{

	class PacketHelper
	{
	public:
		bool push(const Packet& p, std::vector<uint8_t>& res);
		void clear(uint32_t id);

		template<typename Cbck>
		static void divide(uint32_t src, uint32_t dest,
			const std::vector<uint8_t>& data, Cbck cbck);
	private:
		std::map<uint32_t, std::queue<Packet>> _queue;

	};


	template<typename Cbck>
	inline void PacketHelper::divide( uint32_t src, uint32_t dest,
		const std::vector<uint8_t>& data, Cbck cbck)
	{
		if (src == dest) return;
		if (data.empty()) return;
		if (data.size() <= packet_max_size)
		{
			auto p = makePacket(data.size(), src, dest, packet_type::packet_data_unit, &data[0]);
			cbck(p, true);
			return;
		}
		uint32_t offset = 0;
		for (uint32_t i = 0;; i++)
		{
			if (offset + packet_max_size >= data.size())
			{
				auto p = makePacket((uint16_t)(packet_max_size - ((offset + packet_max_size) - data.size())),
					src, dest, packet_type::packet_data_eof, &data[offset]);
				cbck(p, true);
				break;
			}
			if (i == 0)
			{
				auto p = makePacket(packet_max_size, src, dest, packet_type::packet_data_first, &data[offset]);
				offset += packet_max_size;
				cbck(p, false);
			}
			else
			{
				auto p = makePacket(packet_max_size, src, dest, packet_type::packet_data, &data[offset]);
				offset += packet_max_size;
				cbck(p, false);
			}
		}
	}
}

