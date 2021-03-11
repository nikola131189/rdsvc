#pragma once

#include <map>
#include <string>

struct ImprintStore
{
	uint32_t genId(const std::string& imprint)
	{
		auto it = _data.find(imprint);
		if (it == _data.end())
		{
			_id++;
			_data[imprint] = _id;
			return _id;
		}
		return it->second;
	}

private:
	std::map<std::string, uint32_t> _data;
	uint32_t _id = 0;
};

