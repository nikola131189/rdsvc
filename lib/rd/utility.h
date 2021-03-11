#pragma once
#if defined ( _WIN32 )
#pragma comment(lib, "bcrypt.lib")
#include <boost/asio.hpp>
#include <windows.h>
#endif
#include <iostream>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <string>
#include <filesystem>
#include <iconv.h>

namespace utility
{

	template<typename CharOut, typename CharIn>
	static std::basic_string<CharOut> conv(std::basic_string<CharIn> const& in, const std::string& tocode, const std::string& fromcode)
	{
		if (in.empty()) return std::basic_string <CharOut>();
		if (tocode == fromcode) return (CharOut*)in.c_str();


		size_t in_len = in.length() * sizeof(CharIn);
		char* in_ptr = (char*)&in[0];

		size_t buf_size = in_len * 4;
		std::vector<char> buf(buf_size);
		char* buf_ptr = (char*)buf.data();
		iconv_t conv = iconv_open(tocode.c_str(), fromcode.c_str());
		size_t res = iconv(conv, &in_ptr, &in_len, &buf_ptr, &buf_size);
		iconv_close(conv);
		return (CharOut*)buf.data();
	}


	static int closet_multiple(int val, int q)
	{
		return (val % q) ? closet_multiple(val + 1, q) : val;
	}

	static uint64_t get_tick_count()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::steady_clock::now().time_since_epoch()).count();

	}


	static std::string gen_uuid()
	{
		boost::uuids::basic_random_generator<boost::mt19937> gen;
		boost::uuids::uuid u = gen();
		return boost::lexical_cast<std::string>(u);
	}


#ifdef _WIN32
	static std::wstring ConvertAnsiToWide(const std::string& str)
	{
		int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
		std::wstring wstr(count, 0);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &wstr[0], count);
		return wstr;
	}

	static std::wstring ConvertUtf8ToWide(const std::string& str)
	{
		int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
		std::wstring wstr(count, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
		return wstr;
	}

	static std::string ConvertWideToUtf8(const std::wstring& wstr)
	{
		int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
		std::string str(count, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
		return str;
	}

	static std::string ConvertWideToACP(const std::wstring& wstr)
	{
		int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
		std::string str(count, 0);
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
		return str;
	}
#endif

	static std::string localToU8(const std::string& str)
	{
#ifdef _WIN32
		return ConvertWideToUtf8(ConvertAnsiToWide(str));
		//return str;
#endif
#ifdef __linux__ 
		return str;
#endif
	}
}