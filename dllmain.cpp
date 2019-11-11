#include "pch.h"

static float multiplier = 1;
static constexpr float hour_multiplier[] = {
	1.99f,
	1.70f,
	1.50f,
	1.43f,
	1.34f,
	1.15f,
	1.00f,
	0.87f,
	0.77f,
	0.76f,
	0.752f,
	0.71f,
	0.67f,
	0.71f,
	0.752f,
	0.8f,
	0.82f,
	0.85f,
	0.953f,
	1.15f,
	1.34f,
	1.43f,
	1.54f,
	1.76f
};

void SetTimeMultiplier(float ammount, int hour)
{
	int time_per_mill = (int)((ammount * 2000.f) * hour_multiplier[hour]);
	int current_time = Native::Invoke<int>(N::GET_MILLISECONDS_PER_GAME_MINUTE);

	if (current_time != time_per_mill)
	{
		Native::Invoke<void, int>(N::_SET_MILLISECONDS_PER_GAME_MINUTE, time_per_mill);
		Log::Info << "Set Milliseconds Per Game Minute to " << time_per_mill << Log::Endl;
	}
}

extern "C" {
	DLL_EXPORT void Init(GetNativeAddressFunc getAddress)
	{
		Native::SetEssentialFunction(getAddress);
		Log::Info << "Intialized Longer Days " << VERSION << Log::Endl;
	}

	DLL_EXPORT void Tick()
	{
		if (Native::Invoke<bool>(N::GET_IS_LOADING_SCREEN_ACTIVE))
			return;

#ifdef _DEBUG // This code will only be compiled for Debug
		static int last_value;
		static int change_time = timeGetTime();
		int mins = Native::Invoke<int>(N::GET_CLOCK_MINUTES);
		if (last_value != mins)
		{
			change_time = timeGetTime();
		}

		std::ostringstream dbg;
		dbg << "game:ours " << Native::Invoke<int>(N::GET_MILLISECONDS_PER_GAME_MINUTE) << ":" << (int)(2000.f * multiplier) << " current in-game time: " << Native::Invoke<int>(N::GET_CLOCK_HOURS) << (mins < 10 ? ":0" : ":") << mins << " time since update: " << (timeGetTime() - change_time);

		DrawGameText(0.5, 0.5, dbg.str(), 255, 0, 0, 255);
		last_value = mins;
#endif
		
		static int welcome_timer = -1;
		if (welcome_timer == -1)
			welcome_timer = timeGetTime();

		if ((timeGetTime() - welcome_timer) <= 10000)
		{
			std::ostringstream stream;
			stream << "Longer Days " << VERSION;
			DrawGameText(960, 10, stream.str(), 255, 0, 0, 255, true);
		}

		static int last_hour = -1;
		static int time = 0;

		int hour = Native::Invoke<int>(N::GET_CLOCK_HOURS);
		if ((timeGetTime() - time) >= 60000 || time == 0 || last_hour != hour)
		{
			SetTimeMultiplier(multiplier, hour);
			time = timeGetTime();
			last_hour = hour;
		}
	}

#ifdef _DEBUG // This code will only be compiled for Debug
	DLL_EXPORT void OnKeyDown(uint32_t key)
	{

		if (key == 0x72)
		{
			SetTimeMultiplier(++multiplier, Native::Invoke<int>(N::GET_CLOCK_HOURS));
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
			multiplier = ReadFloatIni(str, "settings", "time_multiplier");
			Log::Info << "Time Multiplier: " << multiplier << Log::Endl;
		}
		catch (DWORD e)
		{
			Log::Error << "Failed to read settings ini file! Error Code: " << HEX_UPPER(e) << Log::Endl;
		}
	}
    return TRUE;
}

