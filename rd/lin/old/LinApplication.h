#pragma once
#include "../AbstractApplication.h"
#include <stdio.h>


struct LinApplication : public AbstractApplication
{
	void run()
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
};