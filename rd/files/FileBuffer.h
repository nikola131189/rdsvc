#pragma once
#include "file.h"
#include <boost/lockfree/queue.hpp>
#include <variant>
namespace file_system
{

	class FileBuffer
	{
	public:
		typedef std::function<void(int64_t size, const std::error_code & ec)> write_callback;
		typedef std::function<void(int64_t size, bool eof, const std::error_code & ec)> read_callback;


	private:


		struct ReadTask
		{
			ReadTask(const std::filesystem::path& path_, int64_t off, int64_t s, uint8_t *ptr_, read_callback c) : 
				path(path_), offset(off), size(s), ptr(ptr_), cbck(c)
			{}
			std::filesystem::path path;
			int64_t offset, size;
			uint8_t* ptr;		
			read_callback cbck;
		};


		struct WriteTask
		{
			WriteTask(const std::filesystem::path& p, const std::vector<uint8_t>& b, int64_t off, write_callback c) :
				path(p), offset(off), buff(b), cbck(c)
			{}
			int64_t offset;
			std::vector<uint8_t> buff;
			std::filesystem::path path;
			write_callback cbck;
		};


	public:
		FileBuffer();
		~FileBuffer();
		void write(const std::filesystem::path& path, int64_t offset, const std::vector<uint8_t>& buff, const write_callback& cbck = nullptr);
		void read(const std::filesystem::path& path, int64_t offset, uint8_t* buff, int64_t size, const read_callback& cbck = nullptr);
	private:
		std::shared_ptr<File> makeFile(const std::filesystem::path& path, File::OpenMode m);
		void doRead(ReadTask* t);
		void doWrite(WriteTask* t);
		void loop();
	private:	
		std::condition_variable _cv;
		std::mutex _mut;		
		boost::lockfree::queue<std::variant<WriteTask, ReadTask>* > _tasks;
		std::map<std::filesystem::path, std::shared_ptr<File>> _files;
		std::thread _thr;
		bool _running;
	};

}