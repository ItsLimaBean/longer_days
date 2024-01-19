#include "pch.h"
#include "config.h"

namespace longer_days
{
	bool config::load()
	{
		bool success = m_ini.LoadFile(m_config_file.c_str()) == SI_OK;
		if (success)
		{
			float day_time = static_cast<float>(std::atof(m_ini.GetValue("settings", "day_time", "2.0")));
			float night_time = static_cast<float>(std::atof(m_ini.GetValue("settings", "night_time", "2.0")));

			m_day_time_speed = abs(static_cast<int>(day_time * 1000.0f));
			m_night_time_speed = abs(static_cast<int>(night_time * 1000.0f));

			m_day_start = m_ini.GetLongValue("settings", "day_start", 6);
			m_day_end = m_ini.GetLongValue("settings", "day_end", 19);

			m_show_welcome = m_ini.GetBoolValue("settings", "show_welcome", true);

			m_get_ms_per_game_min_pattern = m_ini.GetValue("patterns", "get_ms_per_game_min", "NULL");

			Log::Info << "Loaded Config." << Log::Endl;
			Log::Info << "Day Time Speed: " << day_time << Log::Endl;
			Log::Info << "Night Time Speed: " << night_time << Log::Endl;
			Log::Info << "Day Starts: " << m_day_start << Log::Endl;
			Log::Info << "Day Ends: " << m_day_end << Log::Endl;
			Log::Info << "Show Welcome: " << m_show_welcome << Log::Endl;
			Log::Info << "get_ms_per_game_min pattern: " << m_get_ms_per_game_min_pattern << Log::Endl;

			if (m_get_ms_per_game_min_pattern == "NULL")
			{
				Log::Warning << "Missing 'get_ms_per_game_min' pattern!" << Log::Endl;
				Log::Warning << "Make sure to use the most up to date ini file." << Log::Endl;
			}
		}
		else
		{
			Log::Info << "Failed to load Longer Days config file!" << Log::Endl;
			const char* message = "Failed to load Longer Days config file."
				"Please ensure you have the file in the correct location and restart the game.";
			MessageBoxA(NULL, message, "Longer Days Error", MB_OK | MB_ICONERROR);
		}

		return success;
	}
}
