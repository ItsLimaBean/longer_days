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
	for (int i = modulePath.size() - 1; i >= 0; --i)
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

static void DrawGameText(uint16_t x, uint16_t y, std::string str, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool centre = false)
{
	float pos_x = x / (float)1920;
	float pos_y = y / (float)1080;

	Native::Invoke<void, float, float>(N::SET_TEXT_SCALE, 0.342f, 0.342f);
	Native::Invoke<void, bool>(N::SET_TEXT_CENTRE, centre);
	Native::Invoke<void, uint8_t, uint8_t, uint8_t, uint8_t>(N::SET_TEXT_COLOR, r, g, b, a);
	
	const char* text = Native::Invoke<const char*, int, const char*, const char*>(N::_CREATE_VAR_STRING, 10, "LITERAL_STRING", str.c_str());
	
	Native::Invoke<void, const char*, float, float>(N::_DRAW_TEXT, text, pos_x, pos_y);
}