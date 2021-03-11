#include "WinClipBoard.h"


void WinClipBoard::setCbck(const ClipBoardCbck& cbck)
{
	_cbck = cbck;
}


void WinClipBoard::setData(const std::string& str)
{
	_syncClipboard = 0;
	std::wstring wstr = utility::ConvertUtf8ToWide(str);
	setClipboardText(wstr);
}

void WinClipBoard::notify()
{
	if (_syncClipboard)
	{
		std::string str = utility::ConvertWideToUtf8(GetClipboardText());

		if (!str.empty() && _cbck)
			_cbck(str);
	}
	_syncClipboard = true;
}
	


std::wstring WinClipBoard::GetClipboardText()
{
	// Try opening the clipboard
	if (!OpenClipboard(nullptr))
		return L"";
		

		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (hData == nullptr)
		{
			CloseClipboard();
			return L"";
		}

		wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
		if (pszText == nullptr)
		{
			GlobalUnlock(hData);
			CloseClipboard();
			return L"";
		}

	  // Save text in a string class instance
		std::wstring text(pszText);

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	return text;
}

void WinClipBoard::setClipboardText(const std::wstring& text)
{
	const wchar_t* output = text.c_str();
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * 2);
	memcpy(GlobalLock(hMem), output, text.size() * 2);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
}
