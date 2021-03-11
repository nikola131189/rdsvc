#pragma once
#include <cstdint>
#include <cstring>

#include <sstream>
#include <array>
#include <zlib.h>
#include <immintrin.h>

#ifdef __linux__ 
#include <popcntintrin.h>

#elif _WIN32
#include  <intrin.h>
#else

#endif





/*#include <windows.h>
#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s << "\n";                   \
   OutputDebugStringW( os_.str().c_str() );  \
}*/

namespace codec
{
	template <size_t bpp>
	struct image_t
	{
		image_t(uint8_t* b, int w, int h, int stride)
			: m_data(b), m_width(w), m_height(h), m_stride(stride)
		{}
		image_t()
			: m_data(0), m_width(0), m_height(0), m_stride(0)
		{}


		inline void fill(int x1, int y1, const image_t& im)
		{
			uint8_t* p_src = im.m_data;
			uint8_t* p_dest = m_data + y1 * m_stride * bpp + x1 * bpp;

			for (int y = 0; y < im.m_height; y++)
			{
				::std::memcpy(p_dest, p_src, im.m_width * bpp);
				p_src += im.m_stride * bpp;
				p_dest += m_stride * bpp;
			}
		}


		inline void fill1(int x1, int y1, const image_t& im)
		{
			uint8_t* p_src = im.m_data;
			uint8_t* p_dest = m_data + y1 * m_stride * bpp + x1 * bpp;

			for (size_t y = 0; y < im.m_height; y++)
			{
				for (size_t x = 0; x < im.m_width * bpp; x++)
				{
					if (*(p_dest + x) == *(p_src + x))
						*(p_dest + x) = 0x0;
					else
						*(p_dest + x) = *(p_src + x);
				}
				p_src += im.m_stride * bpp;
				p_dest += m_stride * bpp;
			}
		}


		inline void fill2(int x1, int y1, const image_t& im)
		{
			uint8_t* p_src = im.m_data;
			uint8_t* p_dest = m_data + y1 * m_stride * bpp + x1 * bpp;

			for (size_t y = 0; y < im.m_height; y++)
			{
				for (size_t x = 0; x < im.m_width * bpp; x++)
				{
					if (*(p_src + x) != 0x0)
						*(p_dest + x) = *(p_src + x);
				}
				p_src += im.m_stride * bpp;
				p_dest += m_stride * bpp;
			}
		}

		inline image_t clip(int x, int y, int w, int h) const
		{
			uint8_t* ptr = m_data + y * m_stride * bpp + x * bpp;
			return { ptr, w, h, m_stride };
		}

		template<typename Func>
		inline void transform(const Func& f)
		{
			uint8_t* ptr = m_data;
			for (size_t i = 0; i < m_width * m_height * bpp; i++)
			{
				f(ptr);
				ptr++;
			}
		}



		inline bool operator ==(const image_t& other)
		{
			if (m_width != other.m_width || m_height != other.m_height) return false;
			uint8_t* ptr1 = m_data;
			uint8_t* ptr2 = other.m_data;

			for (size_t y = 0; y < m_height; y++)
			{
				for (size_t x = 0; x < m_width * bpp; x++)
				{
					if (ptr1[x] != ptr2[x]) return false;
				}
				ptr1 += m_stride * bpp;
				ptr2 += m_stride * bpp;
			}
			return true;
		}

