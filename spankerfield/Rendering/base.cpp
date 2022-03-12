#include "base.h"
#include "../settings.h"
#include "../global.h"

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
				style.FrameRounding = 4.0f;
				style.WindowBorderSize = 1.0f;
				style.PopupBorderSize = 1.0f;
				style.GrabRounding = 4.0f;
				style.WindowPadding = { 10.f, 10.f };
				style.PopupRounding = 0.f;
				style.FramePadding = { 8.f, 4.f };
				style.ItemSpacing = { 10.f, 8.f };
				style.ItemInnerSpacing = { 6.f, 6.f };
				style.TouchExtraPadding = { 0.f, 0.f };
				style.IndentSpacing = 21.f;
				style.ScrollbarSize = 15.f;
				style.GrabMinSize = 8.f;
				style.ChildBorderSize = 0.f;
				style.FrameBorderSize = 0.f;
				style.TabBorderSize = 0.f;
				style.WindowRounding = 4.f;
				style.ChildRounding = 3.f;
				style.ScrollbarRounding = 4.f;
				style.TabRounding = 4.f;
				style.WindowTitleAlign = { 0.5f, 0.5f };
				style.ButtonTextAlign = { 0.5f, 0.5f };
				style.DisplaySafeAreaPadding = { 3.f, 3.f };

				ImVec4* colors = style.Colors;
				colors[ImGuiCol_Text] = ImColor(255, 255, 255);
				colors[ImGuiCol_TextDisabled] = ImColor(42, 42, 42);
				colors[ImGuiCol_WindowBg] = ImColor(18, 12, 27);
				colors[ImGuiCol_ChildBg] = ImColor(18, 12, 27);
				colors[ImGuiCol_PopupBg] = ImColor(18, 12, 27);
				colors[ImGuiCol_Border] = ImColor(35, 32, 40);
				colors[ImGuiCol_BorderShadow] = ImColor(35, 32, 40);
				colors[ImGuiCol_FrameBg] = ImColor(35, 42, 106);
				colors[ImGuiCol_FrameBgHovered] = ImColor(35, 42, 106);
				colors[ImGuiCol_FrameBgActive] = ImColor(34, 122, 180);
				colors[ImGuiCol_TitleBg] = ImColor(39, 51, 125);
				colors[ImGuiCol_TitleBgActive] = ImColor(39, 51, 125);
				colors[ImGuiCol_TitleBgCollapsed] = ImColor(39, 51, 125);
				colors[ImGuiCol_MenuBarBg] = ImColor(39, 51, 125);
				colors[ImGuiCol_ScrollbarBg] = ImColor(18, 12, 27);
				colors[ImGuiCol_ScrollbarGrab] = ImColor(18, 12, 27);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(34, 122, 180);
				colors[ImGuiCol_ScrollbarGrabActive] = ImColor(34, 122, 180);
				colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255);
				colors[ImGuiCol_SliderGrab] = ImColor(33, 107, 167);
				colors[ImGuiCol_SliderGrabActive] = ImColor(27, 100, 151);
				colors[ImGuiCol_Button] = ImColor(33, 43, 105);
				colors[ImGuiCol_ButtonHovered] = ImColor(29, 100, 150);
				colors[ImGuiCol_ButtonActive] = ImColor(27, 100, 151);
				colors[ImGuiCol_Header] = ImColor(34, 122, 180);
				colors[ImGuiCol_HeaderHovered] = ImColor(29, 100, 150);
				colors[ImGuiCol_HeaderActive] = ImColor(34, 122, 180);
				colors[ImGuiCol_Separator] = ImColor(46, 46, 46);
				colors[ImGuiCol_SeparatorHovered] = ImColor(46, 46, 46);
				colors[ImGuiCol_SeparatorActive] = ImColor(46, 46, 46);
				colors[ImGuiCol_ResizeGrip] = ImColor(46, 46, 46);
				colors[ImGuiCol_ResizeGripHovered] = ImColor(29, 100, 150);
				colors[ImGuiCol_ResizeGripActive] = ImColor(27, 100, 151);
				colors[ImGuiCol_Tab] = ImColor(33, 43, 105);
				colors[ImGuiCol_TabHovered] = ImColor(34, 122, 180);
				colors[ImGuiCol_TabActive] = ImColor(34, 122, 180);
				colors[ImGuiCol_TabUnfocused] = ImColor(33, 43, 105);
				colors[ImGuiCol_TabUnfocusedActive] = ImColor(34, 122, 180);

				io.Fonts->AddFontDefault();
				ImFontConfig fontCfg = ImFontConfig();
				fontCfg.FontDataOwnedByAtlas = false;
				std::strcpy(fontCfg.Name, xorstr_("Rubik"));

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

			if (ImGui::BeginTabItem(xorstr_("Main")))
			{
				ImGui::Checkbox(xorstr_("Draw blacklisted players"), &settings::blacklist);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw ESP"), &settings::draw_box);
				ImGui::Checkbox(xorstr_("Draw box"), &settings::draw_box);
				ImGui::SliderInt(xorstr_("Box style"), &settings::box_style, 1, 6);

				ImGui::Text(xorstr_("Box color"));
				static float box_colors_oc[4] = { settings::box_color_occluded.Value.x, settings::box_color_occluded.Value.y, settings::box_color_occluded.Value.z, settings::box_color_occluded.Value.w };
				if (ImGui::ColorEdit4(xorstr_("Not visible##BX"), box_colors_oc))
					settings::box_color_occluded = { box_colors_oc[0], box_colors_oc[1], box_colors_oc[2], box_colors_oc[3] };

				static float box_colors[4] = { settings::box_color.Value.x, settings::box_color.Value.y, settings::box_color.Value.z, settings::box_color.Value.w };
				if (ImGui::ColorEdit4(xorstr_("Visible##BX"), box_colors))
					settings::box_color = { box_colors[0], box_colors[1], box_colors[2], box_colors[3] };

				ImGui::Checkbox(xorstr_("Draw health"), &settings::draw_health);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw name"), &settings::draw_name);

				ImGui::Checkbox(xorstr_("Draw distance"), &settings::draw_distance);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw skeleton"), &settings::draw_skeleton);

				ImGui::Text(xorstr_("Text color"));
				static float text_color_oc[4] = { settings::text_color_occluded.Value.x, settings::text_color_occluded.Value.y, settings::text_color_occluded.Value.z, settings::text_color_occluded.Value.w };
				if (ImGui::ColorEdit4(xorstr_("Not visible##TX"), text_color_oc))
					settings::text_color_occluded = { text_color_oc[0], text_color_oc[1], text_color_oc[2], text_color_oc[3] };

				static float text_colors[4] = { settings::text_color.Value.x, settings::text_color.Value.y, settings::text_color.Value.z, settings::text_color.Value.w };
				if (ImGui::ColorEdit4(xorstr_("Visible##TX"), text_colors))
					settings::text_color = { text_colors[0], text_colors[1], text_colors[2], text_colors[3] };

				ImGui::Text(xorstr_("Skeleton color"));
				static float skeleton_color[4] = { settings::skeleton_color.Value.x, settings::skeleton_color.Value.y, settings::skeleton_color.Value.z, settings::skeleton_color.Value.w };
				if (ImGui::ColorEdit4(xorstr_("##SKC"), skeleton_color))
					settings::skeleton_color = { skeleton_color[0], skeleton_color[1], skeleton_color[2], skeleton_color[3] };

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Radar"), &settings::radar);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("X##RDR"), &settings::radar_x, 0, globals::g_Width);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Y##RDR"), &settings::radar_y, 0, globals::g_Height);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw explosives"), &settings::explosives);
				ImGui::Checkbox(xorstr_("Jet speed"), &settings::jet_speed);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Spectator list"), &settings::spectator_list);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("X##SP"), &settings::spectator_x, 0, globals::g_Width);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Y##SP"), &settings::spectator_y, 0, globals::g_Height);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Auto-spot"), &settings::minimap);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Unspot when OBS"), &settings::obs_check);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Debug")))
			{
				ImGui::Checkbox(xorstr_("Spectator list dummies"), &settings::spectator_list_debug);

				ImGui::EndTabItem();
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