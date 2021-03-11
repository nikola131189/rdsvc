#include "LinClipBoard.h"


LinClipBoard::LinClipBoard()
{

}

LinClipBoard::~LinClipBoard()
{
}

void LinClipBoard::setCbck(const ClipBoardCbck& cbck)
{
	_cbck = cbck;
	_thr = std::thread(&LinClipBoard::loop, this);
	_thr.detach();
}


void LinClipBoard::setData(const std::string& str)
{
	_buffer = str;
	selection = XInternAtom(display, "CLIPBOARD", 0);
	Window owner;
	XSetSelectionOwner(display, selection, window, 0);
	if (XGetSelectionOwner(display, selection) != window) return;
}











void LinClipBoard::loop()
{
	XInitThreads();

	display = XOpenDisplay(0);
	if (!display)
		return;
	int N = DefaultScreen(display);
	window = XCreateSimpleWindow(display, RootWindow(display, N), 0, 0, 1, 1, 0,
		BlackPixel(display, N), WhitePixel(display, N));


	targets_atom = XInternAtom(display, "TARGETS", 0);
	text_atom = XInternAtom(display, "TEXT", 0);
	UTF8 = XInternAtom(display, "UTF8_STRING", 1);
	if (UTF8 == None) UTF8 = XA_STRING;
	




	Atom target,
		CLIPBOARD = XInternAtom(display, "CLIPBOARD", 0),
		XSEL_DATA = XInternAtom(display, "XSEL_DATA", 0);
	XConvertSelection(display, CLIPBOARD, UTF8, XSEL_DATA, window, CurrentTime);
	//XConvertSelection(display, CLIPBOARD, XA_STRING, XSEL_DATA, window, CurrentTime);
	//XSync(display, 0);


	const char* bufname = "CLIPBOARD";
	int event_base, error_base;

	Atom bufid = XInternAtom(display, bufname, False);

	
	if (!XFixesQueryExtension(display, &event_base, &error_base))
	{
		printf("No XFixes extension\n");
	}
	XFixesSelectSelectionInput(display, DefaultRootWindow(display), bufid, XFixesSetSelectionOwnerNotifyMask);



	XEvent event;
	while (1) {
		XNextEvent(display, &event);
		if (event.type == SelectionRequest)
		{
			//printf("SelectionRequest\n");
			unsigned char* text = (unsigned char*)_buffer.c_str();
			int size = _buffer.size();
			if (size)
			{
				if (event.xselectionrequest.selection != selection) break;
				XSelectionRequestEvent* xsr = &event.xselectionrequest;
				XSelectionEvent ev = { 0 };
				int R = 0;
				ev.type = SelectionNotify, ev.display = xsr->display, ev.requestor = xsr->requestor,
					ev.selection = xsr->selection, ev.time = xsr->time, ev.target = xsr->target, ev.property = xsr->property;
				if (ev.target == targets_atom) R = XChangeProperty(ev.display, ev.requestor, ev.property, XA_ATOM, 32,
					PropModeReplace, (unsigned char*)&UTF8, 1);
				else if (ev.target == XA_STRING || ev.target == text_atom)
					R = XChangeProperty(ev.display, ev.requestor, ev.property, XA_STRING, 8, PropModeReplace, text, size);
				else if (ev.target == UTF8)
					R = XChangeProperty(ev.display, ev.requestor, ev.property, UTF8, 8, PropModeReplace, text, size);
				else ev.property = None;
				if ((R & 2) == 0) XSendEvent(display, ev.requestor, 0, 0, (XEvent*)&ev);
			}
		}

		if (event.type == event_base + XFixesSelectionNotify &&
			((XFixesSelectionNotifyEvent*)&event)->selection == bufid)
		{
			//printf("XFixesSelectionNotify\n");
			std::string str;
			if (getSelection(display, window, bufname, "UTF8_STRING", str))
				if(_cbck) _cbck(str);
		}

		/*if (event.type == SelectionNotify)
		{
			printf("SelectionNotify\n");
		}
			char* data = 0;
			int format;
			unsigned long N, size;

			if (event.xselection.selection != CLIPBOARD) break;
			if (event.xselection.property) {
				XGetWindowProperty(event.xselection.display, event.xselection.requestor,
					event.xselection.property, 0L, (~0L), 0, AnyPropertyType, &target,
					&format, &size, &N, (unsigned char**)&data);
				if (target == UTF8 || target == XA_STRING) {
					
					//_cbck(std::string(data, size));

					XFree(data);
				}
				XDeleteProperty(event.xselection.display, event.xselection.requestor, event.xselection.property);
			}
		}*/
	}
}


Bool LinClipBoard::getSelection(Display* display, Window window, const char* bufname, const char* fmtname, std::string& res)
{
	char* result;
	unsigned long ressize, restail;
	int resbits;
	Atom bufid = XInternAtom(display, bufname, False),
		fmtid = XInternAtom(display, fmtname, False),
		propid = XInternAtom(display, "XSEL_DATA", False),
		incrid = XInternAtom(display, "INCR", False);
	XEvent event;

	XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime);
	do {
		XNextEvent(display, &event);
		if (event.type != SelectionNotify)
			return false;
	} while (event.type != SelectionNotify || event.xselection.selection != bufid);

	if (event.xselection.property)
	{
		XGetWindowProperty(display, window, propid, 0, LONG_MAX / 4, False, AnyPropertyType,
			&fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);

		if (fmtid != incrid)
		{
			res.append(result, ressize);
		}

		XFree(result);
		XDeleteProperty(event.xselection.display, event.xselection.requestor, event.xselection.property);
		return True;
	}
	else // request failed, e.g. owner can't convert to the target format
		return False;

}