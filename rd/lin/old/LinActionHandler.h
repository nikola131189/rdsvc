#pragma once

#include "../AbstractActionHandler.h"
#include <X11/extensions/XTest.h>
#include<X11/extensions/Xrandr.h>
#include <rd/keyboard/ScanCodeTranslator.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


class LinActionHandler : public AbstractActionHandler
{
public:
	LinActionHandler();
	~LinActionHandler();
	void onMouse(const rd::ActionEvent::Mouse& mouse);
	void onKeybd(const rd::ActionEvent::Keybd& keybd);
	void blockInput(bool block);
private:
	Display* m_display;
	Window m_root;
};





