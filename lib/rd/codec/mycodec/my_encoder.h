#pragma once
#include <vector>
#include <list>
#include "bit_mask.h"
#include "util.h"
#include <string>
#include "../Codec.h"


namespace codec
{
	class my_encoder : public Encoder
	{
	public:
		bool init(const VideoFormat& fmt);
		bool encode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t& out_size);

	private:
		::std::vector<uint8_t> m_buffer, m_frame_buffer;
		bit_mask m_mask;
		image_t<3> m_frame;
		uint32_t m_fragments_count = 0;
		VideoFormat _fmt;

		::std::array<uint8_t, 256> m_quant_table;

		header_t m_header;

		void acquire_frame(const image_t<3> & im);
		int finalize_frame(uint8_t* out);
	};


	class my_encoder1 : public Encoder
	{
	public:
		bool init(const VideoFormat& fmt);
		bool encode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t& out_size);

	private:
		::std::vector<uint8_t> m_buffer, m_frame_buffer;
		bit_mask m_mask;
		image_t<3> m_frame;
		uint32_t m_fragments_count = 0;
		VideoFormat _fmt;

		::std::array<uint8_t, 256> m_quant_table;

		header_t m_header;

		void acquire_frame(image_t<3>& im);
		int finalize_frame(uint8_t* out);
	};
}