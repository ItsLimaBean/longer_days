#include "pch.h"

void LongerDays::ReadConfig(std::wstring path)
{
	std::string str(path.begin(), path.end());

	try
	{
		Log::Info << "Version " << VERSION << Log::Endl;

		show_welcome = ReadBoolIni(str, "settings", "show_welcome");
		Log::Info << "Show Welcome: " << show_welcome << Log::Endl;

		day_time = ReadFloatIni(str, "settings", "day_time");
		Log::Info << "Day Time: " << day_time << Log::Endl;

		night_time = ReadFloatIni(str, "settings", "night_time");
		Log::Info << "Night Time: " << night_time << Log::Endl;

		day_start = (int)ReadFloatIni(str, "settings", "day_start");
		Log::Info << "Day Start: " << day_start << Log::Endl;

		day_end = (int)ReadFloatIni(str, "settings", "day_end");
		Log::Info << "Day End: " << day_end << Log::Endl;

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
		static auto timeout = timeGetTime() + 10000;

		if (timeGetTime() >= timeout)
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
	int secs = CLOCK::GET_CLOCK_SECONDS();
	if (last_value != mins)
	{
		change_time = timeGetTime();
	}

	std::ostringstream dbg;
	dbg << "	updates every " << (int)(GetTimeFromHour(CLOCK::GET_CLOCK_HOURS()) * 1000.f) << " current in-game time: " << CLOCK::GET_CLOCK_HOURS() << (mins < 10 ? ":0" : ":") << mins << "|" << (secs < 10 ? ":0" : ":")
		<< secs << " time since update: " << (timeGetTime() - change_time)  << " frame time: " << MISC::GET_FRAME_TIME();

	DrawGameText(0, 0, dbg.str(), 255, 0, 0, 255);
	last_value = mins;
#endif

}

void LongerDays::UpdateGameTime()
{
	bool should_pause_clock = PLAYER::IS_PLAYER_CONTROL_ON(PLAYER::PLAYER_ID()) && !HUD::IS_PAUSE_MENU_ACTIVE() && CAM::IS_SCREEN_FADED_IN();
	CLOCK::PAUSE_CLOCK(should_pause_clock, should_pause_clock);
	

	static float hours = (float)CLOCK::GET_CLOCK_HOURS();
	static float minutes = (float)CLOCK::GET_CLOCK_MINUTES();
	static float seconds = (float)CLOCK::GET_CLOCK_SECONDS();

	if (!should_pause_clock)
	{
		hours = (float)CLOCK::GET_CLOCK_HOURS();
		minutes = (float)CLOCK::GET_CLOCK_MINUTES();
		seconds = (float)CLOCK::GET_CLOCK_SECONDS();
		return;
	}

	seconds += MISC::GET_FRAME_TIME() * ( 60 / GetTimeFromHour(hours));

	if (seconds >= 59.f)
	{
		seconds = 0.f;
		minutes++;
	}

	if (minutes >= 59.f)
	{
		minutes = 0.f;
		hours++;
	}

	if (hours >= 23.f)
	{
		hours = 0.f;
	}

	CLOCK::SET_CLOCK_TIME((int)hours, (int)minutes, (int)seconds);
}

float LongerDays::GetTimeFromHour(int hour)
{
	return (hour >= day_start && hour <= day_end) ? day_time : night_time;
}