#include "WinDisplay.h"

WinDisplay::WinDisplay() :
	_opening(false), _thr(&WinDisplay::processLoop, this)
{
}

WinDisplay::~WinDisplay()
{
}

void WinDisplay::getDisplay(Rd::DisplayInfo& inf)
{
	auto sess = getSessions();
	for (auto it : sess)
	{
		HANDLE hStdIn, hStdOut, hStdErr;
		WinProcess proc;
		proc.exec("screenModes.exe", "", it, hStdIn, hStdOut, hStdErr);

		std::string str;
		DWORD dwRead;
		CHAR chBuf[1024];
		BOOL bSuccess = FALSE;
		for (;;)
		{
			bSuccess = ReadFile(hStdOut, chBuf, 1024, &dwRead, NULL);
			if (!bSuccess || dwRead == 0) break;
			str.append(chBuf, dwRead);
		}

		Rd::ScreenInfo s = parseResolutions(str);
		s.id = it;
		inf.screens.push_back(s);
	}



}

void WinDisplay::openScreen(const std::string& src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input)
{
	{
		std::unique_lock<std::mutex> lk(_mut1);
		std::stringstream ss;
		ss << "--src" << " " << src << " "
			<< "--dest" << " " << dest << " "

			<< "--screen_id" << " " << id << " "
			<< "--encoding_id" << " " << (int)fmt.id << " "
			<< "--bit_rate" << " " << fmt.bitarte << " "

			<< "--w" << " " << fmt.w << " "
			<< "--h" << " " << fmt.h << " "
			<< "--dw" << " " << fmt.dw << " "
			<< "--dh" << " " << fmt.dh << " "
			<< "--profile" << " " << fmt.profile << " "
			<< "--quantization" << " " << fmt.quantization << " "
			<< "--thread_count" << " " << fmt.threadCount << " "
			<< "--block_input" << " " << block_input << " ";


		_args = ss.str();
	}
	_screenId = id;
	_opening = true;
	_cv.notify_one();
}

void WinDisplay::closeScreen(int id)
{
	std::unique_lock<std::mutex> lk(_mut1);
	_opening = false;
	_process.terminate();
	_cv.notify_one();
}





std::vector<int> WinDisplay::getSessions()
{
	std::vector<int> res;

	PWTS_SESSION_INFOW pSessionInfo = NULL;
	DWORD sessCount = 0;
	WTSEnumerateSessionsW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &sessCount);

	for (DWORD i = 0; i < sessCount; ++i)
	{
		res.push_back(pSessionInfo[i].SessionId);
	}
	return res;
}



size_t WinDisplay::parseInt(const std::string& str, size_t p, const std::string& delim, uint32_t& res)
{
	size_t pp = str.find(delim, p);
	if (pp == std::string::npos)
		return std::string::npos;

	std::string line(str, p, pp - p);
	res = std::atoi(line.c_str());
	return pp + 1;
}


Rd::ScreenInfo WinDisplay::parseResolutions(const std::string& data)
{
	Rd::ScreenInfo out;

	size_t pos = 0;
	pos = parseInt(data, pos, "x", out.current.width);
	pos = parseInt(data, pos, "\n", out.current.height);

	while (pos != std::string::npos)
	{
		Rd::Resolution r;
		pos = parseInt(data, pos, "x", r.width);
		pos = parseInt(data, pos, "\n", r.height);
		if (r.width && r.height)
			out.resolutions.push_back(r);
	}
	return out;
}



void WinDisplay::processLoop()
{
	while (1)
	{
		std::unique_lock<std::mutex> lk(_mut);
		if (_opening)
		{
			std::string str;
			{
				std::unique_lock<std::mutex> lk(_mut1);
				str = _args;
			}
			_process.exec(_app, str, _screenId);
			_process.wait();
		}
		else
		{
			_cv.wait(lk);
		}
	}

}

