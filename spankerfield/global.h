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
		int g_width{ 1920 };
		int g_height{ 1080 };
		
		bool g_should_draw;
		bool g_fairfight;
		bool g_punkbuster;
		bool g_punkbuster_alt;
		int screenshots_ff;
		int screenshots_pb;
		bool screenshots_pb_just_taken;
		uint32_t screenshots_clean_frames{ 0 };

		Vector3 g_pred_aim_point{ 0.0f, 0.0f, 0.0f };
		bool g_has_pred_aim_point{ false };

		bool g_obs;
	};

	inline globals g_globals;
}
