#include "pch.h"
#include "LongerDays.h"

void LongerDays::ReadConfig(std::wstring path)
{
	std::string str(path.begin(), path.end());

	try
	{
		multiplier = ReadFloatIni(str, "settings", "time_multiplier");
		Log::Info << "Time Multiplier: " << multiplier << Log::Endl;

		show_welcome = ReadBoolIni(str, "settings", "show_welcome");
		Log::Info << "Show Welcome: " << show_welcome << Log::Endl;

		day_scale = ReadFloatIni(str, "settings", "day_scale");
		Log::Info << "Day Scale: " << day_scale << Log::Endl;

		night_scale = ReadFloatIni(str, "settings", "night_scale");
		Log::Info << "Night Scale: " << night_scale << Log::Endl;
	}
	catch (DWORD e)
	{
		Log::Error << "Failed to read settings ini file! Error Code: " << HEX_UPPER(e) << Log::Endl;
	}
}

void LongerDays::Tick()
{
	if (Native::Invoke<bool>(N::GET_IS_LOADING_SCREEN_ACTIVE))
		return;

	if (show_welcome)
	{
		static int timer = -1;
		if (timer == -1)
			timer = timeGetTime();

		if ((timeGetTime() - timer) >= 10000)
			show_welcome = false;
		
		std::ostringstream stream;
		stream << "Longer Days " << VERSION;
		DrawGameText(0.5f, 0.009f, stream.str(), 255, 0, 0, 255, true);
	}

	UpdateGameTime();

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

	DrawGameText(0, 0, dbg.str(), 255, 0, 0, 255);
	last_value = mins;
#endif
}

void LongerDays::UpdateGameTime()
{
	static int last_hour = -1, change_time = 0;
	int hour = Native::Invoke<int>(N::GET_CLOCK_HOURS);

	if (last_hour != hour || (timeGetTime() - change_time) >= 60000)
	{
		last_hour = hour;

		int new_time = (int)(((multiplier * 2000.f) * hour_multiplier[hour]) * GetScaledTime(hour));
		int game_time = Native::Invoke<int>(N::GET_MILLISECONDS_PER_GAME_MINUTE);

		if (game_time != new_time)
		{
			Native::Invoke<void, int>(N::_SET_MILLISECONDS_PER_GAME_MINUTE, new_time);
			Log::Info << "Updated Milliseconds Per Game Minute to " << new_time << Log::Endl;
		}
	}
}

float LongerDays::GetScaledTime(int hour)
{
	return (hour >= 7 && hour <= 19) ? day_scale : night_scale;
}