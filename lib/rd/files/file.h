#pragma once
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <boost/lockfree/queue.hpp>
#include <variant>
#include "../utility.h"
#include <boost/asio.hpp>


namespace file_system
{

	class File
	{
	public:
		enum OpenMode
		{
			read_mode,
			write_mode
		};

		File();
		~File();
		File(const File&) = delete;
		File& operator=(const File&) = delete;

		bool open(const std::filesystem::path& p, OpenMode mode);
		void close();
		bool isOpen();
		std::int64_t read(uint8_t* buff, uint64_t size);
		std::int64_t write(const uint8_t* buff, uint64_t size);
		std::uint64_t seek(std::int64_t pos);
		std::int64_t size();
		bool eof();
		std::filesystem::path path() const;
		void resize(int64_t size);
	private:
		std::filesystem::path m_path;
		std::fstream _os;
		std::filebuf* _buf = 0;
	};


	

}
