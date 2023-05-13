#include "pch.h"

namespace longer_days
{
	void initialize_script()
	{
		g_script.initialize();

		while (true)
		{
			g_script.on_tick();
			WAIT(0);
		}
	}

	void script::on_tick()
	{
#pragma region Show Welcome
		
		if (m_show_initial)
		{
			const auto current_time = timeGetTime();
			static auto timeout = current_time + 8000;
			if (current_time > timeout)
				m_show_initial = false;

			if (m_config.m_show_welcome)
			{
				draw_text(0.5f, 0.009f, "Longer Days " VERSION, true);
			}

			if (!m_use_hook)
			{
				draw_text(0.5f, 0.039f, "~COLOR_YELLOWSTRONG~An issue occured loading Longer Days, check 'longer_days.log' for more details.", true);
				draw_text(0.5f, 0.069f, "~COLOR_YELLOWSTRONG~The mod will still continue to function.", true);
			}
		}
#pragma endregion
		
		m_current_hour = CLOCK::GET_CLOCK_HOURS();

		Player player = PLAYER::PLAYER_ID();
		bool is_mission = MISC::GET_MISSION_FLAG();
		bool is_playing = !HUD::IS_PAUSE_MENU_ACTIVE() && CAM::IS_SCREEN_FADED_IN()
			&& !DLC::GET_IS_LOADING_SCREEN_ACTIVE();
		bool has_control = PLAYER::IS_PLAYER_CONTROL_ON(player) || PLAYER::IS_PLAYER_SCRIPT_CONTROL_ON(player)
			|| PAD::IS_CONTROL_ENABLED(0, 0xA987235F) || PAD::IS_CONTROL_ENABLED(0, 0xD82E0BD2); //     FrontendPause = 0xD82E0BD2,    LookLr = 0xA987235F,
		m_enabled = !is_mission && is_playing && has_control;

		if (!m_use_hook && m_enabled)
		{
			CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(static_cast<int>(get_time_from_hour() * hour_multiplier[m_current_hour]));
		}

#ifdef _DEBUG // This code will only be compiled for Debug

		static int key_tmr = timeGetTime();
		if (IsKeyPressed(0x76) && (timeGetTime() - key_tmr) >= 200) // F7
		{
			key_tmr = timeGetTime();
			if (CLOCK::GET_CLOCK_HOURS() == 23)
			{
				CLOCK::SET_CLOCK_TIME(0, 0, 0);
			}
			else
			{
				CLOCK::SET_CLOCK_TIME(CLOCK::GET_CLOCK_HOURS() + 1, 0, 0);
			}
		}
		if (IsKeyPressed(0x77) && (timeGetTime() - key_tmr) >= 200) // F8
		{
			key_tmr = timeGetTime();
			if (CLOCK::GET_CLOCK_HOURS() == 0)
			{
				CLOCK::SET_CLOCK_TIME(23, 0, 0);
			}
			else
			{
				CLOCK::SET_CLOCK_TIME(CLOCK::GET_CLOCK_HOURS() - 1, 0, 0);
			}
		}

		static int last_value;
		static int change_time = timeGetTime();
		static int previous = 0;
		int mins = CLOCK::GET_CLOCK_MINUTES();
		int secs = CLOCK::GET_CLOCK_SECONDS();
		if (last_value != mins)
		{
			previous = (timeGetTime() - change_time);
			change_time = timeGetTime();
		}

		std::stringstream str;
		str.str(std::string()); str << "Game Time: " << CLOCK::GET_CLOCK_HOURS() << (mins < 10 ? ":0" : ":") << mins << (secs < 10 ? ":0" : ":") << secs;
		draw_text(0.0f, 0.09f, str.str());

		str.str(std::string()); str << "Last Update: " << (timeGetTime() - change_time) << "ms ago";
		draw_text(0.0f, 0.12f, str.str());

		str.str(std::string()); str << "Previous Update time: " << previous << "ms";
		draw_text(0.0f, 0.15f, str.str());

		str.str(std::string()); str << "FPS: " << (int)(1.0f / MISC::GET_FRAME_TIME()) << " (" << MISC::GET_FRAME_TIME() << "s)";
		draw_text(0.0f, 0.18f, str.str());

		str.str(std::string()); str << "Is Rendering TV?: " << GRAPHICS::GET_TV_CHANNEL();
		draw_text(0.0f, 0.21f, str.str());

		str.str(std::string()); str << "Internal Time: " << timeGetTime();
		draw_text(0.0f, 0.24f, str.str());

		str.str(std::string()); str << "Enabled: " << m_enabled;
		draw_text(0.0f, 0.27f, str.str());

		str.str(std::string()); str << "Target Time: " << get_time_from_hour();
		draw_text(0.0f, 0.30f, str.str());

		str.str(std::string()); str << "Current MS Per Game Min: " << CLOCK::GET_MILLISECONDS_PER_GAME_MINUTE();
		draw_text(0.0f, 0.33f, str.str());

		str.str(std::string()); str << "Mode: " << (m_get_ms_per_game_min ? "Hook" : "Native");
		draw_text(0.0f, 0.36f, str.str());

		last_value = mins;
#endif

	}

