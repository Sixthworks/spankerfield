#pragma once
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../SDK/sdk.h"
#include "../Utilities/xorstr.h"
#include "rubik.h"
#include <Windows.h>
#include <d3d11.h>

namespace ImSetup
{
	extern bool g_Implemented;
	extern bool b_isMenuVisible;

	void BeginDraw(IDXGISwapChain* pSwapChain);
	void Fix_UI();
	void Menu();
	void EndDraw();
	void KillImgui();
}