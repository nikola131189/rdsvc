#include "LinApi.h"

LinApi::LinApi()

{




}

void LinApi::getDisplay(rd::DisplayInfo& inf)
{
	std::string str;
	bool res = getResolutions(str);
	inf.screens = parseResolutions(str);
}

void LinApi::openScreen(rd::id_t src, rd::id_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input)
{
	//const char* comm = "/home/user/projects/scr/bin/x64/Release/scr.out";
	const char* comm = "scr";
	int pid = fork();
	if (pid == 0) {
		setgid(100);
		setuid(1000);

		//putenv("HOME=/home/user");
		putenv("DISPLAY=:0");

		execl(comm, 
			"--src", id_to_str(src).c_str(),
			"--dest", id_to_str(dest).c_str(),
			"--display", " ",
			"--screen_id", std::to_string(id).c_str(),
			"--encoding_id", std::to_string((int)fmt.id).c_str(),
			"--bit_rate", std::to_string(fmt.bitarte).c_str(),
			"--w", std::to_string(fmt.w).c_str(),
			"--h", std::to_string(fmt.h).c_str(),
			"--dw", std::to_string(fmt.dw).c_str(),
			"--dh", std::to_string(fmt.dh).c_str(),
			 "--block_input", "0",
			"--profile", std::to_string(fmt.profile).c_str(), NULL);


		//exit(1);
	}
	if (pid > 0) {
		_pid = pid;
	}

	if (pid == -1) {
	}
}

void LinApi::closeScreen(int id)
{
	if (_pid)
	{
		kill(_pid, SIGKILL);
	}
	_pid = 0;
}

std::string LinApi::terminalApp()
{
	return "/bin/bash";
}

std::string LinApi::computerName()
{
	return boost::asio::ip::host_name();
}


bool LinApi::getResolutions(std::string& res)
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

size_t LinApi::parseInt(const std::string& str, size_t p, int& res)
{
	size_t pp = str.size();

	std::string line(str, p, pp - p);
	res = std::atoi(line.c_str());
	return pp + 1;
}



size_t LinApi::parseInt(const std::string& str, size_t p, const std::string& delim, int& res)
{
	size_t pp = str.find(delim, p);
	if (pp == std::string::npos)
		return std::string::npos;

	std::string line(str, p, pp - p);
	res = std::atoi(line.c_str());
	return pp + 1;
}

std::vector<rd::ScreenInfo> LinApi::parseResolutions(const std::string& data)
{
	std::vector<rd::ScreenInfo> out;
	size_t pos = 0;
	size_t first = 0;
	rd::ScreenInfo* scr;

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
				rd::Resolution m;
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



std::string LinApi::id_to_str(rd::id_t id) const
{

	boost::uuids::uuid u;
	memcpy(&u, &id, 16);

	return boost::lexical_cast<std::string>(u);
}

/*
void lin_api::get_resolutions(ScreenInfo* scr, Display* dis, int id)
{
	XRRScreenResources* screen;
	XRRCrtcInfo* crtc_info;
	screen = XRRGetScreenResources(dis, DefaultRootWindow(dis));

	for (int j = 0; j < screen->nmode; j++)
	{
		auto r = scr->mutable_resolutions()->Add();
		r->set_width(screen->modes[j].width); r->set_height(screen->modes[j].height);

	}
}

void lin_api::frame()
{
	if (!m_running) return;
	m_capture.Frame(m_buffer1, m_fmt.width(), m_fmt.height(), 3);
	uint32_t s = 0;
	m_encoder->encode(&m_buffer1[0], m_buffer1.size(), &m_buffer2[0], s);

	rd::event_ptr<VideoEvent> ev;
	ev->mutable_video_frame()->mutable_video_format()->CopyFrom(m_fmt);
	ev->mutable_video_frame()->set_data(m_buffer2.data(), s);
	ev->set_screen_id(m_sid);


	rd::inet::send(m_dest, ev, [this]() {
		if (!m_running) return;
		m_ios.post([this]() { frame(); });
	});
}




void lin_api::OnMouse(const MouseEvent& ev)
{


	switch (ev.type())
	{
	case MouseEvent_Type_MOVE:
		XSelectInput(display, m_root, KeyReleaseMask);
		XWarpPointer(display, None, m_root, 0, 0, 0, 0, ev.x(), ev.y());
		XFlush(display);
		break;
	case MouseEvent_Type_LBUTTON_DOWN:
		XTestFakeButtonEvent(display, 1, True, CurrentTime);
		XFlush(display);
		break;
	case MouseEvent_Type_LBUTTON_UP:
		XTestFakeButtonEvent(display, 1, False, CurrentTime);
		XFlush(display);
		break;
	case MouseEvent_Type_RBUTTON_DOWN:
		XTestFakeButtonEvent(display, 3, True, CurrentTime);
		XFlush(display);
		break;
	case MouseEvent_Type_RBUTTON_UP:
		XTestFakeButtonEvent(display, 3, False, CurrentTime);
		XFlush(display);
		break;
	case MouseEvent_Type_MBUTTON_DOWN:
		XTestFakeButtonEvent(display, 2, True, CurrentTime);
		XFlush(display);
		break;
	case MouseEvent_Type_MBUTTON_UP:
		XTestFakeButtonEvent(display, 2, False, CurrentTime);
		XFlush(display);
		break;

	case MouseEvent_Type_WHEEL_FORWARD:
		XTestFakeButtonEvent(display, 4, True, 10);
		XTestFakeButtonEvent(display, 4, False, CurrentTime);
		XFlush(display);
		break;
	case MouseEvent_Type_WHEEL_BACKWARD:
		XTestFakeButtonEvent(display, 5, True, 10);
		XTestFakeButtonEvent(display, 5, False, CurrentTime);
		XFlush(display);
		break;
	default:
		break;
	}
}



void lin_api::OnKeybd(const KeybdEvent& ev)
{
	
	uint32_t code = sdl_scancode_X11_code_table[ev.key()];
	if (!code) return;
	XTestFakeKeyEvent(display, code, ev.pressed(), CurrentTime);
	XFlush(display);
}

*/




