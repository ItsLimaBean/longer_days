#pragma once
#include "pch.h"

static SHORT IsKeyPressed(int key)
{
	static HWND	game = FindWindowA("sgaWindow", "Red Dead Redemption 2");
	return GetForegroundWindow() == game ? GetAsyncKeyState(key) & 0x8000 : 0;
}