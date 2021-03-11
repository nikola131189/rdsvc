#include "NativeWin.h"




std::string WinSystem::terminalApp()
{
	LPWKSTA_INFO_102 inf = NULL;
	NetWkstaGetInfo(NULL, 100, (LPBYTE*)&inf);

	std::string res = "C:\\Windows\\system32\\cmd.exe";

	if (inf != NULL)
	{
		if (inf->wki102_ver_major >= 10)
			res = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
		NetApiBufferFree(inf);
	}
	return res;
}



WinNative* WinNative::inst = 0;


WinNative::WinNative() :
	_eventLoop((WINEVENTPROC)&WinNative::Wineventproc, &WinNative::WindowProc,
		&WinNative::LowLevelKeyboardProc, &WinNative::LowLevelMouseProc)
{
	inst = this;
}



void WinNative::Wineventproc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime)
{
	if (event == EVENT_SYSTEM_DESKTOPSWITCH)
	{
		PostQuitMessage(0);
	}
}




LRESULT CALLBACK WinNative::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		inst->hwndNextViewer = SetClipboardViewer(hWnd);
		break;

	case WM_DRAWCLIPBOARD: {
		inst->_clipBoard.notify();
		SendMessage(inst->hwndNextViewer, uMsg, wParam, lParam);
		break;
	}
	default:
		break;
	}


	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT WinNative::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	PKBDLLHOOKSTRUCT pHook = (PKBDLLHOOKSTRUCT)lParam;

	if ((uint32_t)pHook->dwExtraInfo == (uint32_t)&inst->_action)
		return 0;
	
	return inst->_eventLoop.isBlockInput();
}

LRESULT WinNative::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSLLHOOKSTRUCT pHook = (PMSLLHOOKSTRUCT)lParam;

	if ((uint32_t)pHook->dwExtraInfo == (uint32_t)&inst->_action)
		return 0;

	return inst->_eventLoop.isBlockInput();
}