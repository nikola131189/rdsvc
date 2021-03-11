#pragma once
#include "../SystemApi.h"

#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <functional>
#include <boost/asio/ip/host_name.hpp>






class LinApi : public SystemApi
{
public:
	LinApi();

	void getDisplay(rd::DisplayInfo& inf);
	void openScreen(rd::id_t src, rd::id_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input);
	void closeScreen(int id);
	std::string terminalApp();
	std::string computerName();
private:
	std::vector<rd::ScreenInfo> parseResolutions(const std::string& data);
	bool getResolutions(std::string& res);
	size_t parseInt(const std::string& str, size_t p, int& res);
	size_t parseInt(const std::string& str, size_t p, const std::string& delim, int& res);
	std::string id_to_str(rd::id_t id) const;


	pid_t _pid = 0;
};

