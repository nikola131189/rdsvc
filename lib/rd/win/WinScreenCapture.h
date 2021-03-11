#pragma once
#include "../Native.h"
#include "desktopDublication/DXGI_screen_reader.h"
#include <VersionHelpers.h>

class WinScreenCapture : public Rd::Native::ScreenCapture
{
public:
	WinScreenCapture();
	~WinScreenCapture();
	bool init(uint32_t width, uint32_t height);
	int frame(uint8_t* data, uint32_t size, Rd::CursorInfo& inf);
	void setResolution(uint32_t w, uint32_t h);
private:
	DXGI_screen_reader _screenReader;
	uint32_t _width, _height;
	bool _isGdi;
private:
	void grab_desktop_GDI(uint8_t* out, int x, int y, int width, int height);
	
};

