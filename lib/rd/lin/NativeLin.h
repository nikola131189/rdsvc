#pragma once
#include "../Native.h"
#include "LinDisplay.h"
#include "LinAction.h"
#include "LinClipBoard.h"
#include "LinScreenCapture.h"
#include "LinEventLoop.h"
#include "../Log.h"
class LinSystem : public Rd::Native::System
{
public:
	std::string terminalApp();
	~LinSystem() {}
};




class LinNative : public Rd::Native
{
public:
	LinNative();
	Display& display() { return _display; }
	Action& action() { return _action; }
	System& system() { return _system; }
	ClipBoard& clipBoard() { return _clipBoard; }
	ScreenCapture& screenCapture() { return _screenCapture; }
	EventLoop& eventLoop() { return _eventLoop; }
private:
	LinDisplay _display;
	LinAction _action;
	LinSystem _system;
	LinClipBoard _clipBoard;
	LinScreenCapture _screenCapture;
	LinEventLoop _eventLoop;
private:

};
