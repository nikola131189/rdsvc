#pragma once
#include <memory>
#include "ffcodec/FFDecoder.h"
#include "ffcodec/FFEncoder.h"
#include "mycodec/my_decoder.h"
#include "mycodec/my_encoder.h"

namespace codec
{
	struct CodecFactory
	{
		static std::shared_ptr<Encoder> makeEncoder(const VideoFormat& fmt)
		{
			if (fmt.id == VideoFormat::ZSTD)
				return std::make_shared<my_encoder>();
		
			return std::make_shared<FFEncoder>();
		}

		static std::shared_ptr<Decoder> makeDecoder(const VideoFormat& fmt)
		{
			if (fmt.id == VideoFormat::ZSTD)
				return std::make_shared<my_decoder>();
			return std::make_shared<FFDecoder>();
		}
	};
}
