#include "pch.h"
#include "memory.h"

namespace longer_days
{
	bool memory::scan()
	{
		const std::uintptr_t module_base = Module(nullptr).Base().As<std::uintptr_t>();
		Log::Info << "RDR2.exe base: " << HEX_UPPER(module_base) << Log::Endl;

		const char* pattern = config::get().m_get_ms_per_game_min_pattern.c_str();
		m_get_ms_per_game_min = Signature(pattern).Scan().As<PVOID>();

		uintptr_t offset = reinterpret_cast<std::uintptr_t>(m_get_ms_per_game_min) - module_base;
		if (offset == 0) m_get_ms_per_game_min = nullptr;

		bool found_ptr = m_get_ms_per_game_min;
		if (found_ptr)
		{
			Log::Info << "Found 'get_ms_per_game_min' at RDR2.exe+0x" << std::hex << std::uppercase << offset << std::nouppercase << "." << Log::Endl;
		}
		else
		{
			Log::Warning << "Failed to find 'get_ms_per_game_min', defaulting to normal game time behaviour." << Log::Endl;
			Log::Warning << "Please report this to the mod author alongside the version (Steam, Epic, Rockstar)." << Log::Endl;
			Log::Warning << "Alternatively, you can update the pattern manually by changing 'get_ms_per_game_min' in longer_days.ini" << Log::Endl;
			Log::Warning << "Current pattern: " << pattern << Log::Endl;
		}

		return found_ptr;
	}

	bool memory::enable_hook()
	{
		MH_STATUS status = MH_Initialize();
		if (status != MH_OK)
		{
			Log::Error << "Failed to initialize MinHook " << MH_FORMAT_ERR(status) << "." << Log::Endl;
			return false;
		}
		Log::Info << "Initialized MinHook." << Log::Endl;

		status = MH_CreateHook(m_get_ms_per_game_min, &hooks::get_ms_per_game_min_hook, (PVOID*)&hooks::get_ms_per_game_min_orig);
		if (status != MH_OK)
		{
			Log::Error << "Failed to create hook for 'm_get_ms_per_game_min' " << MH_FORMAT_ERR(status) << "." << Log::Endl;
			return false;
		}
		Log::Info << "Created hook for 'm_get_ms_per_game_min'." << Log::Endl;


		status = MH_EnableHook(m_get_ms_per_game_min);
		if (status != MH_OK)
		{
			Log::Error << "Failed to enable hook for 'm_get_ms_per_game_min' " << MH_FORMAT_ERR(status) << "." << Log::Endl;
			return false;
		}
		Log::Info << "Enabled hook for 'm_get_ms_per_game_min'." << Log::Endl;

		m_hooks_enabled = true;
		return true;
	}

	void memory::disable_hook()
	{
		MH_STATUS status;
		if (m_hooks_enabled)
		{
			status = MH_DisableHook(m_get_ms_per_game_min);
			if (status != MH_OK)
				Log::Warning << "Failed to disable hook for 'm_get_ms_per_game_min' " << MH_FORMAT_ERR(status) << "." << Log::Endl;
			else
				Log::Info << "Disabled hook for 'm_get_ms_per_game_min'." << Log::Endl;

			status = MH_RemoveHook(m_get_ms_per_game_min);
			if (status != MH_OK)
				Log::Warning << "Failed to remove hook for 'm_get_ms_per_game_min' " << MH_FORMAT_ERR(status) << "." << Log::Endl;
			else
				Log::Info << "Removed hook for 'm_get_ms_per_game_min'." << Log::Endl;
			
			m_hooks_enabled = false;
		}
		status = MH_Uninitialize();
		if (status != MH_OK)
			Log::Warning << "Failed to uninitialize MinHook " << MH_FORMAT_ERR(status) << "." << Log::Endl;
		else
			Log::Info << "Uninitialized MinHook." << Log::Endl;
	}

	// hooking this will cause the game to not scale time and to only use our value.
	unsigned int hooks::get_ms_per_game_min_hook()
	{
		script& script = script::get();
		return script.m_enabled ? script.get_time_from_hour() : hooks::get_ms_per_game_min_orig();
	}
}
