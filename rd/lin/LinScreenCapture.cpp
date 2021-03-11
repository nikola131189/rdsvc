#include "LinScreenCapture.h"















/*
ScreenCapture::ScreenCapture() : ximg(0)
{
}

void ScreenCapture::init(Display* disp, Window win)
{
	display = XOpenDisplay(nullptr);
	root = DefaultRootWindow(display);
	XWindowAttributes window_attributes;
	XGetWindowAttributes(display, root, &window_attributes);
	m_width = window_attributes.width; m_height = window_attributes.height;

}

bool ScreenCapture::frame(std::vector<uint8_t>& buf, int w, int h, int bpp)
{
	ximg = XGetImage(display, root, 0, 0, m_width, m_height, AllPlanes, ZPixmap);
	uint8_t* src = (uint8_t*)ximg->data;
	uint8_t* dst = &buf[0];

	for (size_t y = 0; y < ximg->height; y++)
	{
		for (size_t x = 0; x < ximg->width; x++)
		{
			dst[x * 3] = src[x * 4 + 2];
			dst[x * 3 + 1] = src[x * 4 + 1];
			dst[x * 3 + 2] = src[x * 4];
		}
		src += ximg->width * 4;
		dst += w * 3;
	}

	XDestroyImage(ximg);

	return 1;
}

void ScreenCapture::free()
{
	return;
	if (!ximg) return;
	XDestroyImage(ximg);
	XShmDetach(display, &shminfo);
	shmdt(shminfo.shmaddr);
	XCloseDisplay(display);
}



bool ScreenCapture::frame(std::vector<uint8_t>& buf, int xx, int yy, int w, int h, int bpp)
{

	//ximg = XShmCreateImage(display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen), ZPixmap, NULL, &shminfo, m_width, m_height);

	root = DefaultRootWindow(display);
	XShmGetImage(display, root, ximg, 0, 0, 0xffffff);

	//std::cout << xx << " " << yy << " " << ximg->width << " " << ximg->height << " " << ximg->bits_per_pixel << "\n";

	if (!ximg) return 0;
	if (!ximg->data) return 0;

	int ww = std::min(ximg->width, w);
	int hh = std::min(ximg->height, h);

	uint8_t* src = (uint8_t*)ximg->data + xx * 4 + yy * ximg->width * 4;
	uint8_t* dst = &buf[0];

	for (size_t y = 0; y < hh; y++)
	{
		for (size_t x = 0; x < ww; x++)
		{
			dst[x * 3] = src[x * 4 + 2];
			dst[x * 3 + 1] = src[x * 4 + 1];
			dst[x * 3 + 2] = src[x * 4];
		}
		src += ximg->width * 4;
		dst += w * 3;
	}

	//XDestroyImage(ximg);

	return 1;
}


void ScreenCapture::free()
{
	return;
	if (!ximg) return;
	XDestroyImage(ximg);
	XShmDetach(display, &shminfo);
	shmdt(shminfo.shmaddr);
}
*/




void LinScreenCapture::getCursor(Rd::CursorInfo& inf)
{
	//Window window_returned;
	//XQueryPointer(display, root)

	XFixesCursorImage* cursor = XFixesGetCursorImage(display);
	inf.xhot = cursor->xhot;
	inf.yhot = cursor->yhot;
	inf.x = cursor->x;
	inf.y = cursor->y;

	inf.width = cursor->width;
	inf.height = cursor->height;
	inf.visible = true;
	
	

	if (cursor->pixels && cursor_serial != cursor->cursor_serial)
	{
		inf.data.resize(inf.width * inf.height * 4);
		uint8_t* dest = inf.data.data();
		uint8_t* src = (uint8_t* )cursor->pixels;

		unsigned char r, g, b, a;
		unsigned short row, col, pos;

		for (pos = row = 0; row < cursor->height; row++)
		{
			for (col = 0; col < cursor->width; col++, pos++)
			{
				a = (unsigned char)((cursor->pixels[pos] >> 24) & 0xff);
				r = (unsigned char)((cursor->pixels[pos] >> 16) & 0xff);
				g = (unsigned char)((cursor->pixels[pos] >> 8) & 0xff);
				b = (unsigned char)((cursor->pixels[pos] >> 0) & 0xff);

				*(dest + 0) = a;
				*(dest + 1) = r;
				*(dest + 2) = g;
				*(dest + 3) = b;
				dest += 4;
			}
		}

		cursor_serial = cursor->cursor_serial;
	}
}

