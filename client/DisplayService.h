#pragma once
#include <rd/rd.h>


class DisplayService
{
public:
	DisplayService(Rd::Native& native, const std::string& imprint);	
	void notify(const Rd::DisplayEvent& ev);
private:

	void getDisplay(uint32_t id);
	void openScreen(const std::string& src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input);
	void closeScreen(int id);
private:
	Rd::Native& _native;
	uint32_t _clientId;
	int _sid;
	std::string _imprint;
};