		inline bool operator !=(const image_t& other)
		{
			if (m_width != other.m_width || m_height != other.m_height) return true;
			uint8_t* ptr1 = m_data;
			uint8_t* ptr2 = other.m_data;

			for (size_t y = 0; y < m_height; y++)
			{
				for (size_t x = 0; x < m_width * bpp; x++)
				{
					if (ptr1[x] != ptr2[x]) return true;
				}
				ptr1 += m_stride * bpp;
				ptr2 += m_stride * bpp;
			}
			return false;
		}

#ifdef _WIN32
		inline bool compare_sse_16(const image_t& other)
		{
			if (m_width != other.m_width || m_height != other.m_height) return true;
			uint8_t* ptr1 = m_data;
			uint8_t* ptr2 = other.m_data;

			for (int y = 0; y < m_height; y++)
			{
				
				for (size_t x = 0; x < m_width * bpp; x += 16)
				{
					auto v1 = _mm_loadu_si128((__m128i *)&ptr1[x]);
					auto v2 = _mm_loadu_si128((__m128i*) &ptr2[x]);
					unsigned vcmp = __popcnt(_mm_movemask_epi8(_mm_cmpeq_epi16(v1, v2)));
					if (vcmp != 16) return false;
				}
				ptr1 += m_stride * bpp;
				ptr2 += m_stride * bpp;
			}
			return true;
		}
#endif


#ifdef __linux__ 
		inline bool compare_sse_16(const image_t& other)
		{
			if (m_width != other.m_width || m_height != other.m_height) return true;
			uint8_t* ptr1 = m_data;
			uint8_t* ptr2 = other.m_data;

			for (size_t y = 0; y < m_height; y++)
			{

				for (size_t x = 0; x < m_width * bpp; x += 16)
				{
					auto v1 = _mm_loadu_si128((__m128i*) & ptr1[x]);
					auto v2 = _mm_loadu_si128((__m128i*) & ptr2[x]);
					unsigned vcmp = __builtin_popcount(_mm_movemask_epi8(_mm_cmpeq_epi16(v1, v2)));
					if (vcmp != 16) return false;
				}
				ptr1 += m_stride * bpp;
				ptr2 += m_stride * bpp;
			}
			return true;
		}
#endif

		uint8_t* data() { return m_data; }
	private:
		uint8_t* m_data;
		int m_width, m_height, m_stride;
	};


#pragma pack(push, 1)
	struct header_t
	{
		uint16_t w, h;
		uint8_t q, flag;
		uint32_t bm_size;
	};
#pragma pack(pop)
	/*
	static int zstd_compress(const std::vector<uint8_t> & in, std::vector<uint8_t> & out, int lvl)
	{
		uLongf destLen = out.size();
		uLong sourceLen = in.size();
		int res = compress2(&out[0], &destLen, &in[0], sourceLen, lvl);
		if (res == Z_OK)
		{
			return destLen;
		}
		return 0;
	}


	static int zstd_uncompress(const std::vector<uint8_t> & in, std::vector<uint8_t> & out)
	{
		uLongf destLen = out.size();
		uLong sourceLen = in.size();
		int res = uncompress(&out[0], &destLen, &in[0], sourceLen);
		if (res == Z_OK)
		{
			return destLen;
		}
		return 0;
	}*/

#include <zstd.h>

	static int zstd_compress(const ::std::vector<uint8_t>& in, ::std::vector<uint8_t>& out, int lvl)
	{
		out.resize(ZSTD_compressBound(in.size()));
		return ZSTD_compress(&out[0], out.size(), &in[0], in.size(), lvl);
	}


	static int zstd_uncompress(const uint8_t* in, uint32_t in_size, uint8_t* out, uint32_t out_size)
	{
		return ZSTD_decompress(&out[0], out_size, &in[0], in_size);
	}
	/*
	#include <lz4.h>

	static int zlib_compress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out, int lvl)
	{
		return LZ4_compress_fast((const char *)&in[0], (char *)&out[0], in.size(), out.size(), lvl);

	}


	static int zlib_uncompress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out)
	{
		return LZ4_decompress_safe((const char*)&in[0], (char*)&out[0], in.size(), out.size());
	}


	static bool compare_128(uint8_t* v1, uint8_t* v2)
	{
		auto _v1 = _mm_loadu_si128((__m128i*)v1);
		auto _v2 = _mm_loadu_si128((__m128i*)v2);

		unsigned vcmp = __popcnt(_mm_movemask_epi8(_mm_cmpeq_epi16(_v1, _v2)));
		if (vcmp != 16) return false;
		return true;
	}

	static __m128i compare_128_mask(uint8_t* v1, uint8_t* v2)
	{
		auto _v1 = _mm_loadu_si128((__m128i*)v1);
		auto _v2 = _mm_loadu_si128((__m128i*)v2);
		return _mm_cmpeq_epi16(_v1, _v2);
	}

	static bool mask_128_is_set(__m128i mask)
	{
		unsigned vcmp = __popcnt(_mm_movemask_epi8(mask));
		if (vcmp != 16) return 0;
		return 1;
	}

	static void maskmoveu_si128(uint8_t *v, __m128i mask, uint8_t* mem_addr)
	{
		//memcpy(mem_addr, v, 16);
		auto _v = _mm_loadu_si128((__m128i*)v);
		_mm_maskmoveu_si128(_v, mask, (char *)mem_addr);
	}*/
}