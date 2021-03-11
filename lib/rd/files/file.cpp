#include "file.h"


namespace file_system
{
	File::File()
	{}

	File::~File()
	{
	}


	bool File::open(const std::filesystem::path& p, OpenMode mode)
	{
		m_path = p;
		{
			std::error_code ec;
			if (!std::filesystem::exists(p, ec))
			{
				if (!std::filesystem::exists(p.parent_path(), ec))
				{
					if (!std::filesystem::create_directories(p.parent_path(), ec))
					{
						return 0;
					}
				}
				std::ofstream os(p, std::ios_base::out | std::ios_base::binary);
			}

		}

		if (mode == read_mode)
			_os.open(p, std::fstream::in | std::fstream::binary);

		if (mode == write_mode)
			_os.open(p, std::fstream::in | std::fstream::out | std::fstream::binary);

		if (_os)
		{
			_buf = _os.rdbuf();
			return true;
		}
		return false;
	}

	void File::close()
	{
		
		_os.close();
	}

	bool File::isOpen()
	{
		return _os.is_open();
	}

	std::int64_t File::read(uint8_t* buff, uint64_t size)
	{
		if (!_os) return -1;
		return _buf->sgetn((char*)buff, size);
	}

	std::int64_t File::write(const uint8_t* buff, uint64_t size)
	{
		if (!_os) return -1;
		return _buf->sputn((const char*)buff, size);
	}

	std::uint64_t File::seek(std::int64_t pos)
	{
		if (!_os) return -1;
		return _buf->pubseekoff(pos, _os.beg);
	}

	std::int64_t File::size()
	{
		if (!_os) return -1;
		_os.seekp(0, _os.end);
		return _os.tellp();
	}

	bool File::eof()
	{
		return _buf->sgetc() == EOF;
	}

	std::filesystem::path File::path() const
	{
		return m_path;
	}


	void File::resize(int64_t size)
	{
		std::error_code ec;
		std::filesystem::resize_file(m_path, size, ec);
	}


}








