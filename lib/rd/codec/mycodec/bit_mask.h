#pragma once
#include <vector>
#include <stdint.h>
#include <cstring>

struct bit_mask
{
	bit_mask() : bs(0), m_size(0) {}


	bit_mask(uint8_t *b, size_t s)
	{
		bs = b;
		m_size = s;
	}


	bool get_bit(int n)
	{

		int i = 0;
		int j = n;
		if (n > 7)
		{
			i = n >> 3;
			j = n % 8;
		}

		return (bs[i] & (1 << j));
	}

	size_t size()
	{
		return m_size;
	}

	size_t size_bit()
	{
		return m_size* 8;
	}

	uint8_t * data()
	{
		return bs;
	}


	void set_bit(int n)
	{
		int i = 0;
		int j = n;
		if (n > 7)
		{
			i = n >> 3;
			j = n % 8;
		}
		bs[i] |= (1 << j);
	}


	void clear()
	{
		memset(bs, 0, m_size);
	}

private:
	uint8_t *bs;
	size_t m_size;
};