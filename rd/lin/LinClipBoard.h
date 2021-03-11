#pragma once
#include "../Native.h"

#include "../utility/utility.h"
#include "../Log.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <thread>
#include <X11/extensions/Xfixes.h>
#include <X11/Xmu/Atoms.h>

class LinClipBoard : public Rd::Native::ClipBoard
{
public:
	LinClipBoard();
	~LinClipBoard();
	void setCbck(const ClipBoardCbck& cbck);
	void setData(const std::string& str);
private:
	void loop();
	Bool getSelection(Display* display, Window window, const char* bufname, const char* fmtname, std::string& res);
private:

	ClipBoardCbck _cbck = nullptr;
	std::thread _thr;

	Display* display;
	Window window;
	Atom targets_atom, text_atom, UTF8;

	Atom selection;

	std::string _buffer;
};
