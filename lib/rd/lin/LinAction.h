#pragma once
#include "../Native.h"
#include "../Log.h"

#include <X11/extensions/XTest.h>
#include <X11/extensions/Xrandr.h>
#include "../keyboard/ScanCodeTranslator.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class LinAction : public Rd::Native::Action
{
public:
	LinAction();
	~LinAction();
	void onMouse(const Rd::ActionEvent::Mouse& mouse);
	void onKeybd(const Rd::ActionEvent::Keybd& keybd);
	void init();
private:
	Display* m_display = 0;
	Window m_root;
};


