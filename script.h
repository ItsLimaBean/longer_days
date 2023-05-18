#pragma once
#include "singleton.h"

namespace longer_days
{
	void initialize_script();

	class script final : public singleton<script>
	{
		friend struct hooks;
	
	public:
		script(tkn) {};

	public:
		void initialize();
		void on_tick();
		void cleanup();

	protected:
		int get_time_from_hour();

	private:
		void draw_text(float x, float y, std::string str, bool centre = false);

	protected:
		bool m_enabled{ false };

	private:
		bool m_use_hook{ true };
		bool m_show_initial{ true };

		int m_current_hour{};

		PVOID m_get_ms_per_game_min{};

		static constexpr float hour_multiplier[] = {
			1.99f,
			1.70f,
			1.50f,
			1.43f,
			1.34f,
			1.15f,
			1.00f,
			0.87f,
			0.77f,
			0.76f,
			0.752f,
			0.71f,
			0.67f,
			0.71f,
			0.752f,
			0.8f,
			0.82f,
			0.85f,
			0.953f,
			1.15f,
			1.34f,
			1.43f,
			1.54f,
			1.76f
		};
	};
}