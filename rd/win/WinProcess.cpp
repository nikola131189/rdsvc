#include "WinProcess.h"
#include <iostream>


WinProcess::WinProcess() 
	: _hProcess(0), _hStdIn_Rd(0), _hStdOut_Rd(0), _hStdErr_Rd(0), _hStdIn_Wr(0), _hStdOut_Wr(0), _hStdErr_Wr(0)
{
}


WinProcess::~WinProcess()
{
	terminate();
}

bool WinProcess::exec(const std::string& app, const std::string& comm, DWORD sessionID)
{
	_hProcess = createProcess(app, comm, sessionID);
	if (_hProcess) return true;
	else return false;
}

bool WinProcess::exec(const std::string &app, const std::string &comm, DWORD sessionID,
	HANDLE& hStdIn, HANDLE& hStdOut, HANDLE& hStdErr)
{
	if (!initPipes())
		return false;


	hStdErr = _hStdErr_Rd;
	hStdOut = _hStdOut_Rd;
	hStdIn = _hStdIn_Wr;


	_hProcess = createProcess(app, comm, sessionID);
	if (_hProcess) return true;
	else return false;
}

void WinProcess::terminate()
{
	CloseHandle(_hStdErr_Rd);
	CloseHandle(_hStdOut_Rd);
	CloseHandle(_hStdIn_Wr);
	if (!isRunning()) return;
	TerminateProcess(_hProcess, 0);
	_hProcess = 0;
}

bool WinProcess::isRunning()
{
	if (!_hProcess) false;
	DWORD exFlag = 0;
	GetExitCodeProcess(_hProcess, &exFlag);
	if (exFlag == STILL_ACTIVE) return true;
	_hProcess = 0;
	return false;
}

void WinProcess::wait(int msec)
{
	if(!msec)
		WaitForSingleObject(_hProcess, INFINITE);
	else
		WaitForSingleObject(_hProcess, msec);
}

HANDLE WinProcess::handle() const
{
	return _hProcess;
}


HANDLE WinProcess::createProcess(const std::string &app, const std::string &comm, DWORD sessionID)
{

	HANDLE hpToken, htToken, ppToken;
	int res = 0;
	res = OpenProcessToken(GetCurrentProcess(), TOKEN_IMPERSONATE | TOKEN_DUPLICATE, &hpToken);
	res = DuplicateToken(hpToken, SecurityImpersonation, &htToken);
	res = SetThreadToken(NULL, htToken);

	CloseHandle(hpToken);
	CloseHandle(htToken);

	res = OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID | TOKEN_IMPERSONATE | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY, FALSE, &hpToken);

	SetTokenInformation(hpToken, TokenSessionId, &sessionID, sizeof(sessionID));
	DuplicateTokenEx(hpToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &ppToken);

	PROCESS_INFORMATION processInfo;

	STARTUPINFOA startInfo;
	ZeroMemory(&startInfo, sizeof(STARTUPINFOA));
	startInfo.cb = sizeof(STARTUPINFOA);
	startInfo.hStdError = _hStdErr_Wr;
	startInfo.hStdOutput = _hStdOut_Wr;
	startInfo.hStdInput = _hStdIn_Rd;
	startInfo.dwFlags |= STARTF_USESTDHANDLES;

	res = CreateProcessAsUserA(ppToken, (LPSTR)&app[0], (LPSTR)comm.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW | CREATE_PRESERVE_CODE_AUTHZ_LEVEL, NULL, NULL, &startInfo, &processInfo);


	CloseHandle(hpToken);
	CloseHandle(ppToken);
	CloseHandle(processInfo.hThread);
	ImpersonateSelf(SecurityImpersonation);



	CloseHandle(_hStdErr_Wr);
	CloseHandle(_hStdOut_Wr);
	CloseHandle(_hStdIn_Rd);

	if (!res) return 0;
	return processInfo.hProcess;
}

bool WinProcess::initPipes()
{
	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;


	if (!CreatePipe(&_hStdOut_Rd, &_hStdOut_Wr, &saAttr, 0))
		return 0;

	if (!SetHandleInformation(_hStdOut_Rd, HANDLE_FLAG_INHERIT, 0))
		return 0;


	if (!CreatePipe(&_hStdErr_Rd, &_hStdErr_Wr, &saAttr, 0))
		return 0;

	if (!SetHandleInformation(_hStdErr_Rd, HANDLE_FLAG_INHERIT, 0))
		return 0;


	if (!CreatePipe(&_hStdIn_Rd, &_hStdIn_Wr, &saAttr, 0))
		return 0;

	if (!SetHandleInformation(_hStdIn_Wr, HANDLE_FLAG_INHERIT, 0))
		return 0;


	return true;
}
