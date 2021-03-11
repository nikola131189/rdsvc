#pragma once
#include <openssl/aes.h>
#include <vector>
#include <cstdint>


namespace crypto
{
	template <size_t KEY_SIZE>
	class AesEncryptor
	{
	public:
		void init(const uint8_t* key, uint32_t size)
		{
			AES_set_encrypt_key(key, KEY_SIZE, &m_enc_key);
			AES_set_decrypt_key(key, KEY_SIZE, &m_dec_key);
		}




		template<typename T1, typename T2>
		int encrypt(const T1& in, T2& out)
		{
			return encrypt((const uint8_t*)in.data(), (uint8_t*)out.data(), in.size());
		}


		template<typename T1, typename T2>
		int decrypt(const T1& in, T2& out)
		{
			return decrypt((const uint8_t*)in.data(), (uint8_t*)out.data(), in.size());
		}


		

		int encrypt(const uint8_t* in, uint8_t* out, uint32_t size)
		{
			for (size_t i = 0; i < size; i += 16)
			{
				AES_encrypt(in, out, &m_enc_key);
				in += 16; out += 16;
			}

			return 1;
		}




		int decrypt(const uint8_t* in, uint8_t* out, uint32_t size)
		{
			for (size_t i = 0; i < size; i += 16)
			{
				AES_decrypt(in, out, &m_dec_key);
				in += 16; out += 16;
			}
			return 1;
		}
		
	private:
		AES_KEY m_enc_key;
		AES_KEY m_dec_key;
	};


	
	
}
