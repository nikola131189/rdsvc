#pragma once
#include "../Native.h"
#include "WinDisplay.h"
#include "WinAction.h"
#include "WinClipBoard.h"
#include "WinScreenCapture.h"
#include "WinEventLoop.h"
#include "../Log.h"
class WinSystem : public Rd::Native::System
{
public:
	std::string terminalApp();
	~WinSystem() {}
};




class WinNative : public Rd::Native
{
public:
	WinNative();
	Display& display() { return _display; }
	Action& action() { return _action; }
	System& system() { return _system; }
	ClipBoard& clipBoard() { return _clipBoard; }
	ScreenCapture& screenCapture() { return _screenCapture; }
	EventLoop& eventLoop() { return _eventLoop; }
private:
	WinDisplay _display;
	WinAction _action;
	WinSystem _system;
	WinClipBoard _clipBoard;
	WinScreenCapture _screenCapture;
	WinEventLoop _eventLoop;
private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void Wineventproc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
		LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime);

	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);


	static WinNative* inst;
	HWND hwndNextViewer;
};
