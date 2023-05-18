#include "pch.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	using namespace longer_days;
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
		{
			Log::Push(new Log::FileStream(L".\\longer_days.log"));
			Log::Info << "Longer Days " VERSION << Log::Endl;

			if (!config::get().load())
			{
				return FALSE;
			}

			memory& memory = memory::get();
			if (memory.scan())
			{
				if (!memory.enable_hook())
				{
					return FALSE;
				}
			}

			scriptRegister(module, &initialize_script);
#ifdef _DEBUG
			if (AttachConsole(GetCurrentProcessId()) == false)
			{
				AllocConsole();
			}
			Log::Push(new Log::ConsoleStream());
#endif
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			memory::get().disable_hook();
			script::get().cleanup();
			scriptUnregister(module);
			Log::Info << "Unregistered script!" << Log::Endl;
#ifdef _DEBUG
			FreeConsole();
#endif
			break;
		}
		default:
			break;
	}

    return TRUE;
}

