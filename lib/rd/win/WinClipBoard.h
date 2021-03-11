#pragma once
#include "../Native.h"
#include <windows.h>

#include "../utility.h"
#include "../Log.h"

class WinClipBoard : public Rd::Native::ClipBoard
{
public:
	void setCbck(const ClipBoardCbck& cbck);
	void setData(const std::string& str);

	void notify();

private:
	static std::wstring GetClipboardText();
	static void setClipboardText(const std::wstring& text);
	bool _syncClipboard = true;
	ClipBoardCbck _cbck = nullptr;
};
