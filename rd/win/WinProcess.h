#pragma once

#include <windows.h>
#include <string>
class WinProcess
{
public:
	WinProcess();
	~WinProcess();
	bool exec(const std::string& app, const std::string& comm, DWORD sessionID);
	bool exec(const std::string& app, const std::string& comm, DWORD sessionID,
		HANDLE& hStdIn, HANDLE& hStdOut, HANDLE& hStdErr);

	void terminate();
	bool isRunning();
	void wait(int msec = 0);
	HANDLE handle() const;


private:
	HANDLE _hProcess;
	HANDLE createProcess(const std::string &app, const std::string &comm, DWORD sessionID);
	bool initPipes();

	HANDLE _hStdIn_Rd, _hStdOut_Rd, _hStdErr_Rd, _hStdIn_Wr, _hStdOut_Wr, _hStdErr_Wr;

};

