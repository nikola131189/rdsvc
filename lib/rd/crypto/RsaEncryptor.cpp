#include "RsaEncryptor.h"


namespace crypto
{
	RsaEncryptor::RsaEncryptor()
	{}


	void RsaEncryptor::genRsa(int bits)
	{
		RSA* pRSA = RSA_new();
		BIGNUM* bn = BN_new();
		int rc = BN_set_word(bn, RSA_F4);
		rc = RSA_generate_key_ex(pRSA, bits, bn, NULL);

		_priv = { RSAPrivateKey_dup(pRSA), RSADeleter() };
		_pub = { RSAPublicKey_dup(pRSA), RSADeleter() };
		RSA_free(pRSA);
		BN_free(bn);
	}

	int RsaEncryptor::pubKeySize()
	{
		return RSA_size(_pub.get());
	}

	int RsaEncryptor::privKeySize()
	{
		return RSA_size(_priv.get());
	}

	std::string RsaEncryptor::readKeyFromFile(const std::string& adr)
	{
		std::string res;
		std::ifstream is(adr, std::ios_base::binary);
		if (!is.is_open()) return "";
		is.seekg(0, is.end);
		std::streamoff length = is.tellg();
		is.seekg(0, is.beg);

		res.resize((size_t)length);
		is.read(&res[0], res.size());
		return res;
	}


	bool RsaEncryptor::initPublic(const std::string& key)
	{
		_pub = { create_public_RSA(key), RSADeleter() };
		if (_pub) return 1;
		return 0;
	}

	bool RsaEncryptor::initPrivate(const std::string& key)
	{
		_priv = { create_private_RSA(key), RSADeleter() };
		if (_priv) return 1;
		return 0;
	}


	int RsaEncryptor::encrypt(const uint8_t* in, int size, uint8_t* out)
	{
		memcpy(_inBuff.data(), in, size);
		int res = encrypt_RSA(_pub.get(), _inBuff.data(), size, _outBuff.data());
		if (res > 0)
		{
			memcpy(out, _outBuff.data(), res);
		}
		return res;
	}


	int RsaEncryptor::decrypt(const uint8_t* in, int size, uint8_t* out)
	{
		memcpy(&_inBuff[0], in, size);
		int res = decrypt_RSA(_priv.get(), _inBuff.data(), _outBuff.data());
		if (res > 0)
		{
			memcpy(out, _outBuff.data(), res);
		}
		return res;
	}

	std::string RsaEncryptor::privKey()
	{
		BIO* bio = BIO_new(BIO_s_mem());
		PEM_write_bio_RSAPrivateKey(bio, _priv.get(), NULL, NULL, 0, NULL, NULL);
		const int keylen = BIO_pending(bio);
		std::string key;
		key.resize(keylen);
		BIO_read(bio, &key[0], keylen);
		BIO_free_all(bio);

		return key;
	}

	std::string RsaEncryptor::pubKey()
	{
		BIO* bio = BIO_new(BIO_s_mem());
		int res = PEM_write_bio_RSA_PUBKEY(bio, _pub.get());
		const int keylen = BIO_pending(bio);
		std::string key;
		key.resize(keylen);
		BIO_read(bio, &key[0], keylen);
		BIO_free_all(bio);
		return key;
	}



	RSA* RsaEncryptor::create_public_RSA(const std::string& key)
	{
		RSA* rsa = nullptr;
		BIO* keybio;
		keybio = BIO_new_mem_buf(key.c_str(), key.size()); // !!!
		BIO_set_flags(keybio, BIO_FLAGS_BASE64_NO_NL);
		rsa = PEM_read_bio_RSA_PUBKEY(keybio, nullptr, nullptr, nullptr);  // !!!
		if (!rsa)
		{
			std::cout << ERR_error_string(ERR_get_error(), NULL) << std::endl;

		}
		BIO_free(keybio); // !!!
		return rsa;
	}


	RSA* RsaEncryptor::create_private_RSA(const std::string& key)
	{
		BIO* bio = BIO_new_mem_buf(key.c_str(), key.size());
		//BIO_set_flags( bio, BIO_FLAGS_BASE64_NO_NL ) ; // NO NL
		RSA* rsaPrivKey = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);

		if (!rsaPrivKey)
			printf("ERROR: Could not load PRIVATE KEY!  PEM_read_bio_RSAPrivateKey FAILED: %s\n", ERR_error_string(ERR_get_error(), NULL));

		BIO_free(bio);
		return rsaPrivKey;
	}


	int RsaEncryptor::encrypt_RSA(RSA* pubKey, const uint8_t* in, int size, uint8_t* out)
	{
		if (pubKey == NULL)
			return -1;

		int resultLen = RSA_public_encrypt(size, in, out, pubKey, RSA_PKCS1_PADDING);
		if (resultLen == -1)
		{
			printf("ERROR: RSA_public_encrypt: %s\n", ERR_error_string(ERR_get_error(), NULL));
			return -1;
		}
		//ERR_clear_error();
		//CRYPTO_cleanup_all_ex_data();
		return resultLen;
	}

	int RsaEncryptor::decrypt_RSA(RSA* privKey, const uint8_t* in, uint8_t* out)
	{
		if (privKey == NULL)
			return -1;

		int resultLen = RSA_private_decrypt(RSA_size(privKey), in, out, privKey, RSA_PKCS1_PADDING);
		if (resultLen == -1)
		{
			printf("ERROR: RSA_private_decrypt: %s\n", ERR_error_string(ERR_get_error(), NULL));
			return -1;
		}
		//int resultLen = 0;
		//ERR_clear_error();
		//CRYPTO_cleanup_all_ex_data();
		return resultLen;
	}

	EVP_PKEY* RsaEncryptor::create_rsa_key(RSA* pRSA)
	{

		EVP_PKEY* pKey = NULL;
		pKey = EVP_PKEY_new();
		if (pRSA && pKey && EVP_PKEY_assign_RSA(pKey, pRSA))
		{
			/* pKey owns pRSA from now */
			if (RSA_check_key(pRSA) <= 0)
			{
				fprintf(stderr, "RSA_check_key failed.\n");
	
				EVP_PKEY_free(pKey);
				pKey = NULL;
			}
		}
		else
		{
			if (pRSA)
			{
				RSA_free(pRSA);
				pRSA = NULL;
			}
			if (pKey)
			{
				EVP_PKEY_free(pKey);
				pKey = NULL;
			}
		}
		return pKey;
	}
}