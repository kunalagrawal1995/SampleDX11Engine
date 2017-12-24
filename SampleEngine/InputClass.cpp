// filename: InputClass.cpp

#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass & other)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		mKeys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int key)
{
	mKeys[key] = true;
}

void InputClass::KeyUp(unsigned int key)
{
	mKeys[key] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return mKeys[key];
}