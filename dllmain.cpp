#include "pch.h"

LongerDays longer_days;

extern "C" {
	DLL_EXPORT void Init(GetNativeAddressFunc getAddress)
	{
		Native::SetEssentialFunction(getAddress);

		Log::Info << "Intialized Longer Days " << VERSION << Log::Endl;
	}

	DLL_EXPORT void Tick()
	{
		longer_days.Tick();
	}

#ifdef _DEBUG // This code will only be compiled for Debug
	DLL_EXPORT void OnKeyDown(uint32_t key)
	{
		if (key == 0x72)
		{
			longer_days.multiplier++;
		}
		if (key == 0x73)
		{
			int hour = CLOCK::GET_CLOCK_HOURS();
			CLOCK::SET_CLOCK_TIME(hour == 23 ? 0 : hour + 1, 0, 0);
		}
	}
#endif
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		std::wstring _moduleDir = GetModulePath(module);

		std::wstring ini_file = _moduleDir + L"/longer_days.ini";
		std::wstring logPath = _moduleDir + L"/longer_days_log.txt";

		Log::Push(new Log::FileStream(logPath));
		longer_days.ReadConfig(ini_file);
	}
    return TRUE;
}

