#pragma once
#include "pch.h"
#include <algorithm>

static float ReadFloatIni(std::string file, std::string header, std::string key)
{
	char buf[50];
	GetPrivateProfileStringA(header.c_str(), key.c_str(), "NO_VALUE", buf, 50, file.c_str());
	std::string str(buf);
	if (str.compare("NO_VALUE") == 0)
	{
		Log::Warning << "Could not read ini file!" << Log::Endl;
		return 1.0f;
	}

	return (float)atof(str.c_str());
}

static bool ReadBoolIni(std::string file, std::string header, std::string key)
{
	char buf[50];
	GetPrivateProfileStringA(header.c_str(), key.c_str(), "NO_VALUE", buf, 50, file.c_str());
	std::string str(buf);
	if (str.compare("NO_VALUE") == 0)
	{
		Log::Warning << "Could not read ini file!" << Log::Endl;
		return false;
	}
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);

	return str.compare("true") == 0;
}

static std::wstring GetModulePath(HMODULE module)
{
	DWORD size = MAX_PATH;
	std::vector<wchar_t> buffer(size);

	do
	{
		buffer.resize(size);
		GetModuleFileNameW(module, buffer.data(), size);
		size *= 1.5;
	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	std::wstring modulePath = std::wstring(buffer.begin(), buffer.end());

	size_t slashPos = modulePath.size();
	for (size_t i = modulePath.size() - 1; i >= 0; --i)
	{
		if (modulePath[i] == L'/' || modulePath[i] == L'\\')
		{
			slashPos = i;
			break;
		}
	}

	std::wstring moduleDir = modulePath.substr(0, slashPos);
	return moduleDir;
}

static void DrawGameText(float x, float y, std::string str, bool centre = false)
{
	HUD::SET_TEXT_SCALE(0.342f, 0.342f);
	HUD::SET_TEXT_CENTRE(centre);
	HUD::_SET_TEXT_COLOR(255, 0, 0, 255);
	
	const char* text = MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", str.c_str());
	HUD::_DRAW_TEXT(text, x, y);
}

static SHORT IsKeyPressed(int key)
{
	static HWND	game = FindWindowA("sgaWindow", "Red Dead Redemption 2");
	return GetForegroundWindow() == game ? GetAsyncKeyState(key) & 0x8000 : 0;
}