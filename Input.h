#pragma once
#define DIRECTINPUT_VERSION	0x0800

#include <Windows.h>
#include <wrl.h>
#include <dinput.h>
#include "WinApp.h"

class Input
{
public:

	template <class T> using ComPtr = ComPtr<T>;

public:
	void Initialize(WinApp* winApp);

	void Update();

	bool PushKey(BYTE KeyNumber);

	bool TriggerKey(BYTE KeyNumber);

private:

	ComPtr<IDirectInput8> directInput;

	ComPtr<IDirectInputDevice8> keyboard;

	BYTE key[256] = {};

	BYTE keyPre[256] = {};

	WinApp* winApp = nullptr;
};
