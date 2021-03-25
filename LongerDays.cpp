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
	if (IsKeyPressed(0x76) && (timeGetTime() - key_tmr) >= 200) // F7
	{
		key_tmr = timeGetTime();
		day_time++;
		night_time++;
	}
	if (IsKeyPressed(0x77) && (timeGetTime() - key_tmr) >= 200) // F8
	{
		key_tmr = timeGetTime();
		day_time--;
		night_time--;
	}

	static int last_value;
	static int change_time = timeGetTime();
	int mins = CLOCK::GET_CLOCK_MINUTES();
	int secs = CLOCK::GET_CLOCK_SECONDS();
	if (last_value != mins)
	{
		change_time = timeGetTime();
	}

	std::ostringstream str; str << "Time From Hour: " << (int)(GetTimeFromHour(CLOCK::GET_CLOCK_HOURS()) * 1000.f) << "ms";
	DrawGameText(0.0f, 0.03f, str.str(), 255, 0, 0, 255);

	str.str(std::string()); str << "Update: " << (ShouldUpdate() ? "TRUE" : "FALSE");
	DrawGameText(0.0f, 0.06f, str.str(), 255, 0, 0, 255);

	str.str(std::string()); str << "Game Time: " << CLOCK::GET_CLOCK_HOURS() << (mins < 10 ? ":0" : ":") << mins << (secs < 10 ? ":0" : ":") << secs;
	DrawGameText(0.0f, 0.09f, str.str(), 255, 0, 0, 255);

	str.str(std::string()); str << "Last Update: " << (timeGetTime() - change_time) << "ms ago";
	DrawGameText(0.0f, 0.12f, str.str(), 255, 0, 0, 255);

	str.str(std::string()); str << "FPS: " << (int)(1.0f / MISC::GET_FRAME_TIME()) << " (" << MISC::GET_FRAME_TIME() << "s)";
	DrawGameText(0.0f, 0.15f, str.str(), 255, 0, 0, 255);


	str.str(std::string()); str << "Is Rendering TV?: " << GRAPHICS::GET_TV_CHANNEL();
	DrawGameText(0.0f, 0.18f, str.str(), 255, 0, 0, 255);

	/*std::ostringstream dbg;
	dbg << "	updates every " << (int)(GetTimeFromHour(CLOCK::GET_CLOCK_HOURS()) * 1000.f) << " current in-game time: " << CLOCK::GET_CLOCK_HOURS() << (mins < 10 ? ":0" : ":") << mins << "|" << (secs < 10 ? ":0" : ":")
		<< secs << " time since update: " << (timeGetTime() - change_time)  << " frame time: " << MISC::GET_FRAME_TIME();

	DrawGameText(0, 0, dbg.str(), 255, 0, 0, 255);*/
	last_value = mins;
#endif

}

/*
	From R* script 'script_rel/gang1.c':
	if (CLOCK::GET_CLOCK_HOURS() != 21)
	{
		CLOCK::SET_CLOCK_TIME(21, 0, 0);
		CLOCK::PAUSE_CLOCK(true, 0);
	}
*/

bool LongerDays::ShouldUpdate()
{
	Player id = PLAYER::PLAYER_ID();
	Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 0, 0);

	bool has_control = PLAYER::IS_PLAYER_CONTROL_ON(id) || PLAYER::IS_PLAYER_SCRIPT_CONTROL_ON(id);
	bool is_playing = !HUD::IS_PAUSE_MENU_ACTIVE() && CAM::IS_SCREEN_FADED_IN();
	bool is_mission = MISC::GET_MISSION_FLAG();
	bool near_show = BUILTIN::VDIST2(coords.x, coords.y, coords.z, -347.36, 699.83, 117.162) <= 75 || BUILTIN::VDIST2(coords.x, coords.y, coords.z, 2697.13, -1353.49, 49.45) <= 95;

	return is_playing && has_control && !is_mission && !near_show;
}

void LongerDays::UpdateGameTime()
{
	static float hours = (float)CLOCK::GET_CLOCK_HOURS();
	static float minutes = (float)CLOCK::GET_CLOCK_MINUTES();
	static float seconds = (float)CLOCK::GET_CLOCK_SECONDS();

	bool should_update_time = ShouldUpdate();
	if (!should_update_time)
	{
		hours = (float)CLOCK::GET_CLOCK_HOURS();
		minutes = (float)CLOCK::GET_CLOCK_MINUTES();
		seconds = (float)CLOCK::GET_CLOCK_SECONDS();
		return;
	}

	seconds += MISC::GET_FRAME_TIME() * (60 / GetTimeFromHour(hours));

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

	if (GRAPHICS::GET_TV_CHANNEL() == -1)
	{
		CLOCK::SET_CLOCK_TIME((int)hours, (int)minutes, (int)seconds);
	}
	
}

float LongerDays::GetTimeFromHour(int hour)
{
	return (hour >= day_start && hour <= day_end) ? day_time : night_time;
}