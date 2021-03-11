#include "my_decoder.h"


void codec::my_decoder::acquire_frame(uint8_t* ptr)
{
	int x = 0; int y = 0; int k = 0;
	for (size_t i = 0; i < m_mask.size_bit(); i++)
	{
		if (m_mask.get_bit(i))
		{
			image_t<3> im(ptr + k * 256 * 3, 16, 16, 16);
			im.transform([&](uint8_t* b) {
				if(*b > 1)
					*b *= m_header.q; 
			});
			m_frame.fill(x, y, im);
			k++;
		}
		x += 16;
		if (x == m_width)
		{
			x = 0;
			y += 16;
		}
	}

}


bool codec::my_decoder::decode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
{

	int s = zstd_uncompress(in, in_size, &m_buffer1[0], m_buffer1.size());
	//DBOUT(s);
	if (!s) return 0;
	//memset(&m_buffer[0], 0, m_buffer.size());
	uint8_t* ptr = (uint8_t*)& m_buffer1[0];

	memcpy(&m_header, ptr, sizeof(m_header)); ptr += sizeof(m_header);

	if (m_header.flag == 0x0)
		memset(&m_buffer[0], 0, m_buffer.size());


	m_mask = bit_mask(ptr, m_header.bm_size); ptr += m_header.bm_size;

	acquire_frame(ptr);

	memcpy(out, &m_buffer[0], out_size);

	return 1;
}


bool codec::my_decoder::init(const VideoFormat& fmt)
{
	m_width = fmt.dw; m_height = fmt.dh;
	m_buffer.resize(m_width * m_height * 3);
	m_buffer1.resize(m_width * m_height * 3 * 2);
	m_frame = image_t<3>((uint8_t*)& m_buffer[0], m_width, m_height, m_width);
	return 1;
}














bool codec::my_decoder1::init(const VideoFormat& fmt)
{
	m_width = fmt.dw; m_height = fmt.dh;
	m_buffer.resize(m_width * m_height * 3);
	m_buffer1.resize(m_width * m_height * 3 * 2);
	m_frame = image_t<3>((uint8_t*)&m_buffer[0], m_width, m_height, m_width);
	return 1;
}

bool codec::my_decoder1::decode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
{
	int s = zstd_uncompress(in, in_size, &m_buffer1[0], m_buffer1.size());
	//DBOUT(s);
	if (!s) return 0;
	//memset(&m_buffer[0], 0, m_buffer.size());
	uint8_t* ptr = (uint8_t*)&m_buffer1[0];

	memcpy(&m_header, ptr, sizeof(m_header)); ptr += sizeof(m_header);

	if (m_header.flag == 0x0)
		memset(&m_buffer[0], 0, m_buffer.size());


	m_mask = bit_mask(ptr, m_header.bm_size); ptr += m_header.bm_size;


	acquire_frame(ptr);

	memcpy(out, &m_buffer[0], out_size);

	return 1;
}

void codec::my_decoder1::acquire_frame(uint8_t* ptr)
{
	uint8_t* dest = &m_buffer[0];
	int x = 0; int y = 0; int k = 0;
	for (size_t i = 0; i < m_mask.size_bit(); i++)
	{
		if (m_mask.get_bit(i))
		{
			for (uint16_t j = 0; j < m_header.w * 3; j++)
			{
				if (*ptr != 0x0)
				{
					*dest = *ptr * m_header.q;
				}
				dest++;
				ptr++;
			}
		}
		else
		{
			dest += m_header.w * 3;
		}

	}

}
