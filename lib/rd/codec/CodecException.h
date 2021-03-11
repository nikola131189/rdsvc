#pragma once
#include <string>
#include <exception>
#include <sstream>
namespace codec
{

	class CodecException : public std::exception
	{

	public:

		CodecException(const std::string& error);

		CodecException(const std::string&error, int returnValue);

		const char * what()
		{
			return _error.c_str();
		}

	private:
		std::string _error;

	};
}

