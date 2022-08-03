#pragma once
#include "SDK/sdk.h"

namespace big
{
	class globals
	{
	public:
		HWND g_hwnd{};
		HANDLE g_main_thread{};
		DWORD g_main_thread_id{};
		HINSTANCE g_hmodule{};

		Matrix g_viewproj{};
		bool g_running{ true };
		int g_width{ 2560 };
		int g_height{ 1440 };
		
		bool g_fairfight;
		bool g_punkbuster;
		bool g_obs;

		ULONGLONG g_obscheck{};
		ULONGLONG g_blcheck{};
	};

	inline globals g_globals;
}