LinScreenCapture::LinScreenCapture() : ximg(0)
{
}

LinScreenCapture::~LinScreenCapture()
{
	if (!ximg) return;
	XDestroyImage(ximg);
	XShmDetach(display, &shminfo);
	shmdt(shminfo.shmaddr);
}

bool LinScreenCapture::init(uint32_t width, uint32_t height)
{
	display = XOpenDisplay(0);
	if (!display)
	{
		std::cout << "XOpenDisplay err\n";
		return 0;
	}

	root = DefaultRootWindow(display);
	XWindowAttributes window_attributes;
	XGetWindowAttributes(display, root, &window_attributes);
	m_width = width; m_height = height;
	ww = std::min(m_width, window_attributes.width);
	hh = std::min(m_height, window_attributes.height);

	screen = window_attributes.screen;


	printf("_useShm %d\n", _useShm);
	config_reader cfg;
	cfg.readFile("configX11");
	_useShm = cfg.find<bool>("use_shm");

	printf("_useShm %d\n", _useShm);





	if (_useShm)
	{
		ximg = XShmCreateImage(display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen), ZPixmap, NULL, &shminfo, window_attributes.width, window_attributes.height);
		shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT | 0777);
		shminfo.shmaddr = ximg->data = (char*)shmat(shminfo.shmid, 0, 0);
		shminfo.readOnly = False;
		if (shminfo.shmid < 0)
			printf("Fatal shminfo error!\n");
		if (!XShmAttach(display, &shminfo))
			_useShm = false;

		printf("_useShm %d\n", _useShm);
	}

	return 1;
}

int LinScreenCapture::frame(uint8_t* data, uint32_t size, Rd::CursorInfo& inf)
{
	
	if (_useShm)
	{
		XShmGetImage(display, root, ximg, 0, 0, 0xffffff);
	}
	else
	{
		ximg = XGetImage(display, root, 0, 0, ww, hh, AllPlanes, ZPixmap);
	}

	
	//std::cout << hh << " " << ww << " " << ximg->width << " " << ximg->height << " " << ximg->bits_per_pixel << "\n";

	if (!ximg)
	{
		return 0;
	}
	if (!ximg->data)
	{
		return 0;
	}


	uint8_t* src = (uint8_t*)ximg->data;
	uint8_t* dst = data;

	for (size_t y = 0; y < hh; y++)
	{
		for (size_t x = 0; x < ww; x++)
		{
			dst[x * 3] = src[x * 4 + 2];
			dst[x * 3 + 1] = src[x * 4 + 1];
			dst[x * 3 + 2] = src[x * 4];
		}
		src += ximg->width * 4;
		dst += m_width * 3;
	}

	if(!_useShm)
		XDestroyImage(ximg);

	getCursor(inf);

	return 1;
}

void LinScreenCapture::setResolution(uint32_t w, uint32_t h)
{
	int sid = 0;
	Display* dpy = XOpenDisplay(0);
	if (!dpy) return;
	Window root = RootWindow(dpy, 0);
	Rotation original_rotation;

	int id = -1;
	int freq = -1;




	int num_sizes;
	XRRScreenSize* xrrs = XRRSizes(dpy, 0, &num_sizes);

	for (int i = 0; i < num_sizes; i++) {
		short* rates;
		int     num_rates;

		rates = XRRRates(dpy, 0, i, &num_rates);

		if (xrrs[i].width == w && xrrs[i].height == h) {
			freq = rates[0]; id = i;
		}

	}
	if (id != -1 || freq != -1)
	{
		XRRScreenConfiguration* conf = XRRGetScreenInfo(dpy, root);


		short original_rate = XRRConfigCurrentRate(conf);
		SizeID original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);
		//std::cout << original_size_id << " " << id << "\n";
		if (original_size_id != id)
			XRRSetScreenConfigAndRate(dpy, conf, root, id, RR_Rotate_0, freq, CurrentTime);
	}
	XCloseDisplay(dpy);
}
