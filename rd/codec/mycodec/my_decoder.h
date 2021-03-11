#pragma once
#include <vector>
#include "bit_mask.h"
#include "util.h"
#include <string>
#include "../Codec.h"
namespace codec
{
	class my_decoder : public Decoder
	{
	public:
		bool init(const VideoFormat& fmt);
		bool decode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size);

	private:
		std::vector<uint8_t> m_buffer, m_buffer1;
		header_t m_header;

		bit_mask m_mask;
		image_t<3> m_frame;
		int m_width, m_height, m_fragment_count;


		void acquire_frame(uint8_t* ptr);

	};


	class my_decoder1 : public Decoder
	{
	public:
		bool init(const VideoFormat& fmt);
		bool decode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size);

	private:
		std::vector<uint8_t> m_buffer, m_buffer1;
		header_t m_header;

		bit_mask m_mask;
		image_t<3> m_frame;
		int m_width, m_height, m_fragment_count;


		void acquire_frame(uint8_t* ptr);

	};
}
