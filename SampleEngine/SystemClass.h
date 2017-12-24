// filename: SystemClass.h

#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// includes
#include <windows.h>

// include my classes
#include "GraphicsClass.h"
#include "InputClass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass& other);
	~SystemClass();

	bool Initialize();
	void Run();
	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private: 
	bool Draw();
	void InitializeWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();

private:
	LPCSTR mApplicationName;
	HINSTANCE mHinstance;
	HWND mHwnd;

	InputClass * mInput;
	GraphicsClass * mGraphics;
};


// function prototypes
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

// globals
static SystemClass * ApplicationHandle = 0;

#endif // !_SYSTEMCLASS_H_
