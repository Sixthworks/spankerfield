#include "common.h"
#include "Rendering/renderer.h"
#include "Utilities/vtablehook.h"
#include "Hooks/hooks.h"
#include "MinHook.h"
#include "global.h"
#include "settings.h"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hmod);
		
		g_globals.g_hmodule = hmod;
		g_globals.g_main_thread = CreateThread(nullptr, 0, [](PVOID) -> DWORD
		{
			g_globals.g_hwnd = FindWindowA(xorstr_("Battlefield 4"), NULL);
			while (!g_globals.g_hwnd)
				std::this_thread::sleep_for(std::chrono::seconds(1));

			auto logger_instance = std::make_unique<logger>();
			try
			{
				auto renderer_instance = std::make_unique<renderer>();
				LOG(INFO) << xorstr_("Renderer initialized.");

				g_hooking->initialize();
				LOG(INFO) << xorstr_("MinHook initialized.");

				g_hooking->enable();
				LOG(INFO) << xorstr_("Hooking enabled.");

				g_config.load();
				LOG(INFO) << xorstr_("Configuration loaded.");

				while (g_globals.g_running)
				{
					if (GetAsyncKeyState(VK_END) & 0x8000)
						g_globals.g_running = false;

					Sleep(75);
				}

				g_hooking->disable();
				LOG(INFO) << xorstr_("Hooking disabled.");

				g_hooking->uninitialize();
				LOG(INFO) << xorstr_("MinHook uninitialized.");

				Sleep(500);

				renderer_instance.reset();
				LOG(INFO) << xorstr_("Renderer uninitialized.");
			}
			catch (std::exception const& ex)
			{
				LOG(WARNING) << ex.what();
				MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
			}

			LOG(INFO) << xorstr_("Farewell!");
			logger_instance.reset();

			CloseHandle(g_globals.g_main_thread);
			FreeLibraryAndExitThread(g_globals.g_hmodule, 0);
		}, nullptr, 0, &g_globals.g_main_thread_id);
	}

	return true;
}