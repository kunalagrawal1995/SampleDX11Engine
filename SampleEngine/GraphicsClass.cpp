// filename: GraphicsClass.cpp

#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	mD3D = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass & other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// use _mm_malloc to align the memory allocation
	// ref : https://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16
	mD3D = (D3DClass *)_mm_malloc(sizeof(D3DClass), 16);
	if (!mD3D)
	{
		return false;
	}

	// initialize D3D
	result = mD3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D", "ERROR", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (mD3D)
	{
		mD3D->Shutdown();
		_mm_free(mD3D);
		mD3D = NULL;
	}

	return;
}

bool GraphicsClass::Draw()
{
	bool result;

	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	// draw a gray scene
	mD3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// present the scene
	mD3D->EndScene();
	return true;
}