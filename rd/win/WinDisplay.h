#pragma once
#include "../Native.h"

#include "WinProcess.h"
#include <Wbemidl.h>

#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

#include <VersionHelpers.h>


#include <Lm.h>
#pragma comment(lib, "Netapi32.lib")


class WinDisplay : public Rd::Native::Display
{
public:
	WinDisplay();
	void getDisplay(Rd::DisplayInfo& inf);
	void openScreen(const std::string& src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input);
	void closeScreen(int id);

	~WinDisplay();
private:
	std::vector<int> getSessions();
	size_t parseInt(const std::string& str, size_t p, const std::string& delim, uint32_t& res);
	Rd::ScreenInfo parseResolutions(const std::string& data);
	void processLoop();
private:
	std::mutex _mut, _mut1;
	WinProcess _process;
	bool _opening;
	std::condition_variable _cv;
	std::thread _thr;
	int _screenId;

	//const std::string _app = "C:\\Users\\user\\source\\repos\\scr\\Release\\scr.exe";
	const std::string _app = "scr.exe";
	std::string _args;
};
