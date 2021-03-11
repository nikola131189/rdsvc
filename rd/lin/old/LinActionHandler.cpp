#include "LinActionHandler.h"

LinActionHandler::LinActionHandler()
{
	m_display = XOpenDisplay(":0.0");
	m_root = DefaultRootWindow(m_display);
}

LinActionHandler::~LinActionHandler()
{
}

void LinActionHandler::onMouse(const rd::ActionEvent::Mouse& mouse)
{
	
	switch (mouse.type)
	{
	case rd::ActionEvent::Mouse::MOVE:
		XSelectInput(m_display, m_root, KeyReleaseMask);
		XWarpPointer(m_display, None, m_root, 0, 0, 0, 0, mouse.x, mouse.y);
		XFlush(m_display);
		break;
	case rd::ActionEvent::Mouse::LBUTTON_DOWN:
		XTestFakeButtonEvent(m_display, 1, True, CurrentTime);
		XFlush(m_display);
		break;
	case rd::ActionEvent::Mouse::LBUTTON_UP:
		XTestFakeButtonEvent(m_display, 1, False, CurrentTime);
		XFlush(m_display);
		break;
	case rd::ActionEvent::Mouse::RBUTTON_DOWN:
		XTestFakeButtonEvent(m_display, 3, True, CurrentTime);
		XFlush(m_display);
		break;
	case rd::ActionEvent::Mouse::RBUTTON_UP:
		XTestFakeButtonEvent(m_display, 3, False, CurrentTime);
		XFlush(m_display);
		break;
	case rd::ActionEvent::Mouse::MBUTTON_DOWN:
		XTestFakeButtonEvent(m_display, 2, True, CurrentTime);
		XFlush(m_display);
		break;
	case rd::ActionEvent::Mouse::MBUTTON_UP:
		XTestFakeButtonEvent(m_display, 2, False, CurrentTime);
		XFlush(m_display);
		break;

	case rd::ActionEvent::Mouse::WHEEL_FORWARD:
		XTestFakeButtonEvent(m_display, 4, True, 10);
		XTestFakeButtonEvent(m_display, 4, False, CurrentTime);
		XFlush(m_display);
		break;
	case rd::ActionEvent::Mouse::WHEEL_BACKWARD:
		XTestFakeButtonEvent(m_display, 5, True, 10);
		XTestFakeButtonEvent(m_display, 5, False, CurrentTime);
		XFlush(m_display);
		break;
	default:
		break;
	}
}

void LinActionHandler::onKeybd(const rd::ActionEvent::Keybd& keybd)
{
	uint32_t code = ScanCodeTranslator::sdlToSystem((SDL_Scancode)keybd.key);
	if (!code) return;
	XTestFakeKeyEvent(m_display, code, keybd.pressed, CurrentTime);
	XFlush(m_display);
}

void LinActionHandler::blockInput(bool block)
{
}
