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

namespace codec
{
	class FFDecoder : public Decoder
	{
	public:
		bool init(const VideoFormat& fmt);
		bool decode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size);
		~FFDecoder();
	private:
		const AVCodec* codec;
		AVCodecParserContext* parser;
		AVCodecContext* c = NULL;
		FILE* f;
		AVFrame* frame;

		int ret;
		AVPacket* pkt;

		struct SwsContext* sws_ctx;
	};
}

