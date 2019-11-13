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
			int hour = Native::Invoke<int>(N::GET_CLOCK_HOURS);
			Native::Invoke<int, int, int, int>(N::SET_CLOCK_TIME, hour == 23 ? 0 : hour + 1, 0, 0);
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

		try
		{
			std::string str(ini_file.begin(), ini_file.end());
			longer_days.multiplier = ReadFloatIni(str, "settings", "time_multiplier");
			Log::Info << "Time Multiplier: " << longer_days.multiplier << Log::Endl;

			longer_days.show_welcome = ReadBoolIni(str, "settings", "show_welcome");
			Log::Info << "Show Welcome: " << longer_days.show_welcome << Log::Endl;
		}
		catch (DWORD e)
		{
			Log::Error << "Failed to read settings ini file! Error Code: " << HEX_UPPER(e) << Log::Endl;
		}
	}
    return TRUE;
}

