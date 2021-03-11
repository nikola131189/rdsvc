#include "FileBuffer.h"

namespace file_system
{


	FileBuffer::FileBuffer()
		: _tasks(128), _thr(&FileBuffer::loop, this), _running(true)
	{
	}

	FileBuffer::~FileBuffer()
	{
		
		_running = false;
		_cv.notify_one();
		if (_thr.joinable())
			_thr.join();
	}

	void FileBuffer::write(const std::filesystem::path& path, int64_t offset, const std::vector<uint8_t>& buff, const write_callback& cbck)
	{
		_tasks.push(new std::variant<WriteTask, ReadTask>(WriteTask(path, buff, offset, cbck)));
		_cv.notify_one();
	}

	void FileBuffer::read(const std::filesystem::path& path, int64_t offset, uint8_t* buff, int64_t size, const read_callback& cbck)
	{
		_tasks.push(new std::variant<WriteTask, ReadTask>(ReadTask(path, offset, size, buff, cbck)));
		_cv.notify_one();
	}
	
	
	std::shared_ptr<File> FileBuffer::makeFile(const std::filesystem::path& path, File::OpenMode m)
	{
		auto it = _files.find(path);
		if (it != _files.end())
			return it->second;
		
		auto f = std::make_shared<File>();
		if (f->open(path, m))
		{
			_files[path] = f;
			return f;
		}
			
		return nullptr;
	}

	void FileBuffer::doRead(ReadTask* t)
	{
		auto file = makeFile(t->path, File::read_mode);

		if (!file)
		{
			t->cbck(0, 0, std::make_error_code(std::errc::bad_file_descriptor));
			return;
		}

		file->seek(t->offset);

		int64_t res = file->read(t->ptr, t->size);
		bool eof = file->eof();

		std::error_code ec;
		if (res == -1)
			ec = std::make_error_code(std::errc::bad_file_descriptor);
		if (t->cbck)
			t->cbck(res, eof, ec);
	}

	void FileBuffer::doWrite(WriteTask* t)
	{
		auto file = makeFile(t->path, File::write_mode);
		if (!file)
		{
			t->cbck(0, std::make_error_code(std::errc::bad_file_descriptor));
			return;
		}

		if (t->offset == 0)
			file->resize(0);

		file->seek(t->offset);

		int64_t res = 0;
		if(t->buff.size())
			res = file->write(&t->buff[0], t->buff.size());

		std::error_code ec;
		if (res == -1)
			ec = std::make_error_code(std::errc::bad_file_descriptor);
		if (t->cbck)
			t->cbck(res, ec);
	}



	void FileBuffer::loop()
	{
		while (_running)
		{
			std::unique_lock<std::mutex> lk(_mut);
			if (!_tasks.empty())
			{
				std::variant<WriteTask, ReadTask>* it;
				_tasks.pop(it);			
				if (auto pval = std::get_if<ReadTask>(it))
					doRead(pval);
				if (auto pval = std::get_if<WriteTask>(it))
					doWrite(pval);
				delete it;
				if (_tasks.empty())
				{
					_files.clear();
				}
			}
			else
			{
				_cv.wait(lk);
			}
		}
	}

}


