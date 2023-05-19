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

		/* Why do what's below?
		* Well, here's how time is calculated in RDR2.
		* Function im referencing: LAB_14074e618 (1491.18)
			
			// first we calculate the difference between the games internal timer
			// and the last time the in-game player time was updated
			uint time_difference = (uint)(game_timer - last_time_update)

			// now we loop until we can't add our ms_pergamemin onto the last_time_update
			// without it being larger than of time difference
			while (ms_pergamemin <= time_difference) {
			  last_time_update = last_time_update + ms_pergamemin;

			  // for each loop we increment the current time by 1 minute
			  increment_time(&time_date_struct,0,1,0);
			  local_hour = time_hour;
			  local_min = time_min;
			  local_sec = time_sec;
			}
			
		* Finally there's some code that'll calculate the seconds.
		* As you can see the game will play catchup if we go from a large ms_pergamemin to a small one.
		* 60000 to 2000 would mean we could jump from 12:00:00 up to a maximum of 12:30:00 in one tick!
		* So what we do is we just wait for a tick and restore the time before that tick,
		* this should fix time jumping when we disable our custom timescale.
		*/
		static bool last_frame_enabled = !m_enabled;
		if (m_enabled != last_frame_enabled)
		{
			last_frame_enabled = m_enabled;
			if (!m_enabled)
			{
				int h = CLOCK::GET_CLOCK_HOURS();
				int m = CLOCK::GET_CLOCK_MINUTES();
				int s = CLOCK::GET_CLOCK_SECONDS();
				std::uint32_t start_bitset = h << 12 | m << 6 | s;

				int tries = 10;
				while (tries > 0)
				{
					if (start_bitset != (CLOCK::GET_CLOCK_HOURS() << 12 | CLOCK::GET_CLOCK_MINUTES() << 6 | CLOCK::GET_CLOCK_SECONDS()))
						break;

					tries--;
					WAIT(0);
				}
				Log::Info << "Tried to restore time to " << FORMAT_CLOCK(h, m, s) << ", from jump to " << FORMAT_CLOCK(CLOCK::GET_CLOCK_HOURS(), CLOCK::GET_CLOCK_MINUTES(), CLOCK::GET_CLOCK_SECONDS()) << " " << tries <<  "." << Log::Endl;
				CLOCK::SET_CLOCK_TIME(h, m, s);
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

	void script::draw_text(float x, float y, std::string str, bool centre)
	{
		HUD::SET_TEXT_SCALE(0.342f, 0.342f);
		HUD::SET_TEXT_CENTRE(centre);
		HUD::_SET_TEXT_COLOR(255, 0, 0, 255);

		const char* text = MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", str.c_str());
		HUD::_DRAW_TEXT(text, x, y);
	}
}
