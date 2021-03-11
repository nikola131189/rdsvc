#pragma once
#include "../Native.h"
#include "../keyboard/ScanCodeTranslator.h"
#include <windows.h>
#include "../Log.h"


class WinAction : public Rd::Native::Action
{
public:
	WinAction();
	~WinAction();
	void onMouse(const Rd::ActionEvent::Mouse& mouse);
	void onKeybd(const Rd::ActionEvent::Keybd& keybd);
};


