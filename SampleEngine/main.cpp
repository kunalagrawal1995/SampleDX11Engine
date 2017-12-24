// filename: main.cpp

#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass * System;
	bool result;

	//create System object
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	//initialize System object and run it
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	//Shutdown once run is complete
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}