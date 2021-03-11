#include "DisplayService.h"

DisplayService::DisplayService(Rd::Native& native, const std::string& imprint) :
	_native(native), _clientId(0), _sid(0), _imprint(imprint)
{}

void DisplayService::notify(const Rd::DisplayEvent& ev)
{
	if (auto pval = std::get_if<Rd::DisplayEvent::Request>(&ev.val))
	{
		getDisplay(ev.source);
	}

	if (auto pval = std::get_if<Rd::DisplayEvent::Open>(&ev.val))
	{
		openScreen(_imprint, ev.source, pval->display, pval->screenId, pval->videoFormat, pval->blockInput);
	}


	if (auto pval = std::get_if<Rd::DisplayEvent::Close>(&ev.val))
	{
		closeScreen(pval->screenId);
	}
}

void DisplayService::getDisplay(uint32_t id)
{
	Rd::DisplayEvent::Response v;
	_native.display().getDisplay(v.display);

	Rd::DisplayEvent ev;
	ev.val = v;
	Rd::Inet::send(id, ev);
}

void DisplayService::openScreen(const std::string & src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input)
{
	_clientId = dest;
	_sid = id;
	_native.display().openScreen(src, dest, disp, id, fmt, block_input);
}

void DisplayService::closeScreen(int id)
{
	_native.display().closeScreen(id);
}
