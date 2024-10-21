#pragma once
#define DIRECTINPUT_VERSION	0x0800

#include <Windows.h>
#include <wrl.h>
#include <dinput.h>

class Input
{
public:

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	void Update();

private:

	ComPtr<IDirectInputDevice8> keyboard;

};
