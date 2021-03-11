#include "FFEncoder.h"

namespace codec
{
	FFEncoder::~FFEncoder()
	{
		avcodec_free_context(&c);
		av_frame_free(&frame);
		av_packet_free(&pkt);

		sws_freeContext(sws_ctx);
	}

	bool FFEncoder::init(const VideoFormat& fmt)
	{
		sws_ctx = sws_getContext(fmt.dw, fmt.dh, AV_PIX_FMT_RGB24, fmt.dw, fmt.dh, AV_PIX_FMT_YUV420P,
			SWS_BILINEAR, NULL, NULL, NULL);
		if (!sws_ctx)
		{
			throw CodecException("sws_getContext error");
			return 0;
		}

		int ret = 0;

//----------------------------------------------------------------------




		if (fmt.id == VideoFormat::MPEG2)
		{
			codec = avcodec_find_encoder(AV_CODEC_ID_MPEG2VIDEO);

			if (!codec) {
				throw CodecException("Codec not found");
				return 0;
			}

			c = avcodec_alloc_context3(codec);
			if (!c) {
				throw CodecException("Could not allocate video codec context");
				return 0;
			}
			c->thread_count = fmt.threadCount;
			initMpeg2();
		}
	


		//----------------------------------------------------------------------

		if (fmt.id == VideoFormat::H264)
		{
			codec = avcodec_find_encoder(AV_CODEC_ID_H264);

			if (!codec) {
				throw CodecException("Codec not found");
				return 0;
			}
			c = avcodec_alloc_context3(codec);
			if (!c) {
				throw CodecException("Could not allocate video codec context");
				return 0;
			}
			c->thread_count = fmt.threadCount;
			initH264();
		}


		//----------------------------------------------------------------------



		if (fmt.id == VideoFormat::VP8)
		{
			codec = avcodec_find_encoder(AV_CODEC_ID_VP8);

			if (!codec) {
				throw CodecException("Codec not found");
				return 0;
			}
			c = avcodec_alloc_context3(codec);
			if (!c) {
				throw CodecException("Could not allocate video codec context");
				return 0;
			}
			c->thread_count = fmt.threadCount;
			initVp8();
		}





		//------------------------------------------------------------------------

		if (fmt.id == VideoFormat::MPEG4)
		{
			codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);

			if (!codec) {
				throw CodecException("Codec not found");
				return 0;
			}
			c = avcodec_alloc_context3(codec);
			if (!c) {
				throw CodecException("Could not allocate video codec context");
				return 0;
			}
			c->thread_count = fmt.threadCount;
			initMpeg4();
		}
		//-------------------------------------------------------------------------



		if (fmt.id == VideoFormat::MPEG1)
		{
			codec = avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);

			if (!codec) {
				throw CodecException("Codec not found");
				return 0;
			}
			c = avcodec_alloc_context3(codec);
			if (!c) {
				throw CodecException("Could not allocate video codec context");
				return 0;
			}
			c->thread_count = fmt.threadCount;
			initMpeg1();
		}
		//-------------------------------------------------------------------------





		c->bit_rate = fmt.bitarte * 1000;
		c->width = fmt.dw;
		c->height = fmt.dh;

		pkt = av_packet_alloc();
		if (!pkt)
		{
			throw CodecException("Could not allocate packet");
			return 0;
		}


		if (avcodec_open2(c, codec, NULL) < 0) {
			throw CodecException("Could not open codec");
			return 0;
		}


		frame = av_frame_alloc();
		if (!frame) {
			throw CodecException("Could not allocate video frame");
			return 0;
		}


		frame->format = c->pix_fmt;
		frame->width = c->width;
		frame->height = c->height;
		frame->pts = 0;
		ret = av_frame_get_buffer(frame, 16);
		return 1;
	}

	bool FFEncoder::encode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t& out_size)
	{
		frame->pts++;
		int ret = 0;
		ret = av_frame_make_writable(frame);

		int src_linesize[4];
		src_linesize[0] = c->width * 3;
		src_linesize[1] = 0;
		src_linesize[2] = 0;
		src_linesize[3] = 0;

		uint8_t* src_data[4];
		src_data[0] = (uint8_t*)in;
		src_data[1] = 0;
		src_data[2] = 0;
		src_data[3] = 0;

		ret = sws_scale(sws_ctx, src_data, src_linesize, 0, c->height, frame->data, frame->linesize);

		ret = avcodec_send_frame(c, frame);

		while (ret >= 0) {
			ret = avcodec_receive_packet(c, pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return false;
			else if (ret < 0) {
				return false;
			}

			memcpy(out, pkt->data, pkt->size);
			out_size = pkt->size;

			av_packet_unref(pkt);
		}
		return 1;
	}


	void FFEncoder::initVp8()
	{
		c->time_base = { 1, 25 };
		c->framerate = { 25, 1 };

		c->gop_size = 999999;
		//c->max_b_frames = 2;
		c->pix_fmt = AV_PIX_FMT_YUV420P;

		//-------------vp8
		c->qmin = 4;
		c->qmax = 56;
		c->rc_buffer_size = c->bit_rate * 6;
		c->rc_initial_buffer_occupancy = c->bit_rate * 4;
		c->profile = 3;
		//c->rc_max_rate = 500000;
		//c->rc_buffer_size = 1000000;
		//c->thread_count = std::thread::hardware_concurrency();
		//av_opt_set_double(c, "max-intra-rate", 90, 0);
		///av_opt_set(c, "quality", "realtime", 0);
	}

	void FFEncoder::initMpeg2()
	{
		/* put sample parameters */

		/* frames per second */
		c->time_base = { 1, 25 };
		c->framerate = { 25, 1 };

		c->gop_size = 12;
		c->max_b_frames = 2;
		c->pix_fmt = AV_PIX_FMT_YUV420P;
		//c->thread_count = std::thread::hardware_concurrency();
		//-----------------------
	}

	void FFEncoder::initMpeg4()
	{
		c->time_base = { 1, 25 };
		c->framerate = { 25, 1 };

		c->gop_size = 12;
		c->max_b_frames = 2;
		c->pix_fmt = AV_PIX_FMT_YUV420P;
		//c->thread_count = std::thread::hardware_concurrency();
	}

	void FFEncoder::initH264()
	{
		c->time_base = { 1, 25 };
		c->framerate = { 25, 1 };

		c->gop_size = 12;
		c->max_b_frames = 2;
		c->pix_fmt = AV_PIX_FMT_YUV420P;
		//c->thread_count = std::thread::hardware_concurrency();
		av_opt_set(c, "preset", "ultrafast", 0);
	}



	void FFEncoder::initMpeg1()
	{
		c->time_base = { 1, 25 };
		c->framerate = { 25, 1 };

		c->gop_size = 12;
		c->max_b_frames = 2;
		c->pix_fmt = AV_PIX_FMT_YUV420P;
		//c->thread_count = std::thread::hardware_concurrency();
	}
}
