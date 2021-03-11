#include "LinDisplay.h"

LinDisplay::LinDisplay()
{
}

LinDisplay::~LinDisplay()
{
}

void LinDisplay::getDisplay(Rd::DisplayInfo& inf)
{
	
	std::string str;
	bool res = getResolutions(str);
	inf.screens = parseResolutions(str);

}

void LinDisplay::openScreen(const std::string& src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input)
{
	forkScreen(src, dest, disp, id, fmt, block_input);
}

void LinDisplay::closeScreen(int id)
{
	if (_pid)
	{
		kill(_pid, SIGKILL);
	}
	_pid = 0;
}




bool LinDisplay::getResolutions(std::string& res)
{
	//const char* comm = "/home/user/projects/resolutions/bin/x64/Release/resolutions.out";
	const char* comm = "resolutions";
	int my_pipe[2];
	pipe(my_pipe);

	int child = fork();
	if (child == 0) {
		setgid(100);
		setuid(1000);

		//putenv((char*)usr.c_str());
		//putenv("HOME=/home/user");
		putenv("DISPLAY=:0");

		close(my_pipe[0]);
		dup2(my_pipe[1], 1);

		execl(comm, " ", NULL);
		exit(1);
	}

	if (child > 0) {
		waitpid(child, 0, 0);
		close(my_pipe[1]);

		char reading_buf[1];

		while (read(my_pipe[0], reading_buf, 1) > 0)
		{
			res.push_back(reading_buf[0]);
			//printf(reading_buf);
		}

		close(my_pipe[0]);

		if (res.empty()) return false;
		return true;
	}

	if (child == -1) {

	}

	return false;
}

size_t LinDisplay::parseInt(const std::string& str, size_t p, int& res)
{
	size_t pp = str.size();

	std::string line(str, p, pp - p);
	res = std::atoi(line.c_str());
	return pp + 1;
}



size_t LinDisplay::parseInt(const std::string& str, size_t p, const std::string& delim, int& res)
{
	size_t pp = str.find(delim, p);
	if (pp == std::string::npos)
		return std::string::npos;

	std::string line(str, p, pp - p);
	res = std::atoi(line.c_str());
	return pp + 1;
}

std::vector<Rd::ScreenInfo> LinDisplay::parseResolutions(const std::string& data)
{
	std::vector<Rd::ScreenInfo> out;
	size_t pos = 0;
	size_t first = 0;
	Rd::ScreenInfo* scr;

	while (pos != std::string::npos)
	{
		pos = data.find("\n", first);
		std::string line(data, first, pos - first);
		{

			size_t p = line.find("screen");
			if (p != std::string::npos)
			{
				out.emplace_back(); scr = &out[out.size() - 1];
				p += 7;
				int v;
				p = parseInt(line, p, " ", v); //scr->x = v;
				p = parseInt(line, p, " ", v); //scr->y = v;
				p = parseInt(line, p, "x", v); scr->current.width = v;
				p = parseInt(line, p, v); scr->current.height = v;
			}


			p = line.find("mode");
			if (p != std::string::npos)
			{
				Rd::Resolution m;
				p += 4;
				int v;
				p = parseInt(line, p, "x", v); m.width = v;
				p = parseInt(line, p, v); m.height = v;
				scr->resolutions.push_back(m);
			}

		}
		first = pos + 1;
	}
	return out;
}

void LinDisplay::forkScreen(const std::string& src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input)
{
	const char* comm = "scr";
	//const char* comm = "/home/user/projects/scrLin/scrLin/scrLin/bin/x64/Release/scrLin.out";
//	signal(SIGCHLD, SIG_IGN);
	int pid = fork();
	if (pid == 0) {
		setgid(100);
		setuid(1000);

		//putenv("HOME=/home/user");
		putenv("DISPLAY=:0");

		execl(comm,
			"--src", src.c_str(),
			"--dest", std::to_string(dest).c_str(),
			"--screen_id", std::to_string(id).c_str(),
			"--encoding_id", std::to_string((int)fmt.id).c_str(),
			"--bit_rate", std::to_string(fmt.bitarte).c_str(),
			"--w", std::to_string(fmt.w).c_str(),
			"--h", std::to_string(fmt.h).c_str(),
			"--dw", std::to_string(fmt.dw).c_str(),
			"--dh", std::to_string(fmt.dh).c_str(),
			"--profile", std::to_string(fmt.profile).c_str(),
			"--quantization", std::to_string(fmt.quantization).c_str(),
			"--thread_count", std::to_string(fmt.threadCount).c_str(),
			"--block_input", std::to_string(block_input).c_str(),
			NULL);


		exit(1);
	}
	if (pid > 0) {
		_pid = pid;
	}

	if (pid == -1) {
	}
}
