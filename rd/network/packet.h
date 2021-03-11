#pragma once
#include <boost/asio.hpp>


enum class packet_type : uint8_t
{
	packet_client_not_found = 0x1,
	packet_ping = 0x2,
	packet_pong = 0x3,
	packet_data = 0x4,
	packet_hello = 0x5,
	packet_data_first = 0x6,
	packet_data_eof = 0x7,
	packet_data_unit = 0x8
};

#pragma pack(push, 1)


typedef std::function<void()> packet_cbck;
const int packet_max_size = 1024 * 5;
struct packet
{
	uint16_t size;
	uint32_t src, dest;
	packet_type type;
	uint32_t reserved;
	uint8_t flags;
	uint8_t buf[packet_max_size];
};


static packet makePacket(uint16_t size, uint32_t src, uint32_t dest, packet_type type, const void *data = 0)
{
	packet p;
	p.size = size;
	p.src = src;
	p.dest = dest;
	p.type = type;
	if(data)
		memcpy(p.buf, data, size);
	return std::move(p);
}

#pragma pack(pop)


