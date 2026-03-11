#include "pch.h"
//#define _DEBUG 1
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
		const config& cfg = config::get();

#pragma region Show Welcome
		
		if (m_show_initial)
		{
			const auto current_time = timeGetTime();
			static auto timeout = current_time + 8000;
			if (current_time > timeout)
				m_show_initial = false;

			if (cfg.m_show_welcome)
			{
				utility::draw_text(0.5f, 0.009f, "Longer Days " VERSION, true);
			}

			if (!m_use_hook)
			{
				utility::draw_text(0.5f, 0.039f, "~COLOR_YELLOWSTRONG~An issue occured loading Longer Days, check 'longer_days.log' for more details.", true);
				utility::draw_text(0.5f, 0.069f, "~COLOR_YELLOWSTRONG~The mod will still continue to function.", true);
			}
		}
#pragma endregion
		
		m_current_hour = CLOCK::GET_CLOCK_HOURS();

		const Player player = PLAYER::PLAYER_ID();
		const bool is_mission = MISC::GET_MISSION_FLAG();
		const bool is_playing = !HUD::IS_PAUSE_MENU_ACTIVE() && CAM::IS_SCREEN_FADED_IN()
			&& !DLC::GET_IS_LOADING_SCREEN_ACTIVE();
		const bool has_control = PLAYER::IS_PLAYER_CONTROL_ON(player) || PLAYER::IS_PLAYER_SCRIPT_CONTROL_ON(player)
			|| PAD::IS_CONTROL_ENABLED(0, 0xA987235F) || PAD::IS_CONTROL_ENABLED(0, 0x308588E6); //     FrontendPause = 0xD82E0BD2, 0x308588E6 = INPUT_GAME_MENU_CANCEL
		m_enabled = !is_mission && is_playing && has_control;

#if defined(_DEBUG) && defined(MANUAL_START_STOP)
		static bool manual_stop_start = true;
		m_enabled = manual_stop_start;
#endif

		static bool last_frame_enabled = !m_enabled;
		if (m_enabled != last_frame_enabled)
		{
			float* weight_retention_ptr = get_weight_retention_global_ptr();

			last_frame_enabled = m_enabled;
			if (!m_enabled)
			{
				
				// only reset the retention multiplier if its what we set it as
				if ((*weight_retention_ptr - 1.0f) == cfg.m_weight_retention_multiplier)
				{
					*weight_retention_ptr = 0.0f;
				}

				// Calling _SET_MILLISECONDS_PER_GAME_MINUTE with a value of 9999999
				// causes the two timers that calculate time to be equal the next time you call 
				// _SET_MILLISECONDS_PER_GAME_MINUTE, this should be a better way of fixing time jumps!
				CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(9999999);
				CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(2000);
				Log::Info << "Mod disabled: Is Mission: " << is_mission << ", Is Playing: " << is_playing << ", Has Control: " << has_control << ", Weight retention value: " << *weight_retention_ptr << Log::Endl;
				if (is_mission)
				{
					try_log_active_mission();
				}
			}
			else
			{
				// Retention Multiplier should always be 1 less than what you want
				// 	fVar0 = (fVar0 * (Global_40.f_11095.f_52 + 1f));
				// As R* scripts adds 1 to the value before multiplying
				
				*weight_retention_ptr = fmaxf(0.0f, cfg.m_weight_retention_multiplier - 1.0f);
				Log::Info << "Mod enabled: Is Mission: " << is_mission << ", Is Playing: " << is_playing << ", Has Control: " << has_control << ", Weight retention value: " << *weight_retention_ptr << Log::Endl;
			}
		}

		if (!m_use_hook && m_enabled)
		{
			CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(static_cast<int>(get_time_from_hour() * hour_multiplier[m_current_hour]));
		}

#ifdef _DEBUG // This code will only be compiled for Debug

		if (utility::is_key_pressed(0x76)) // F7
		{
			CLOCK::SET_CLOCK_TIME((CLOCK::GET_CLOCK_HOURS() + 1) % 24, 0, 0);
		}
		if (utility::is_key_pressed(0x77)) // F8
		{
			const int cur_hour = CLOCK::GET_CLOCK_HOURS();
			const int new_hour = cur_hour <= 0 ? 23 : cur_hour - 1;
			CLOCK::SET_CLOCK_TIME(new_hour, 0, 0);
		}

#if defined(_DEBUG) && defined(MANUAL_START_STOP)
		if (utility::is_key_pressed(0x78)) // F9
		{
			manual_stop_start = !manual_stop_start;
		}
