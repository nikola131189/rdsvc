#pragma once
#include <openssl/opensslv.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef OPENSSL_ALGORITHM_DEFINES
# error OPENSSL_ALGORITHM_DEFINES no longer supported
#endif

	/*
	 * OpenSSL was configured with the following options:
	 */

#ifndef OPENSSL_SYS_MINGW64
# define OPENSSL_SYS_MINGW64 1
#endif
#ifndef OPENSSL_NO_MD2
# define OPENSSL_NO_MD2
#endif
#ifndef OPENSSL_THREADS
# define OPENSSL_THREADS
#endif
#ifndef OPENSSL_RAND_SEED_OS
# define OPENSSL_RAND_SEED_OS
#endif
#ifndef OPENSSL_NO_ASAN
# define OPENSSL_NO_ASAN
#endif
#ifndef OPENSSL_NO_CRYPTO_MDEBUG
# define OPENSSL_NO_CRYPTO_MDEBUG
#endif
#ifndef OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
# define OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
#endif
#ifndef OPENSSL_NO_DEVCRYPTOENG
# define OPENSSL_NO_DEVCRYPTOENG
#endif
#ifndef OPENSSL_NO_EC_NISTP_64_GCC_128
# define OPENSSL_NO_EC_NISTP_64_GCC_128
#endif
#ifndef OPENSSL_NO_EGD
# define OPENSSL_NO_EGD
#endif
#ifndef OPENSSL_NO_EXTERNAL_TESTS
# define OPENSSL_NO_EXTERNAL_TESTS
#endif
#ifndef OPENSSL_NO_FUZZ_AFL
# define OPENSSL_NO_FUZZ_AFL
#endif
#ifndef OPENSSL_NO_FUZZ_LIBFUZZER
# define OPENSSL_NO_FUZZ_LIBFUZZER
#endif
#ifndef OPENSSL_NO_HEARTBEATS
# define OPENSSL_NO_HEARTBEATS
#endif
#ifndef OPENSSL_NO_MSAN
# define OPENSSL_NO_MSAN
#endif
#ifndef OPENSSL_NO_SCTP
# define OPENSSL_NO_SCTP
#endif
#ifndef OPENSSL_NO_SSL_TRACE
# define OPENSSL_NO_SSL_TRACE
#endif
#ifndef OPENSSL_NO_SSL3
# define OPENSSL_NO_SSL3
#endif
#ifndef OPENSSL_NO_SSL3_METHOD
# define OPENSSL_NO_SSL3_METHOD
#endif
#ifndef OPENSSL_NO_UBSAN
# define OPENSSL_NO_UBSAN
#endif
#ifndef OPENSSL_NO_UNIT_TEST
# define OPENSSL_NO_UNIT_TEST
#endif
#ifndef OPENSSL_NO_WEAK_SSL_CIPHERS
# define OPENSSL_NO_WEAK_SSL_CIPHERS
#endif
#ifndef OPENSSL_NO_DYNAMIC_ENGINE
# define OPENSSL_NO_DYNAMIC_ENGINE
#endif
#ifndef OPENSSL_NO_AFALGENG
# define OPENSSL_NO_AFALGENG
#endif


	 /*
	  * Sometimes OPENSSSL_NO_xxx ends up with an empty file and some compilers
	  * don't like that.  This will hopefully silence them.
	  */
#define NON_EMPTY_TRANSLATION_UNIT static void *dummy = &dummy;

	  /*
	   * Applications should use -DOPENSSL_API_COMPAT=<version> to suppress the
	   * declarations of functions deprecated in or before <version>. Otherwise, they
	   * still won't see them if the library has been built to disable deprecated
	   * functions.
	   */
#ifndef DECLARE_DEPRECATED
# define DECLARE_DEPRECATED(f)   f;
# ifdef __GNUC__
#  if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 0)
#   undef DECLARE_DEPRECATED
#   define DECLARE_DEPRECATED(f)    f __attribute__ ((deprecated));
#  endif
# endif
#endif

#ifndef OPENSSL_FILE
# ifdef OPENSSL_NO_FILENAMES
#  define OPENSSL_FILE ""
#  define OPENSSL_LINE 0
# else
#  define OPENSSL_FILE __FILE__
#  define OPENSSL_LINE __LINE__
# endif
#endif

