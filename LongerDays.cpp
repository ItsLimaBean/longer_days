#include "pch.h"

void LongerDays::ReadConfig(std::wstring path)
{
	std::string str(path.begin(), path.end());

	try
	{
		show_welcome = ReadBoolIni(str, "settings", "show_welcome");
		Log::Info << "Show Welcome: " << show_welcome << Log::Endl;

		day_time = ReadFloatIni(str, "settings", "day_time");
		Log::Info << "Day Time: " << day_time << Log::Endl;

		night_time = ReadFloatIni(str, "settings", "night_time");
		Log::Info << "Night Time: " << night_time << Log::Endl;
	}
	catch (DWORD e)
	{
		Log::Error << "Failed to read settings ini file! Error Code: " << HEX_UPPER(e) << Log::Endl;
	}
}

void LongerDays::Tick()
{
	if (DLC::GET_IS_LOADING_SCREEN_ACTIVE())
		return;

	if (show_welcome)
	{
		static auto timeout = std::chrono::high_resolution_clock::now() + 10s;

		if (std::chrono::high_resolution_clock::now() >= timeout)
			show_welcome = false;
		
		std::ostringstream stream;
		stream << "Longer Days " << VERSION;
		DrawGameText(0.5f, 0.009f, stream.str(), 255, 0, 0, 255, true);
	}

	UpdateGameTime();
#ifdef _DEBUG // This code will only be compiled for Debug

	static int key_tmr = timeGetTime();
	if (IsKeyPressed(0x72) && (timeGetTime() - key_tmr) >= 200)
	{
		key_tmr = timeGetTime();
		day_time++;
		night_time++;
	}
	if (IsKeyPressed(0x73) && (timeGetTime() - key_tmr) >= 200)
	{
		key_tmr = timeGetTime();
		int hour = CLOCK::GET_CLOCK_HOURS();
		CLOCK::SET_CLOCK_TIME(hour == 23 ? 0 : hour + 1, 0, 0);
	}

		static int last_value;
		static int change_time = timeGetTime();
		int mins = CLOCK::GET_CLOCK_MINUTES();
		if (last_value != mins)
		{
			change_time = timeGetTime();
		}

		std::ostringstream dbg;
		dbg << "game:ours " << CLOCK::GET_MILLISECONDS_PER_GAME_MINUTE() << ":" << (int)(GetTimeFromHour(CLOCK::GET_CLOCK_HOURS()) * 1000.f) << " current in-game time: " << CLOCK::GET_CLOCK_HOURS() << (mins < 10 ? ":0" : ":") << mins << " time since update: " << (timeGetTime() - change_time);

		DrawGameText(0, 0, dbg.str(), 255, 0, 0, 255);
		last_value = mins;
#endif

}

void LongerDays::UpdateGameTime()
{
	static auto interval = std::chrono::high_resolution_clock::now() + 15s;
	static int last_hour = -1;

	int hour = CLOCK::GET_CLOCK_HOURS();
	if (std::chrono::high_resolution_clock::now() >= interval || last_hour != hour)
	{
		interval = std::chrono::high_resolution_clock::now() + 15s;
		last_hour = hour;

		//int time = (int)(((multiplier * 2000.f) * hour_multiplier[hour]) * GetScaledTime(hour));
		int time = (int)((GetTimeFromHour(hour) * 1000.f) * hour_multiplier[hour]);
		if (time != CLOCK::GET_MILLISECONDS_PER_GAME_MINUTE())
		{
			CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(time);
			Log::Info << "Changed time to: " << time << Log::Endl;
		}
	}
}

float LongerDays::GetTimeFromHour(int hour)
{
	return (hour >= 7 && hour <= 19) ? day_time : night_time;
}