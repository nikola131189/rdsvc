#include "my_encoder.h"

bool codec::my_encoder::init(const VideoFormat& fmt)
{
	uint8_t q = (uint8_t)fmt.quantization; if (!q) q++;

	_fmt = fmt;

	uint32_t bs = fmt.dw * fmt.dh / 256 / 8+1;

	m_buffer.resize(sizeof(header_t) + bs + fmt.dw * fmt.dh * 3);


	uint8_t* ptr = &m_buffer[0];

	m_header = { (uint16_t)fmt.dw, (uint16_t)fmt.dh, (uint8_t)q, 0x0, bs };

	ptr += sizeof(header_t);

	m_mask = bit_mask(ptr, bs); ptr += bs;




	m_frame_buffer.resize(fmt.dw * fmt.dh * 3);

	m_frame = image_t<3>(&m_frame_buffer[0], fmt.dw, fmt.dh, fmt.dw);
	

	for (int i = 0; i < 256; i++)
		m_quant_table[i] = i / q;
	m_quant_table[0] = 0x1;

	return 1;
}


bool codec::my_encoder::encode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t& out_size)
{
	m_mask.clear(); m_fragments_count = 0;

	image_t<3> img((uint8_t *)in, _fmt.dw, _fmt.dh, _fmt.dw);

	img.transform([&](uint8_t* b) {*b = m_quant_table[*b]; });

	acquire_frame(img);
	out_size = finalize_frame(out);
	memcpy(m_frame_buffer.data(), in, m_frame_buffer.size());
	m_header.flag = 0x1;
	return 1;
}

void codec::my_encoder::acquire_frame(const image_t<3> & im)
{
	uint8_t* ptr = &m_buffer[sizeof(header_t) + m_mask.size()];

	int k = 0;

	for (int y = 0; y < _fmt.dh; y += 16)
	{
		for (int x = 0; x < _fmt.dw; x += 16)
		{
			image_t<3> im1 = m_frame.clip(x, y, 16, 16);
			image_t<3> im2 = im.clip(x, y, 16, 16);
			if (!im1.compare_sse_16(im2))
			//if (im1 != im2)
			{
				m_mask.set_bit(k);
				image_t<3> tmp(ptr, 16, 16, 16);
				tmp.fill(0, 0, im2);
				ptr += 256 * 3;
				m_fragments_count++;
			}
			k++;
		}
	}
}




int codec::my_encoder::finalize_frame(uint8_t* out)
{
	memcpy(&m_buffer[0], &m_header, sizeof(header_t));

	size_t capacity = m_frame_buffer.size();
	size_t in_size = sizeof(header_t) + m_mask.size() + 256 * 3 * m_fragments_count;
	int res = ZSTD_compress(out, capacity, &m_buffer[0], in_size, _fmt.profile);
	return res;

	/*out.resize(m_buffer.size() * 4);
	auto end = m_buffer.begin() + sizeof(header_t) + m_mask.size() + 256 * 3 * m_fragments.size();
	int res = zstd_compress({ m_buffer.begin(), end }, out, m_opt.profile);
	out.resize(res);*/
}




















bool codec::my_encoder1::init(const VideoFormat& fmt)
{
	uint8_t q = (uint8_t)fmt.quantization; if (!q) q++;
	_fmt = fmt;
	m_frame_buffer.resize(fmt.dw * fmt.dh * 3);
	m_buffer.resize(fmt.dw * fmt.dh * 4);
	m_header = { (uint16_t)fmt.dw, (uint16_t)fmt.dh, (uint8_t)q, 0x0, (uint32_t)_fmt.dh/8 };


	m_mask = bit_mask(&m_buffer[sizeof(m_header)], _fmt.dh/8);


	for (uint8_t i = 0; i < 256; i++)
		m_quant_table[i] = i / q;
	m_quant_table[0] = 0x1;
	return 1;
}

bool codec::my_encoder1::encode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t& out_size)
{
	m_fragments_count = 0;
	memset(m_buffer.data(), 0, m_buffer.size());

	image_t<3> img((uint8_t*)in, _fmt.dw, _fmt.dh, _fmt.dw);
	img.transform([&](uint8_t* b) {
		*b = m_quant_table[*b]; 
	});

	acquire_frame(img);
	out_size = finalize_frame(out);
	memcpy(m_frame_buffer.data(), in, m_frame_buffer.size());
	m_header.flag = 0x1;
	return 1;

}

void codec::my_encoder1::acquire_frame(image_t<3>& im)
{
	uint8_t* ptrRes = &m_buffer[sizeof(header_t) + m_mask.size()];
	uint8_t* ptr1 = m_frame_buffer.data();
	uint8_t* ptr2 = im.data();

	for (int y = 0; y < _fmt.dh; y++)
	{
		for (int x = 0; x < _fmt.dw*3; x++)
		{
			if (*(ptrRes) != *(ptr2))
			{
				*(ptrRes) = *(ptr2);
				m_mask.set_bit(y);
			}

			ptrRes++;
			ptr1++;
			ptr2++;
		}
		if (m_mask.get_bit(y))
		{
			m_fragments_count++;
		}
		else
		{
			ptrRes -= _fmt.dw * 3;
		}
	}
}



int codec::my_encoder1::finalize_frame(uint8_t* out)
{
	memcpy(&m_buffer[0], &m_header, sizeof(header_t));
	size_t capacity = m_frame_buffer.size();
	size_t in_size = sizeof(header_t) + m_mask.size() + m_fragments_count * _fmt.dw * 3;
	int res = ZSTD_compress(out, capacity, &m_buffer[0], in_size, _fmt.profile);
	return res;

	/*out.resize(m_buffer.size() * 4);
	auto end = m_buffer.begin() + sizeof(header_t) + m_mask.size() + 256 * 3 * m_fragments.size();
	int res = zstd_compress({ m_buffer.begin(), end }, out, m_opt.profile);
	out.resize(res);*/
}