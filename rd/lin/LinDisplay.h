#pragma once
#include "../Native.h"

#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>

class LinDisplay : public Rd::Native::Display
{
public:
	LinDisplay();
	void getDisplay(Rd::DisplayInfo& inf);
	void openScreen(const std::string& src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input);
	void closeScreen(int id);
	~LinDisplay();
private:
	bool getResolutions(std::string& res);
	size_t parseInt(const std::string& str, size_t p, int& res);
	size_t parseInt(const std::string& str, size_t p, const std::string& delim, int& res);
	std::vector<Rd::ScreenInfo> parseResolutions(const std::string& data);
	pid_t _pid = 0;

private:
	std::thread _thr;
	void forkScreen(const std::string& src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input);
};
