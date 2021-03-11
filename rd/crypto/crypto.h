#pragma once

#include "RsaEncryptor.h"
#include "AesEncryptor.h"
#include "base64.h"
#include "HMAC_SHA.hpp"

#include <array>
#include <openssl/sha.h>



namespace crypto
{


	static int sha256(const uint8_t* buf, int size, uint8_t* hash)
	{
		SHA256_CTX _sha256;
		SHA256_Init(&_sha256);
		SHA256_Update(&_sha256, buf, size);
		return SHA256_Final(hash, &_sha256);
	}


	template<typename T1, typename T2>
	static int sha256(const T1& in, T2& out)
	{
		return sha256((const uint8_t*)in.data(), in.size(), (uint8_t*)out.data());
	}




	template<typename T>
	static int rand(T& t)
	{
		return RAND_bytes((uint8_t*)t.data(), t.size());
	}

	static int rand(uint8_t *buf, int size)
	{
		return RAND_bytes(buf, size);
	}


}
