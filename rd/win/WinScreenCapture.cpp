#include "WinScreenCapture.h"

WinScreenCapture::WinScreenCapture()
{
	_isGdi = !IsWindows8OrGreater();
}

WinScreenCapture::~WinScreenCapture()
{
}

bool WinScreenCapture::init(uint32_t width, uint32_t height)
{
	_width = width; _height = height;

	if (_isGdi)
	{
		return 1;
	}
	else
	{
		for (size_t i = 0; i < 20; i++)
		{
			bool res = _screenReader.init(width, height);
			if (res) return 1;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		_isGdi = true;

		return 1;
	}
	
}


int WinScreenCapture::frame(uint8_t* data, uint32_t size, Rd::CursorInfo& inf)
{
	if (_isGdi)
	{
		grab_desktop_GDI(data, 0, 0, _width, _height);
		return 1;
	}
	else
	{
		shape_t s;
		int res = _screenReader.frame(data, s);
		inf.data = s.data;
		inf.width = s.width;
		inf.height = s.height;
		inf.xhot = s.hot_spot.x;
		inf.yhot = s.hot_spot.y;
		inf.x = s.pos.x;
		inf.y = s.pos.y;
		inf.visible = s.visible;
		return res;
	}
}



void WinScreenCapture::setResolution(uint32_t w, uint32_t h)
{
	
	DEVMODE devmode;
	devmode.dmPelsWidth = w;
	devmode.dmPelsHeight = h;
	devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	devmode.dmSize = sizeof(DEVMODE);
	long result = ChangeDisplaySettings(&devmode, 0);
}



void  WinScreenCapture::grab_desktop_GDI(uint8_t* out, int x, int y, int width, int height)
{

	RECT r;
	HWND hwnd = 0;
	HDC win_dc = GetDC(hwnd);
	//HDC win_dc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

	HDC dc = CreateCompatibleDC(win_dc);
	HBITMAP bmp = CreateCompatibleBitmap(win_dc, width, height);
	SelectObject(dc, bmp);


	BitBlt(dc, 0, 0, width, height, win_dc, 0, 0, SRCCOPY | CAPTUREBLT);
	//StretchBlt(dc, 0, win.h, win.w, -win.h, win_dc, 0, 0, win.w, win.h, SRCCOPY);
	//PrintWindow(hwnd, dc, 0);


	BITMAPINFO bminfo;
	bminfo.bmiHeader.biBitCount = 24;
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bminfo.bmiHeader.biCompression = BI_RGB;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biWidth = width;
	bminfo.bmiHeader.biHeight = -height;
	bminfo.bmiHeader.biSizeImage = width * 3 * height; // must be DWORD aligned
	bminfo.bmiHeader.biXPelsPerMeter = 0;
	bminfo.bmiHeader.biYPelsPerMeter = 0;
	bminfo.bmiHeader.biClrUsed = 0;
	bminfo.bmiHeader.biClrImportant = 0;
	int res = GetDIBits(dc, bmp, 0, height, out, &bminfo, DIB_RGB_COLORS);




	for (int i = 0; i < width * height * 3; i += 3)
	{
		uint8_t tmp = out[i];
		out[i] = out[i + 2];
		out[i + 2] = tmp;
	}



	DeleteObject(bmp);
	DeleteDC(win_dc);
	DeleteDC(dc);
}