	void script::cleanup()
	{
		if (m_use_hook)
		{
			MH_DisableHook(m_get_ms_per_game_min);
			MH_RemoveHook(m_get_ms_per_game_min);
			Log::Info << "Disabled 'get_ms_per_game_min' hook." << Log::Endl;
		}
		else
		{
			CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(2000);
		}
		
		MH_Uninitialize();
		Log::Info << "Uninitialized MinHook." << Log::Endl;
	}

	void script::initialize()
	{
		Log::Info << "Default MS per Game Minute " << CLOCK::GET_MILLISECONDS_PER_GAME_MINUTE() << Log::Endl;
		MH_Initialize();
		Log::Info << "Initialized MinHook." << Log::Endl;

		const char* pattern = m_config.m_get_ms_per_game_min_pattern.c_str();
		m_get_ms_per_game_min = Signature(pattern).Scan().As<PVOID>();
		uintptr_t offset = (uintptr_t)m_get_ms_per_game_min - Module(nullptr).Base().As<std::uintptr_t>();
		if (m_get_ms_per_game_min && offset != 0)
		{
			Log::Info << "Found 'get_ms_per_game_min' at RDR2.exe+0x" << std::hex << std::uppercase << offset << std::nouppercase << "." << Log::Endl;

			MH_CreateHook(m_get_ms_per_game_min, &hooks::get_ms_per_game_min_hook, (PVOID*)&hooks::get_ms_per_game_min_orig);
			MH_EnableHook(m_get_ms_per_game_min);
			Log::Info << "Enabled 'get_ms_per_game_min' hook." << Log::Endl;
		}
		else
		{
			m_use_hook = false;
			Log::Warning << "Failed to 'find get_ms_per_game_min', defaulting to normal game time behaviour." << Log::Endl;
			Log::Warning << "Please report this to the mod author alongside the version (Steam, Epic, Rockstar)." << Log::Endl;
			Log::Warning << "Alternatively, you can update the pattern manually by changing 'get_ms_per_game_min' in longer_days.ini" << Log::Endl;
			Log::Warning << "Current pattern: " << pattern << Log::Endl;
		}
	}

	int script::get_time_from_hour()
	{
		int hour = m_current_hour;
		return (hour >= m_config.m_day_start && hour <= m_config.m_day_end) ? m_config.m_day_time_speed : m_config.m_night_time_speed;
	}

	void script::draw_text(float x, float y, std::string str, bool centre)
	{
		HUD::SET_TEXT_SCALE(0.342f, 0.342f);
		HUD::SET_TEXT_CENTRE(centre);
		HUD::_SET_TEXT_COLOR(255, 0, 0, 255);

		const char* text = MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", str.c_str());
		HUD::_DRAW_TEXT(text, x, y);
	}

	// hooking this will cause the game to not scale time and to only use our value.
	unsigned int hooks::get_ms_per_game_min_hook()
	{
		return g_script.m_enabled ? g_script.get_time_from_hour() : hooks::get_ms_per_game_min_orig();
	}
}
