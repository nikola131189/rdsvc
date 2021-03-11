#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>

#ifdef _WIN32
#include <boost/asio.hpp>
#include <sys/stat.h>
#include <windows.h>
#endif

namespace files
{

	enum class file_type {
		none_file,
		not_found_file,
		regular_file,
		directory_file,
		symlink_file,
		block_file,
		character_file,
		fifo_file,
		socket_file
	};


	namespace fs = std::filesystem;

	struct file_info_t
	{
		fs::path path;
		uint64_t size = 0;
		std::time_t time = 0;
		file_type type = file_type::none_file;
		std::filesystem::perms perm = std::filesystem::perms::none;
	};

	static file_type get_file_type(const fs::path & p, std::error_code & ec)
	{
		if (fs::is_block_file(p, ec)) return file_type::block_file;

		if (fs::is_regular_file(p, ec)) return file_type::regular_file;

		if (fs::is_character_file(p, ec)) return file_type::character_file;

		if (fs::is_directory(p, ec)) return file_type::directory_file;

		if (fs::is_fifo(p, ec)) return file_type::fifo_file;

		if (fs::is_socket(p, ec)) return file_type::socket_file;

		if (fs::is_symlink(p, ec)) return file_type::symlink_file;

		return file_type::none_file;
	}


	static std::uintmax_t get_file_size(const fs::path & p, std::error_code & ec)
	{
		if (fs::is_regular_file(p, ec)) {
			return fs::file_size(p, ec);
		}
		return 0;
	}

	template <typename TP>
	static std::time_t to_time_t(TP tp)
	{
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
			+ system_clock::now());
		return system_clock::to_time_t(sctp);
	}


	static std::time_t get_last_write_time(const fs::path & p, std::error_code & ec)
	{

#ifdef _WIN32
		struct _stat64 fileInfo;
		if (_wstati64(p.wstring().c_str(), &fileInfo) != 0)
		{
			return 0;
		}
		return fileInfo.st_mtime;
#endif
		
#ifdef __linux__ 
		auto fsTime = std::filesystem::last_write_time(p, ec);
		return to_time_t(fsTime);
#endif
	}


#ifdef _WIN32
	static std::string ConvertWideToU8(const std::wstring& wstr)
	{
		if (wstr.empty()) return "";
		int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
		std::string str(count, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
		return str;
	}
#endif



	static void dir(const fs::path &p, std::list<file_info_t> &res, std::error_code& ec)
	{
		for (auto& x : fs::directory_iterator(p, ec))
		{
			file_info_t f;
#ifdef _WIN32
			//f.path = x.path();
			//int s = x.path().generic_u16string().size();
			f.path = ConvertWideToU8(x.path().generic_wstring());
#endif

#ifdef __linux__ 
			f.path = x.path();
#endif
			f.type = get_file_type(x, ec);
			f.size = get_file_size(x, ec);
			f.time = get_last_write_time(x, ec);
			f.perm = std::filesystem::status(x, ec).permissions();
			if (ec)
				f.perm = fs::perms::none;
			res.push_back(f);
		}
	}

	static void dir_recursion(const fs::path & p, std::list<file_info_t>& res, std::error_code &ec)
	{
		for (auto x : fs::directory_iterator(p, ec))
		{
			file_info_t f;
#ifdef _WIN32
			f.path = ConvertWideToU8(x.path());
#endif

#ifdef __linux__ 
			f.path = x.path();
#endif
			f.type = get_file_type(x, ec);
			f.size = get_file_size(x, ec);
			f.time = get_last_write_time(x, ec);
			if (ec) return;
			res.push_back(f);
			if (f.type == file_type::directory_file)
				dir_recursion(x, res, ec);
		}
	}

	static bool create_if_not_exists(const fs::path & p, uintmax_t size, std::error_code &ec)
	{
		if (!fs::exists(p, ec))
		{
			if (!fs::exists(p.parent_path(), ec))
			{
				if (!fs::create_directories(p.parent_path(), ec))
				{
					return 0;
				}
			}

			std::ofstream os(p, std::ios::binary);
			if (!os.is_open()) return 0;
		}
		if (fs::file_size(p, ec) != size)
		{
			fs::resize_file(p, size, ec);

		}
		if (ec) return false;
		return true;
	}


	static bool create_if_not_exists(const fs::path& p, std::error_code& ec)
	{
		if (!fs::exists(p, ec))
		{
			if (!fs::exists(p.parent_path(), ec))
			{
				if (!fs::create_directories(p.parent_path(), ec))
				{
					return 0;
				}
			}

			std::ofstream os(p, std::ios::binary);
			if (!os.is_open()) return 0;
		}
		if (ec) return false;
		return true;
	}
}