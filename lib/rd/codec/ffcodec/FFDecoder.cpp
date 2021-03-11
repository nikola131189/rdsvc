#include "FFDecoder.h"
namespace codec
{
    bool FFDecoder::init(const VideoFormat& fmt)
    {
        pkt = av_packet_alloc();
		if (!pkt)
		{
			throw CodecException("Could not allocate packet");
			return 0;
		}

        switch (fmt.id)
        {
        case VideoFormat::H264:
            codec = avcodec_find_decoder(AV_CODEC_ID_H264);
            break;
        case VideoFormat::MPEG4:
            codec = avcodec_find_decoder(AV_CODEC_ID_MPEG4);
            break;
        case VideoFormat::MPEG2:
            codec = avcodec_find_decoder(AV_CODEC_ID_MPEG2VIDEO);
            break;
		case VideoFormat::MPEG1:
            codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
            break;
        case VideoFormat::VP8:
            codec = avcodec_find_decoder(AV_CODEC_ID_VP8);
            break;
        default:
            return 0;
        }

        if (!codec) {
            throw CodecException("Codec not found");
            return 0;
        }

        parser = av_parser_init(codec->id);
        if (!parser) {
            throw CodecException("parser not found");
            return 0;
        }

        c = avcodec_alloc_context3(codec);
        if (!c) {
            throw CodecException("Could not allocate video codec context");
            return 0;
        }

        /* For some codecs, such as msmpeg4 and mpeg4, width and height
           MUST be initialized there because this information is not
           available in the bitstream. */

           /* open it */
        if (avcodec_open2(c, codec, NULL) < 0) {
            throw CodecException("Could not open codec");
            return 0;
        }


        frame = av_frame_alloc();
        if (!frame) {
            throw CodecException("Could not allocate video frame");
            return 0;
        }


        sws_ctx = sws_getContext(fmt.dw, fmt.dh, AV_PIX_FMT_YUV420P, fmt.dw, fmt.dh, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, NULL, NULL, NULL);
		if (!sws_ctx)
		{
			throw CodecException("sws_getContext error");
			return 0;
		}
        return true;
    }

    bool FFDecoder::decode(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
    {
        int ret;

        pkt->data = (uint8_t*)in;
        pkt->size = in_size;
        /*ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
            in, in_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);*/


        ret = avcodec_send_packet(c, pkt);
        if (ret < 0) {
            fprintf(stderr, "Error sending a packet for decoding\n");
            return 0;
        }

        ret = avcodec_receive_frame(c, frame);
        if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            return 0;
        }

        int dst_linesize[4];
        dst_linesize[0] = c->width * 3;
        dst_linesize[1] = 0;
        dst_linesize[2] = 0;
        dst_linesize[3] = 0;

        uint8_t* dst_data[4];
        dst_data[0] = out;
        dst_data[1] = 0;
        dst_data[2] = 0;
        dst_data[3] = 0;

        ret = sws_scale(sws_ctx, frame->data, frame->linesize, 0, c->height, dst_data, dst_linesize);

        return 1;
    }

    FFDecoder::~FFDecoder()
    {
        avcodec_free_context(&c);
        av_frame_free(&frame);
        av_packet_free(&pkt);
        sws_freeContext(sws_ctx);
    }
}
