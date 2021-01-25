#include "pch.h"

LongerDays longer_days;

void ScriptMain()
{
	while (true)
	{
		longer_days.Tick();
		WAIT(0);
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
		{
			std::wstring _moduleDir = GetModulePath(module);

			std::wstring ini_file = _moduleDir + L"/longer_days.ini";
			std::wstring logPath = _moduleDir + L"/longer_days_log.txt";

			Log::Push(new Log::FileStream(logPath));
			longer_days.ReadConfig(ini_file);

			scriptRegister(module, &ScriptMain);
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			scriptUnregister(module);
			break;
		}
		default:
			break;
	}

    return TRUE;
}

