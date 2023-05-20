#include "pch.h"

namespace longer_days
{
	void initialize_script()
	{
		script& script = script::get();

		script.initialize();
		while (true)
		{
			script.on_tick();
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

			if (config::get().m_show_welcome)
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

#if defined(_DEBUG) && defined(MANUAL_START_STOP)
		static bool _do = true;
		m_enabled = _do;
#endif

		static bool last_frame_enabled = !m_enabled;
		if (m_enabled != last_frame_enabled)
		{
			last_frame_enabled = m_enabled;
			if (!m_enabled)
			{
				// Calling _SET_MILLISECONDS_PER_GAME_MINUTE with a value of 9999999
				// causes the two timers that calculate time to be equal the next time you call 
				// _SET_MILLISECONDS_PER_GAME_MINUTE, this should be a better way of fixing time jumps!
				CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(9999999);
				CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(2000);
				Log::Info << "Mod disabled: Is Mission: " << is_mission << ", Is Playing: " << is_playing << ", Has Control: " << has_control << Log::Endl;
				if (is_mission)
				{
					const char* mission_label = get_current_mission_label();
					Log::Info << "Mission: " << mission_label << " (" << HUD::_GET_LABEL_TEXT(mission_label) << ")." << Log::Endl;
				}
			}
			else
			{
				Log::Info << "Mod enabled: Is Mission: " << is_mission << ", Is Playing: " << is_playing << ", Has Control: " << has_control << Log::Endl;
			}
		}

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

#if defined(_DEBUG) && defined(MANUAL_START_STOP)
		if (IsKeyPressed(0x78) && (timeGetTime() - key_tmr) >= 200) // F9
		{
			key_tmr = timeGetTime();
			_do = !_do;
		}
#endif


		static int last_value;
		static int change_time = timeGetTime();
		static int previous = 0;
		int mins = CLOCK::GET_CLOCK_MINUTES();
		if (last_value != mins)
		{
			previous = (timeGetTime() - change_time);
			change_time = timeGetTime();
		}

		std::stringstream str;

#if defined(_DEBUG) && defined(MANUAL_START_STOP)
		str.str(std::string()); str << "MANUAL START/STOP ENABLED";
		draw_text(0.0f, 0.06f, str.str());
#endif

		str.str(std::string()); str << "Game Time: " << FORMAT_CLOCK(CLOCK::GET_CLOCK_HOURS(), mins, CLOCK::GET_CLOCK_SECONDS());
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

		str.str(std::string()); str << "Mode: " << (m_use_hook ? "Hook" : "Native");
		draw_text(0.0f, 0.36f, str.str());

		last_value = mins;
#endif

	}

	void script::cleanup()
	{
		// This won't get called in a game thread, should be fine.
		if (!m_use_hook)
		{
			CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(9999999);
			CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(2000);
		}
	}

	void script::initialize()
	{
		Log::Info << "Current ms per game minute: " << CLOCK::GET_MILLISECONDS_PER_GAME_MINUTE() << "." << Log::Endl;
		m_use_hook = memory::get().found_ptr();
		Log::Info << "Time scaling mode: " << (m_use_hook ? "Hook" : "Native") << "." << Log::Endl;
	}

	int script::get_time_from_hour()
	{
		config& config = config::get();
		int hour = m_current_hour;
		return (hour >= config.m_day_start && hour <= config.m_day_end) ? config.m_day_time_speed : config.m_night_time_speed;
	}

	const char* script::get_current_mission_label()
	{
		// This global hasn't changed in 3 years, i doubt it'll ever change again.
		return getGameVersion() == eGameVersion::VER_AUTO ? (const char*)getGlobalPtr(1879514 + 2) : "";
		
	}

	void script::draw_text(float x, float y, std::string str, bool centre)
	{
		HUD::SET_TEXT_SCALE(0.342f, 0.342f);
		HUD::SET_TEXT_CENTRE(centre);
		HUD::_SET_TEXT_COLOR(255, 0, 0, 255);

		const char* text = MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", str.c_str());
		HUD::_DRAW_TEXT(text, x, y);
	}
}
