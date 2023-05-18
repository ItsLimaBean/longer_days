#pragma once
#include "SimpleIni.h"
#include "singleton.h"

namespace longer_days
{
	class config final : public singleton<config>
	{
	public:
		config(tkn) {};

	public:
		bool load();

	public:
		int m_day_time_speed{};
		int m_night_time_speed{};
		int m_day_start{};
		int m_day_end{};
		bool m_show_welcome{};

		std::string m_get_ms_per_game_min_pattern{};

	private:
		std::string m_config_file{ ".\\longer_days.ini" };
		CSimpleIniA m_ini;
	};
}