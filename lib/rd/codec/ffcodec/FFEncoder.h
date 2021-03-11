#pragma once
extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}
#include "../Codec.h"
#include "../CodecException.h"
#include <thread>
namespace codec
{
	class FFEncoder : public Encoder
	{
	public:
		~FFEncoder();
		bool init(const VideoFormat& fmt);
		bool encode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t& out_size);
	private:
		void initVp8();
		void initMpeg2();
		void initMpeg4();
		void initH264();
		void initMpeg1();
	private:
		struct SwsContext* sws_ctx;
		const AVCodec* codec;
		AVCodecContext* c = NULL;

		AVFrame* frame;
		AVPacket* pkt;

	};
}

