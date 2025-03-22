#include "common.h"
#include "Rendering/renderer.h"
#include "Utilities/thread_pool.h"
#include "Hooks/hooks.h"
#include "global.h"
#include "settings.h"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID reserved)
{
	using namespace big;

	// Wait for D3D11 before proceeding, after it's up everything else should be too
	while (!GetModuleHandleW(L"d3d11.dll"))
		std::this_thread::sleep_for(std::chrono::seconds(5));

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
			std::unique_ptr<renderer> renderer_instance;
			std::unique_ptr<thread_pool> thread_pool_instance;

			try
			{
				g_config.refresh_configs();
				g_config.load(xorstr_("default"));
				LOG(INFO) << xorstr_("Configuration system initialized.");

				renderer_instance = std::make_unique<renderer>();
				LOG(INFO) << xorstr_("Renderer initialized.");

				thread_pool_instance = std::make_unique<thread_pool>();
				LOG(INFO) << xorstr_("Thread pool initialized.");

				g_hooking->initialize();
				LOG(INFO) << xorstr_("MinHook initialized.");

				g_hooking->enable();
				LOG(INFO) << xorstr_("Hooking enabled.");

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

				renderer_instance.reset();
				LOG(INFO) << xorstr_("Renderer uninitialized.");

				g_thread_pool->destroy();
				thread_pool_instance.reset();
				LOG(INFO) << xorstr_("Thread pool uninitialized.");

				Sleep(500);
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