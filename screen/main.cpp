#pragma once

#include <rd/rd.h>

#include "Application.h"

#ifdef _WIN32
#include <rd/win/NativeWin.h>
#endif

#ifdef __linux__ 
#include <rd/lin/NativeLin.h>
#endif

#include <string>


template<typename Ret>
Ret findArg(const std::string& key, int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == key)
		{
			if (i + 1 == argc) boost::lexical_cast<Ret>("");
			return boost::lexical_cast<Ret>(argv[i + 1]);
		}
	}
	return Ret();
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
	HDESK hdesk = OpenInputDesktop(0, true, GENERIC_ALL);
	SetThreadDesktop(hdesk);
	CloseDesktop(hdesk);

	WinNative native;
#endif

#ifdef __linux__ 
	LinNative native;
#endif


	//LOG_storage::init("screen.log", 1024 * 1024 * 20);
	
	auto imprint = findArg<std::string>("--src", argc, argv);

	auto dest = findArg<uint32_t>("--dest", argc, argv);

	int sid = findArg<int>("--screen_id", argc, argv); 

	codec::VideoFormat fmt;
	fmt.w = findArg<int>("--w", argc, argv);
	fmt.h = findArg<int>("--h", argc, argv); 
	fmt.dw = findArg<int>("--dw", argc, argv);
	fmt.dh = findArg<int>("--dh", argc, argv);
	fmt.id  = (codec::VideoFormat::CodecId)findArg<int>("--encoding_id", argc, argv);
	fmt.bitarte = findArg<int64_t>("--bit_rate", argc, argv); 
	fmt.profile = findArg<int>("--profile", argc, argv);
	fmt.quantization = findArg<int>("--quantization", argc, argv);
	fmt.threadCount = findArg<int>("--thread_count", argc, argv);
	bool blockInput = findArg<bool>("--block_input", argc, argv);


	native.eventLoop().blockInput(blockInput);
	Rd::Config cfg("config.cfg");

	Application appl(native, cfg, dest, sid, fmt);
	
	native.eventLoop().loop();
	return 1;
}


