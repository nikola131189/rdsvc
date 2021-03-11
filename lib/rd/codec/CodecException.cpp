#include "CodecException.h"

namespace codec
{
	CodecException::CodecException(const std::string& error)
		: _error(error)

	{
	}

	CodecException::CodecException(const std::string& error, int returnValue)
	{
		std::stringstream s;
		s << error << " " << returnValue;
		_error = s.str();
	}
}
