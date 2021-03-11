#pragma once

#include <openssl/hmac.h>
#include <string>
#include <array>

template<size_t SIZE>
class HMAC_SHA {
public:
	template<typename T1, typename T2>
	HMAC_SHA(const T1& key, const T2& msg) {
		/*HMAC_CTX* ctx = HMAC_CTX_new();

		if(SIZE == 512)
			HMAC_Init_ex(ctx, reinterpret_cast<const unsigned char*>(key.c_str()), key.size(), EVP_sha512(), NULL);
		
		if (SIZE == 256)
			HMAC_Init_ex(ctx, reinterpret_cast<const unsigned char*>(key.c_str()), key.size(), EVP_sha256(), NULL);

		HMAC_Update(ctx, reinterpret_cast<const unsigned char*>(msg.c_str()), msg.size());


		HMAC_Final(ctx, _digest.data(), &_resultLen);

		HMAC_CTX_free(ctx);*/

		HMAC_CTX ctx;
		HMAC_CTX_init(&ctx);

		if (SIZE == 512)
			HMAC_Init_ex(&ctx, reinterpret_cast<const unsigned char*>(key.data()), key.size(), EVP_sha512(), NULL);

		if (SIZE == 256)
			HMAC_Init_ex(&ctx, reinterpret_cast<const unsigned char*>(key.data()), key.size(), EVP_sha256(), NULL);

		HMAC_Update(&ctx, reinterpret_cast<const unsigned char*>(msg.data()), msg.size());


		HMAC_Final(&ctx, _digest.data(), &_resultLen);

		HMAC_CTX_cleanup(&ctx);
	}

	std::string hexDigest() {
		std::string str;
		const char hex_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		for (int i = 0; i < _resultLen; i++) {
			const char byte = _digest[i];
			str.push_back(hex_chars[(byte & 0xF0) >> 4]);
			str.push_back(hex_chars[(byte & 0x0F) >> 0]);
		}
		return str;
	}

private:
	std::array<uint8_t, SIZE> _digest;
	unsigned int _resultLen = SIZE;
};

