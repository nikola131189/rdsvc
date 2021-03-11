#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <time.h>
#include <stdlib.h> 
#include <stdio.h>


struct LOG_storage
{
	static void init(const std::string &p, long max_size)
	{
		instance().m_adr = p;

		std::ofstream os(instance().m_adr, std::ios_base::app);
		if (os.tellp() > max_size && max_size)
		{
			os.close();
			os.open(instance().m_adr, std::ios_base::trunc);
		}
	}


	template<class T>
	static void write(const T& msg)
	{
		instance().m_os << msg;
	}


	static void open()
	{
		instance().m_mut.lock();
		instance().m_os.open(instance().m_adr, std::ios_base::app);
	}

	static void close()
	{
		instance().m_os.close();
		instance().m_mut.unlock();
	}

private:
	static LOG_storage& instance()
	{
		static LOG_storage s;
		return s;	
	}


	std::mutex m_mut;
	std::string m_adr;
	std::ofstream m_os;
};

	enum LOG_LEVEL
	{
		DEBUG,
		INFO,
		WARN,
		ERR
	};

struct LOG
{

	static void init(const std::string& fmt)
	{
		LOG::m_fmt = fmt;
	}

	LOG(LOG_LEVEL type)
	{
		LOG_storage::open();

		LOG_storage::write( "[" + m_levels[(uint32_t)type] + "] ");

		std::time_t seconds = std::time(nullptr);
		tm timeinfo;
		
#ifdef _WIN32
		int err = localtime_s(&timeinfo, &seconds);
		if (err) return;
#endif

#ifdef __linux__ 
		if (!localtime_r(&seconds, &timeinfo))
			return;
#endif



		
		char buffer[80];
		size_t s = strftime(buffer, 80, m_fmt.c_str(), &timeinfo);
		LOG_storage::write(std::string(buffer, s));
	}

	template<class T>
	LOG& operator << (const T& msg) 
	{
		LOG_storage::write(msg);
		return *this;
	}


	~LOG() 
	{
		LOG_storage::write("\n");
		LOG_storage::close();
	}
private:
	static inline std::string m_fmt = "%d.%m.%Y %H:%M:%S  ";
	static inline std::string m_levels[] = { "DEBUG", "INFO", "WARN", "ERROR" };
};
