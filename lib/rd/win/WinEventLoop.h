#pragma once
#include "../Native.h"
#include <functional>
#include <windows.h>
#include <cstdint>
#include <thread>
#include "../Log.h"

class WinEventLoop : public Rd::Native::EventLoop
{
public:
	WinEventLoop(WINEVENTPROC eventProc, WNDPROC wndProc, HOOKPROC keybdProc, HOOKPROC mouseProc);
	void loop();
	void blockInput(bool block);
	bool isBlockInput();
private:
	WINEVENTPROC _eventProc;
	WNDPROC _wndProc;
	HOOKPROC _keybdProc;
	HOOKPROC _mouseProc;
	bool _blockInput = false;
};

