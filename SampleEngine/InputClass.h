// filename: InputClass.h

#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass &other);
	~InputClass();

	void Initialize();
	
	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);

	bool IsKeyDown(unsigned int key);

private:
	bool mKeys[256];
};

#endif // !_INPUTCLASS_H_
