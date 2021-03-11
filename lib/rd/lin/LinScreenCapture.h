#pragma once
#include <vector>
#include <cstdint>
#include "../Native.h"
#include <iostream>
#include <sys/shm.h>
#include <algorithm>


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/extensions/XShm.h>

#include <X11/extensions/XTest.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xfixes.h> 



class LinScreenCapture : public Rd::Native::ScreenCapture
{
public:
	LinScreenCapture();
	~LinScreenCapture();
	bool init(uint32_t width, uint32_t height);
	int frame(uint8_t* data, uint32_t size, Rd::CursorInfo& inf);
	void setResolution(uint32_t w, uint32_t h);
private:

	void getCursor(Rd::CursorInfo& inf);

private:
	int m_width, m_height;
	Display* display;
	Window root;
	XImage* ximg = 0;
	Screen* screen;
	XShmSegmentInfo shminfo;

	int ww, hh;
	unsigned long cursor_serial;

	bool _useShm = true;

};



