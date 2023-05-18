#pragma once
#include "singleton.h"

#define MH_FORMAT_ERR(x) "(" << MH_StatusToString(status) << ")"

namespace longer_days
{
	class memory final : public singleton<memory> 
	{
	public:
		memory(tkn) {};

	public:
		bool scan();

		bool enable_hook();
		void disable_hook();

		bool found_ptr() const
		{
			return m_get_ms_per_game_min != nullptr;
		}

	public:
		bool m_hooks_enabled{ false };
	protected:
		PVOID m_get_ms_per_game_min{};
	};

	struct hooks
	{
		inline static unsigned int(__fastcall* get_ms_per_game_min_orig)();
		static unsigned int get_ms_per_game_min_hook();
	};
}