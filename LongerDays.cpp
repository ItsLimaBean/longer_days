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
		DrawGameText(0.5f, 0.009f, stream.str(), true);
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
	DrawGameText(0.0f, 0.03f, str.str());

	str.str(std::string()); str << "Update: " << (ShouldUpdate() ? "TRUE" : "FALSE");
	DrawGameText(0.0f, 0.06f, str.str());

	str.str(std::string()); str << "Game Time: " << CLOCK::GET_CLOCK_HOURS() << (mins < 10 ? ":0" : ":") << mins << (secs < 10 ? ":0" : ":") << secs;
	DrawGameText(0.0f, 0.09f, str.str());

	str.str(std::string()); str << "Last Update: " << (timeGetTime() - change_time) << "ms ago";
	DrawGameText(0.0f, 0.12f, str.str());

	str.str(std::string()); str << "FPS: " << (int)(1.0f / MISC::GET_FRAME_TIME()) << " (" << MISC::GET_FRAME_TIME() << "s)";
	DrawGameText(0.0f, 0.15f, str.str());

	str.str(std::string()); str << "Is Rendering TV?: " << GRAPHICS::GET_TV_CHANNEL();
	DrawGameText(0.0f, 0.18f, str.str());

	str.str(std::string()); str << "Internal Time " << time;
	DrawGameText(0.0f, 0.21f, str.str());

	last_value = mins;
#endif

}

bool LongerDays::ShouldUpdate()
{
	Player id = PLAYER::PLAYER_ID();
	Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 0, 0);

	bool has_control = PLAYER::IS_PLAYER_CONTROL_ON(id) || PLAYER::IS_PLAYER_SCRIPT_CONTROL_ON(id)
		|| PAD::IS_CONTROL_ENABLED(0, 0xA987235F) || PAD::IS_CONTROL_ENABLED(0, 0xD82E0BD2); //     FrontendPause = 0xD82E0BD2,    LookLr = 0xA987235F,

	bool is_playing = !HUD::IS_PAUSE_MENU_ACTIVE() && CAM::IS_SCREEN_FADED_IN();
	bool is_mission = MISC::GET_MISSION_FLAG();
	bool near_show = BUILTIN::VDIST2(coords.x, coords.y, coords.z, -347.36f, 699.83f, 117.162f) <= 75.f || BUILTIN::VDIST2(coords.x, coords.y, coords.z, 2697.13f, -1353.49f, 49.45f) <= 95.f;

	return is_playing && has_control && !is_mission && !near_show;
}

void LongerDays::FetchGameTime()
{
	int hour = CLOCK::GET_CLOCK_HOURS() * 60 * 60;
	int min = CLOCK::GET_CLOCK_MINUTES() * 60;
	int sec = CLOCK::GET_CLOCK_SECONDS();
	time = sec + min + hour;
}

void LongerDays::UpdateGameTime()
{
	static std::once_flag flag;
	std::call_once(flag, [this] { FetchGameTime(); });

	bool should_update_time = ShouldUpdate();
	if (!should_update_time)
	{
		FetchGameTime();
		return;
	}

	int hour = CLOCK::GET_CLOCK_HOURS();

	CheckTimeChange(hour);

	time += MISC::GET_FRAME_TIME() * (60 / GetTimeFromHour(hour));
	
	// In reality this doesn't really need to be a while loop
	if (time >= 86400.0f)
	{
		time -= 86400.0f;
	}

	if (GRAPHICS::GET_TV_CHANNEL() == -1)
	{
		CLOCK::SET_CLOCK_TIME((int)time / 60 / 60, ((int)time / 60) % 60, (int)time % 60);
	}
}

// This function allows for other mods to change the game time.
void LongerDays::CheckTimeChange(int hour)
{
	static int last_hour = hour;
	int current_hour = hour;
	if (current_hour != last_hour)
	{
		last_hour = current_hour;
		FetchGameTime();
	}
}

float LongerDays::GetTimeFromHour(int hour)
{
	return (hour >= day_start && hour <= day_end) ? day_time : night_time;
}