#ifndef OPENSSL_MIN_API
# define OPENSSL_MIN_API 0
#endif

#if !defined(OPENSSL_API_COMPAT) || OPENSSL_API_COMPAT < OPENSSL_MIN_API
# undef OPENSSL_API_COMPAT
# define OPENSSL_API_COMPAT OPENSSL_MIN_API
#endif

	   /*
		* Do not deprecate things to be deprecated in version 1.2.0 before the
		* OpenSSL version number matches.
		*/
#if OPENSSL_VERSION_NUMBER < 0x10200000L
# define DEPRECATEDIN_1_2_0(f)   f;
#elif OPENSSL_API_COMPAT < 0x10200000L
# define DEPRECATEDIN_1_2_0(f)   DECLARE_DEPRECATED(f)
#else
# define DEPRECATEDIN_1_2_0(f)
#endif

#if OPENSSL_API_COMPAT < 0x10100000L
# define DEPRECATEDIN_1_1_0(f)   DECLARE_DEPRECATED(f)
#else
# define DEPRECATEDIN_1_1_0(f)
#endif

#if OPENSSL_API_COMPAT < 0x10000000L
# define DEPRECATEDIN_1_0_0(f)   DECLARE_DEPRECATED(f)
#else
# define DEPRECATEDIN_1_0_0(f)
#endif

#if OPENSSL_API_COMPAT < 0x00908000L
# define DEPRECATEDIN_0_9_8(f)   DECLARE_DEPRECATED(f)
#else
# define DEPRECATEDIN_0_9_8(f)
#endif

		/* Generate 80386 code? */
#undef I386_ONLY

#undef OPENSSL_UNISTD
#define OPENSSL_UNISTD <unistd.h>

#define OPENSSL_EXPORT_VAR_AS_FUNCTION

/*
 * The following are cipher-specific, but are part of the public API.
 */
#if !defined(OPENSSL_SYS_UEFI)
# undef BN_LLONG
 /* Only one for the following should be defined */
# undef SIXTY_FOUR_BIT_LONG
# define SIXTY_FOUR_BIT
# undef THIRTY_TWO_BIT
#endif

#define RC4_INT unsigned int

#ifdef  __cplusplus
}
#endif


#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <array>

namespace crypto
{
	class RsaEncryptor
	{
	public:
		RsaEncryptor();
		RsaEncryptor(const RsaEncryptor&);
		~RsaEncryptor();

		void genRsa(int bits);

		int pubKeySize();
		int privKeySize();

		static std::string readKeyFromFile(const std::string& adr);

		bool initPublic(const std::string& key);

		bool initPrivate(const std::string& key);

		template<typename T1, typename T2>
		int encrypt(const T1& in, T2& out)
		{
			out.resize(_inBuff.size());
			int res = encrypt((const uint8_t*)in.data(), in.size(), (uint8_t*)out.data());
			if(res < 0)
				out.resize(0);
			else
				out.resize(res);
			return res;
		}


		int encrypt(const uint8_t* in, int size, uint8_t* out);


		template<typename T1, typename T2>
		int decrypt(const T1& in, T2& out)
		{
			out.resize(_inBuff.size());
			int res = decrypt((const uint8_t *)in.data(), in.size(), (uint8_t*)out.data());
			if (res < 0)
				out.resize(0);
			else
				out.resize(res);
			return res;
		}

		int decrypt(const uint8_t* in, int size, uint8_t* out);

		std::string privKey();
		std::string pubKey();
		std::vector<uint8_t> pubKeyBin();
	private:
		RSA* create_public_RSA(const std::string& key);
		RSA* create_private_RSA(const std::string& key);

		int encrypt_RSA(RSA* pubKey, const uint8_t* in, int size, uint8_t* out);
		int decrypt_RSA(RSA* privKey, const uint8_t* in, uint8_t* out);

		EVP_PKEY* create_rsa_key(RSA* pRSA);
	private:
		RSA *_pub, *_priv;
		std::array<uint8_t, 4096> _inBuff, _outBuff;
		std::string _pubKey, _privKey;
	};
}
