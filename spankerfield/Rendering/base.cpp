#include "base.h"

#ifdef _WIN64
#define GWL_WNDPROC GWLP_WNDPROC
#endif

WNDPROC oWndProc;
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static POINT cursor_coords{};
LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYUP && wParam == VK_INSERT)
	{
		ImSetup::b_isMenuVisible ^= true;
		if (cursor_coords.x + cursor_coords.y != 0)
			 SetCursorPos(cursor_coords.x, cursor_coords.y);
	}

	if (ImSetup::b_isMenuVisible)
	{
		GetCursorPos(&cursor_coords);

		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

		switch (uMsg) {
		case WM_MOUSEMOVE: return false;
		default:
			break;
		}
		return true;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

namespace ImSetup
{
	bool g_Implemented = false;
	bool b_isMenuVisible = false;

	void BeginDraw(IDXGISwapChain* pSwapChain)
	{
		if (!g_Implemented) {
			const auto pDxRenderer = DxRenderer::GetInstance();
			const auto pGameRenderer = GameRenderer::GetInstance();

			if (pGameRenderer && pDxRenderer && pDxRenderer->m_pDevice && pDxRenderer->m_pContext) {
				ImGui::CreateContext();

				ImGuiIO& io = ImGui::GetIO(); (void)io;
				io.DeltaTime = 1.0f / 60.0f;

				auto& style = ImGui::GetStyle();
				style.WindowPadding = { 10.f, 10.f };
				style.PopupRounding = 0.f;
				style.FramePadding = { 8.f, 4.f };
				style.ItemSpacing = { 10.f, 8.f };
				style.ItemInnerSpacing = { 6.f, 6.f };
				style.TouchExtraPadding = { 0.f, 0.f };
				style.IndentSpacing = 21.f;
				style.ScrollbarSize = 15.f;
				style.GrabMinSize = 8.f;
				style.WindowBorderSize = 1.f;
				style.ChildBorderSize = 0.f;
				style.PopupBorderSize = 1.f;
				style.FrameBorderSize = 0.f;
				style.TabBorderSize = 0.f;
				style.WindowRounding = 0.f;
				style.ChildRounding = 0.f;
				style.FrameRounding = 0.f;
				style.ScrollbarRounding = 0.f;
				style.GrabRounding = 0.f;
				style.TabRounding = 0.f;
				style.WindowTitleAlign = { 0.5f, 0.5f };
				style.ButtonTextAlign = { 0.5f, 0.5f };
				style.DisplaySafeAreaPadding = { 3.f, 3.f };

				auto& colors = style.Colors;
				colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 0.90f, 0.19f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
				colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
				colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.27f, 0.27f, 0.54f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
				colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.39f, 0.38f, 0.38f, 1.00f);
				colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.74f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.87f);
				colors[ImGuiCol_Header] = ImVec4(0.37f, 0.37f, 0.37f, 0.31f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.38f, 0.38f, 0.37f);
				colors[ImGuiCol_HeaderActive] = ImVec4(0.37f, 0.37f, 0.37f, 0.51f);
				colors[ImGuiCol_Separator] = ImVec4(0.38f, 0.38f, 0.38f, 0.50f);
				colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.46f, 0.46f, 0.50f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.64f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
				colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.27f, 0.86f);
				colors[ImGuiCol_TabActive] = ImVec4(0.34f, 0.34f, 0.34f, 0.86f);
				colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

				io.Fonts->AddFontDefault();
				ImFontConfig fontCfg = ImFontConfig();
				fontCfg.FontDataOwnedByAtlas = false;
				std::strcpy(fontCfg.Name, "Rubik");

				ImFont* Rubik = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 20.f, &fontCfg);
				io.FontDefault = Rubik;

				HWND Window = FindWindowA(xorstr_("Battlefield 4"), NULL);
				if (Window != NULL)
				{
					ImGui_ImplWin32_Init(Window);
					ImGui_ImplDX11_Init(pDxRenderer->m_pDevice, pDxRenderer->m_pContext);
					oWndProc = (WNDPROC)SetWindowLongPtr(Window, GWL_WNDPROC, (LONG_PTR)WndProc);

					g_Implemented = true;
				}

				printf(xorstr_("Rendering initialized.\n"));
			}
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		const auto pInput = BorderInputNode::GetInstance();
		if (IsValidPtr(pInput) && IsValidPtr(pInput->m_pMouse) && IsValidPtr(pInput->m_pMouse->m_pDevice))
			if (!pInput->m_pMouse->m_pDevice->m_CursorMode)
				pInput->m_pMouse->m_pDevice->m_UIOwnsInput, pInput->m_pMouse->m_pDevice->m_UseRawMouseInput, ImGui::GetIO().MouseDrawCursor = b_isMenuVisible;
	}

	void Menu()
	{
		if (!b_isMenuVisible)
			return;

		auto& style = ImGui::GetStyle();
		ImGui::SetNextWindowSize({ 690, 710 });
		ImGui::SetNextWindowBgAlpha(1.f);

		if (ImGui::Begin(xorstr_("Battlefield 4 Hack##"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			ImGui::BeginTabBar(xorstr_("tabbar"));

			// Global
			if (ImGui::BeginTabItem(xorstr_("Test")))
			{
				static bool demo_bool = true;
				static int demo_int = 1;
				static float demo_float = 1.f;

				static const char* demo_combo[]
				{
					"One",
					"Two",
					"Three"
				};
				static int demo_combo_pos = 0;

				ImGui::Checkbox("Bool", &demo_bool);
				ImGui::SliderInt("Int", &demo_int, 0, 10);
				ImGui::SliderFloat("Float", &demo_float, 0.f, 10.f);
				ImGui::Combo("Combo", &demo_combo_pos, demo_combo, sizeof(demo_combo) / sizeof(*demo_combo));
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	void EndDraw()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void KillImgui()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();
	}
}