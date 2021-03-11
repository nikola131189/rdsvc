#include "LinAction.h"


LinAction::LinAction()
{

}

LinAction::~LinAction()
{
}

void LinAction::onMouse(const Rd::ActionEvent::Mouse& ev)
{
	if (!m_display)
		return;
	switch (ev.type)
	{
	case Rd::ActionEvent::Mouse::move:
		XSelectInput(m_display, m_root, KeyReleaseMask);
		XWarpPointer(m_display, None, m_root, 0, 0, 0, 0, ev.x, ev.y);
		XFlush(m_display);
		break;
	case Rd::ActionEvent::Mouse::button_1:
		XTestFakeButtonEvent(m_display, 1, ev.pressed, CurrentTime);
		XFlush(m_display);
		break;
	case Rd::ActionEvent::Mouse::button_2:
		XTestFakeButtonEvent(m_display, 3, ev.pressed, CurrentTime);
		XFlush(m_display);
		break;
	case Rd::ActionEvent::Mouse::button_3:
		XTestFakeButtonEvent(m_display, 2, ev.pressed, CurrentTime);
		XFlush(m_display);
		break;
	case Rd::ActionEvent::Mouse::button_4:
		XTestFakeButtonEvent(m_display, 8, ev.pressed, CurrentTime);
		XFlush(m_display);
		break;
	case Rd::ActionEvent::Mouse::button_5:
		XTestFakeButtonEvent(m_display, 9, ev.pressed, CurrentTime);
		XFlush(m_display);
		break;
	/*case Rd::ActionEvent::Mouse::button_6:
		XTestFakeButtonEvent(m_display, 8, ev.pressed, CurrentTime);
		XFlush(m_display);
		break;*/

	case Rd::ActionEvent::Mouse::wheel_forward:
		XTestFakeButtonEvent(m_display, 4, True, 10);
		XTestFakeButtonEvent(m_display, 4, False, CurrentTime);
		XFlush(m_display);
		break;
	case Rd::ActionEvent::Mouse::wheel_backward:
		XTestFakeButtonEvent(m_display, 5, True, 10);
		XTestFakeButtonEvent(m_display, 5, False, CurrentTime);
		XFlush(m_display);
		break;
	default:
		break;
	}
}

void LinAction::onKeybd(const Rd::ActionEvent::Keybd& ev)
{
	if (!m_display)
		return;
	uint32_t code = ScanCodeTranslator::sdlToSystem((SDL_Scancode)ev.key);
	if (!code) return;
	XTestFakeKeyEvent(m_display, code, ev.pressed, CurrentTime);
	XFlush(m_display);
}

void LinAction::init()
{
	m_display = XOpenDisplay(":0.0");
	if (m_display)
		m_root = DefaultRootWindow(m_display);
}


