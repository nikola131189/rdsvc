#include "WinEventLoop.h"

WinEventLoop::WinEventLoop(WINEVENTPROC eventProc, WNDPROC wndProc, HOOKPROC keybdProc, HOOKPROC mouseProc) :
	_eventProc(eventProc),
	_wndProc(wndProc),
	_keybdProc(keybdProc),
	_mouseProc(mouseProc)

{
}


void WinEventLoop::loop()
{
	HDESK hdesk = OpenInputDesktop(0, true, GENERIC_ALL);
	SetThreadDesktop(hdesk);
	CloseDesktop(hdesk);

	/*HWND hwnd_monitor = FindWindow(0, 0);
	SendMessage(hwnd_monitor, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
	Sleep(10000);*/

	mouse_event(MOUSEEVENTF_MOVE, 0, 1, 0, NULL);
	Sleep(40);
	mouse_event(MOUSEEVENTF_MOVE, 0, -1, 0, NULL);

	
	HINSTANCE hInstance = GetModuleHandle(NULL);

	SetWinEventHook(0, 0x0020, 0, _eventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
	SetWindowsHookEx(WH_KEYBOARD_LL, _keybdProc, hInstance, 0);
	SetWindowsHookEx(WH_MOUSE_LL, _mouseProc, hInstance, 0);

	

	static const wchar_t* class_name = L"DUMMY_CLASS";
	WNDCLASSEX wx = {};
	wx.cbSize = sizeof(WNDCLASSEX);
	wx.lpfnWndProc = _wndProc;
	wx.hInstance = hInstance;
	wx.lpszClassName = class_name;


	if (RegisterClassEx(&wx)) {
		HWND hwnd = CreateWindowEx(0, class_name, L"dummy_name", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
		BOOL bRetVal;
		MSG msg;
		while ((bRetVal = GetMessage(&msg, NULL, 0, 0)) != 0)
		{
			if (bRetVal == -1)
			{
				exit(0);
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

}


void WinEventLoop::blockInput(bool block)
{
	_blockInput = block;
}


bool WinEventLoop::isBlockInput()
{
	return _blockInput;
}




