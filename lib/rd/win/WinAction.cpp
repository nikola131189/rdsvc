#include "WinAction.h"


WinAction::WinAction()
{

}

WinAction::~WinAction()
{
}

void WinAction::onMouse(const Rd::ActionEvent::Mouse& ev)
{
	DWORD dwExtra = (DWORD)this;
	SetCursorPos(ev.x, ev.y);
	if (ev.pressed)
	{
		switch (ev.type)
		{
		case Rd::ActionEvent::Mouse::button_1:
			mouse_event(MOUSEEVENTF_LEFTDOWN, ev.x, ev.y, 0, dwExtra);
			break;
		case Rd::ActionEvent::Mouse::button_2:
			mouse_event(MOUSEEVENTF_RIGHTDOWN, ev.x, ev.y, 0, dwExtra);
			break;
		case Rd::ActionEvent::Mouse::button_3:
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, ev.x, ev.y, 0, dwExtra);
			break;
		case Rd::ActionEvent::Mouse::button_4:
			mouse_event(MOUSEEVENTF_XDOWN, ev.x, ev.y, XBUTTON1, dwExtra);
			break;
		case Rd::ActionEvent::Mouse::button_5:
			mouse_event(MOUSEEVENTF_XDOWN, ev.x, ev.y, XBUTTON2, dwExtra);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (ev.type)
		{
		case Rd::ActionEvent::Mouse::button_1:
			mouse_event(MOUSEEVENTF_LEFTUP, ev.x, ev.y, 0, dwExtra);
			break;
		case Rd::ActionEvent::Mouse::button_2:
			mouse_event(MOUSEEVENTF_RIGHTUP, ev.x, ev.y, 0, dwExtra);
			break;
		case Rd::ActionEvent::Mouse::button_3:
			mouse_event(MOUSEEVENTF_MIDDLEUP, ev.x, ev.y, 0, dwExtra);
			break;
		case Rd::ActionEvent::Mouse::button_4:
			mouse_event(MOUSEEVENTF_XUP, ev.x, ev.y, XBUTTON1, dwExtra);
			break;
		case Rd::ActionEvent::Mouse::button_5:
			mouse_event(MOUSEEVENTF_XUP, ev.x, ev.y, XBUTTON2, dwExtra);
		default:
			break;
		}
	}


	if (ev.type == Rd::ActionEvent::Mouse::wheel_forward)
	{
		mouse_event(MOUSEEVENTF_WHEEL, ev.x, ev.y, 120, dwExtra);
	}

	if (ev.type == Rd::ActionEvent::Mouse::wheel_backward)
	{
		mouse_event(MOUSEEVENTF_WHEEL, ev.x, ev.y, -120, dwExtra);
	}

}

void WinAction::onKeybd(const Rd::ActionEvent::Keybd& ev)
{

	/*uint8_t vk = ScanCodeTranslator::sdlToSystem((SDL_Scancode)ev.key);
	UINT sc = MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
	DWORD flags = 0;
	if (ev.extended)
		flags |= KEYEVENTF_EXTENDEDKEY;
	if(!ev.pressed)
		flags |= KEYEVENTF_KEYUP;
	keybd_event(vk, sc, flags, 10);*/


	uint8_t k = ScanCodeTranslator::sdlToSystem((SDL_Scancode)ev.key);
	bool pressed = ev.pressed;
	//MessageBox(0, std::to_wstring(keyUp).c_str(), L"gfs", MB_OK);
	//if (k == VK_RSHIFT || k == VK_LSHIFT) k = VK_SHIFT;
	INPUT inp;
	inp.type = INPUT_KEYBOARD;
	inp.ki.wVk = k;
	inp.ki.wScan = MapVirtualKey(k, MAPVK_VK_TO_VSC); ;
	inp.ki.dwFlags = 0;
	if (!pressed)
		inp.ki.dwFlags |= KEYEVENTF_KEYUP;

	if (ev.extended)
		inp.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;

	inp.ki.time = 0;
	inp.ki.dwExtraInfo = (uint32_t)this;
	
	UINT i = SendInput(1, &inp, sizeof(inp));
}