#endif
		if (utility::is_key_pressed(0x79))
		{
			float* weight = (float*)getGlobalPtr(40 + 11095 + 11 + (1 + 13));
			if (*weight < 0.0f)
				*weight = 100.0f;
			else
				*weight = -75.0f;

			*(int*)getGlobalPtr(1347477 + 201) = 1;
		}


		static int last_value;
		static int change_time = timeGetTime();
		static int previous = 0;
		int mins = CLOCK::GET_CLOCK_MINUTES();
		if (last_value != mins)
		{
			previous = (timeGetTime() - change_time);
			change_time = timeGetTime();
		}

		std::stringstream str{};
		int line = 0;

#if defined(_DEBUG) && defined(MANUAL_START_STOP)
		str << "MANUAL START/STOP ENABLED";
		debug::draw_text_line(line, str);
#endif

		str << "Game Time: " << FORMAT_CLOCK(CLOCK::GET_CLOCK_HOURS(), mins, CLOCK::GET_CLOCK_SECONDS());
		debug::draw_text_line(line, str);

		str << "Last Update: " << (timeGetTime() - change_time) << "ms ago";
		debug::draw_text_line(line, str);

		str << "Previous Update time: " << previous << "ms";
		debug::draw_text_line(line, str);

		str << "FPS: " << (int)(1.0f / MISC::GET_FRAME_TIME()) << " (" << MISC::GET_FRAME_TIME() << "s)";
		debug::draw_text_line(line, str);

		str << "Is Rendering TV?: " << GRAPHICS::GET_TV_CHANNEL();
		debug::draw_text_line(line, str);

		str << "Internal Time: " << timeGetTime();
		debug::draw_text_line(line, str);

		str << "Enabled: " << m_enabled;
		debug::draw_text_line(line, str);

		str << "Target Time: " << get_time_from_hour();
		debug::draw_text_line(line, str);

		str << "Current MS Per Game Min: " << CLOCK::GET_MILLISECONDS_PER_GAME_MINUTE();
		debug::draw_text_line(line, str);

		str << "Mode: " << (m_use_hook ? "Hook" : "Native");
		debug::draw_text_line(line, str);

		float weight = *(float*)getGlobalPtr(40 + 11095 + 11 + (1 + 13));
		str << "Current Weight: " << weight;
		debug::draw_text_line(line, str);

		// Retention Multiplier should always be 1 less than what you want
		// 	fVar0 = (fVar0 * (Global_40.f_11095.f_52 + 1f));
		// As R* scripts add 1 to the value before multiplying
		float retention_mutliplier = *get_weight_retention_global_ptr();
		str << "Weight Retention Multiplier (Global): " << retention_mutliplier << " | Config: " << config::get().m_weight_retention_multiplier;
		debug::draw_text_line(line, str);

		const bool native_has_control = PLAYER::IS_PLAYER_CONTROL_ON(player) || PLAYER::IS_PLAYER_SCRIPT_CONTROL_ON(player);
		str << "Has Control (Aggregate): " << has_control << ", Game Natvies: " << native_has_control << ", Controls: (0xA987235F, 0x308588E6): " << PAD::IS_CONTROL_ENABLED(0, 0xA987235F) << ", " << PAD::IS_CONTROL_ENABLED(0, 0x308588E6);
		debug::draw_text_line(line, str);

		last_value = mins;
#endif

	}

	void script::try_log_active_mission()
	{
		__try
		{
			log_active_mission();
		}
		__except (1)
		{
			Log::Warning << "Encountered an exception when trying to read mission name." << Log::Endl;
		}
	}

	// This exists since object unwinding
	void script::log_active_mission()
	{
		if (std::string mission_label = get_current_mission_label(); mission_label.size() > 0)
		{
			Log::Info << "Mission: " << mission_label << " (" << HUD::_GET_LABEL_TEXT(mission_label.c_str()) << ")." << Log::Endl;
		}
	}

	void script::cleanup()
	{
		// This won't get called in a game thread, should be fine.
		if (!m_use_hook)
		{
			CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(9999999);
			CLOCK::_SET_MILLISECONDS_PER_GAME_MINUTE(2000);
		}

		*get_weight_retention_global_ptr() = 0.0f;
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

	const std::string script::get_current_mission_label()
	{
		// This global hasn't changed in 3 years, i doubt it'll ever change again.
		return getGameVersion() == eGameVersion::VER_AUTO ? std::string(reinterpret_cast<char*>(getGlobalPtr(1879514 + 2))) : "";
		
	}

	float* script::get_weight_retention_global_ptr()
	{
		return reinterpret_cast<float*>(getGlobalPtr(40 + 11095 + 52));
	}
}
