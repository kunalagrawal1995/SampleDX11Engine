// filename: SystemClass.cpp

#include "SystemClass.h"

SystemClass::SystemClass()
{
	mInput = 0;
	mGraphics = 0;
}

SystemClass::SystemClass(const SystemClass &other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	// initialize windows
	InitializeWindows(screenWidth, screenHeight);

	// create and initialize input object
	mInput = new InputClass;
	if (!mInput)
	{
		return false;
	}
	mInput->Initialize();

	mGraphics = new GraphicsClass;
	if (!mGraphics)
	{
		return false;
	}

	result = mGraphics->Initialize(screenWidth, screenHeight, mHwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// release Graphics object
	if (mGraphics)
	{
		mGraphics->Shutdown();
		delete mGraphics;
		mGraphics = 0;
	}

	// release Input object
	if (mInput)
	{
		delete mInput;
		mInput = 0;
	}

	// shutdown windows
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done;
	bool result;

	// initialize message
	ZeroMemory(&msg, sizeof(MSG));

	// main loop
	done = false;
	while (!done)
	{
		// handle windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// do frame processing
			result = Draw();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool SystemClass::Draw()
{
	bool result;

	if (mInput->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// do frame processing
	result = mGraphics->Draw();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_KEYDOWN:
		{
			// if key pressed, inform input object
			mInput->KeyDown((unsigned int)wparam);
			return 0;
		}
		case WM_KEYUP:
		{
			// if key unpressed, inform input object
			mInput->KeyUp((unsigned int)wparam);
			return 0;
		}
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// pointer to this object
	ApplicationHandle = this;

	// get instance of this application
	mHinstance = GetModuleHandle(NULL);

	// application name
	mApplicationName = "Sample Engine";

	// setup window class
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
		
	// register window class
	RegisterClassEx(&wc);

	// find resolution of screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// setup screen settings based on screen mode
	if (FULL_SCREEN)
	{
		// 32 bit screen and max size.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSWIDTH;

		// change display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// set position of window to top left corner
		posX = 0;
		posY = 0;
	}
	else
	{
		// windowed mode
		screenWidth = 800;
		screenHeight = 600;

		// place window in middle of screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// create window and get its handle
	mHwnd = CreateWindowEx(WS_EX_APPWINDOW, mApplicationName, mApplicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, mHinstance, NULL);

	// bring the window on screen and set it in focus
	ShowWindow(mHwnd, SW_SHOW);
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);

	// hide the mouse cursor
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// show mouse cursor
	ShowCursor(true);

	// fix display settings if in full screen mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// remove window
	DestroyWindow(mHwnd);
	mHwnd = NULL;

	// remove application instance
	UnregisterClass(mApplicationName, mHinstance);
	mHinstance = NULL;

	// release pointer to this class
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
	}
}
