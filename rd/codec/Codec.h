#pragma once
#include <vector>
#include <cmath>

namespace codec
{
	struct VideoFormat
	{
		enum CodecId
		{
			VP8,
			H264,
			MPEG2,
			MPEG1,
			MPEG4,
			ZSTD
		};
		CodecId id;
		int64_t bitarte;
		int w, h, dw, dh, profile, quantization, threadCount;
	};




	constexpr bool operator == (const VideoFormat& c1, const VideoFormat& c2)
	{
		if (c1.id == c2.id && c1.bitarte == c2.bitarte && c1.w == c2.w && c1.h == c2.h
			&& c1.profile == c2.profile && c1.quantization == c2.quantization 
			&& c1.threadCount == c2.threadCount)
			return true;
		return false;
	}


	constexpr bool operator != (const VideoFormat& c1, const VideoFormat& c2)
	{
		if (c1.id == c2.id && c1.bitarte == c2.bitarte && c1.w == c2.w && c1.h == c2.h
			&& c1.profile == c2.profile && c1.quantization == c2.quantization
			&& c1.threadCount == c2.threadCount)
			return false;
		return true;
	}


	struct Encoder
	{
		virtual bool init(const VideoFormat& fmt) = 0;
		virtual bool encode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t& out_size) = 0;
		virtual ~Encoder() {}
	};

	struct Decoder
	{
		virtual bool init(const VideoFormat& fmt) = 0;
		virtual bool decode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size) = 0;
		virtual ~Decoder() {}
	};


	constexpr int closet_multiple(int val, int q)
	{
		return (val % q) ? closet_multiple(val + 1, q) : val;
	}
}



