#include "gui.h"
#include "renderer.h"
#include "bone_manager.h"
#include "../common.h"
#include "../global.h"
#include "../settings.h"
#include "../ImGui/imgui.h"
#include "../Features/main.h"
#include "../Utilities/other.h"

namespace big
{
	void gui::dx_init()
	{
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

		ImVec4* colors = ImGui::GetStyle().Colors;
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

		ImGui::SetNextWindowSize(ImVec2(800, 710));
	}

	void gui::dx_on_tick()
	{
		if (ImGui::Begin(xorstr_("Spankerfield")))
		{
			ImGui::BeginTabBar(xorstr_("Tab bar"));

			if (ImGui::BeginTabItem(xorstr_("Weapons")))
			{
				ImGui::Checkbox(xorstr_("Aimbot"), &g_settings.aimbot);
				ImGui::Checkbox(xorstr_("FOV target selection"), &g_settings.aim_fov_method);
				if (g_settings.aim_fov_method)
				{
					ImGui::Separator();

					ImGui::Checkbox(xorstr_("Draw FOV"), &g_settings.aim_draw_fov);

					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("FOV (pixels)##Aimbot"), &g_settings.aim_fov, 0.f, 10000.f);
					ImGui::PopItemWidth();

					ImGui::Separator();
				}

				ImGui::Text(xorstr_("Aim bone"));

				static const char* text = g_settings.aim_bone < bone_map.size() ? bone_map[(UpdatePoseResultData::BONES)g_settings.aim_bone].c_str() : xorstr_("Unknown");
				ImGui::PushItemWidth(300.f);
				if (ImGui::DragInt(xorstr_("##BS"), &g_settings.aim_bone, 1.0f, 0, (int)(bone_map.size() - 1)))
				{
					if (g_settings.aim_bone >= 0 && g_settings.aim_bone < bone_map.size())
					{
						auto it = bone_map.begin();
						std::advance(it, g_settings.aim_bone);

						g_settings.aim_bone = it->first;
						text = it->second.c_str();
					}
				}
				ImGui::PopItemWidth();

				if (text != "")
				{
					ImGui::SameLine();
					ImGui::Text(text);
				}

				ImGui::Separator();

				ImGui::Text(xorstr_("Smoothing"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Minimum time to target (seconds)##Aimbot"), &g_settings.aim_min_time_to_target, 0.f, g_settings.aim_max_time_to_target);
				ImGui::SliderFloat(xorstr_("Maximum time to target (seconds)##Aimbot"), &g_settings.aim_max_time_to_target, g_settings.aim_min_time_to_target, 10.f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Overheat control"), &g_settings.overheat_control);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Critical overheat value"), &g_settings.overheat_control_critical, 0.f, 1.f);
				ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Visuals")))
			{
				ImGui::Checkbox(xorstr_("Draw ESP"), &g_settings.esp);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Distance##ESP"), &g_settings.esp_distance, 1.f, 10000.f);
				ImGui::PopItemWidth();

				ImGui::Checkbox(xorstr_("Draw teammates##ESP"), &g_settings.esp_draw_teammates);
				
				ImGui::Text(xorstr_("Teammates color"));

				color_wrapper(xorstr_("Teammate##ESP"), &g_settings.esp_teammate_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw box"), &g_settings.esp_draw_box);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Box style"), &g_settings.esp_box_style, 1, 6);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Box color"));

				color_wrapper(xorstr_("Not visible##BX"), &g_settings.esp_box_color_occluded);
				color_wrapper(xorstr_("Visible##BX"), &g_settings.esp_box_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw health"), &g_settings.esp_draw_health);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw name"), &g_settings.esp_draw_name);

				ImGui::Checkbox(xorstr_("Draw distance"), &g_settings.esp_draw_distance);
				
				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw explosives"), &g_settings.explosives);

				ImGui::Text(xorstr_("Explosives color"));

				color_wrapper(xorstr_("Explosives##BX"), &g_settings.explosives_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw skeleton"), &g_settings.skeleton);
				ImGui::Checkbox(xorstr_("Use skeleton dots"), &g_settings.skeleton_use_dots);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Dots distance"), &g_settings.skeleton_dots_distance, 1.f, 5000.f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text(xorstr_("Text color"));

				color_wrapper(xorstr_("Not visible##TX"), &g_settings.text_color_occluded);
				color_wrapper(xorstr_("Visible##TX"), &g_settings.text_color);

				ImGui::Text(xorstr_("Skeleton color"));

				color_wrapper(xorstr_("Bones##SKC"), &g_settings.skeleton_color);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Radar")))
			{
				ImGui::Checkbox(xorstr_("Radar"), &g_settings.radar);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Distance##RDR"), &g_settings.radar_distance, 1.f, 10000.f);
				ImGui::PopItemWidth();

				ImGui::Checkbox(xorstr_("Draw teammates##RDR"), &g_settings.radar_draw_teammates);

				ImGui::Separator();

				ImGui::Text(xorstr_("Positioning"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("X##RDR"), &g_settings.radar_x, 0.f, (float)g_globals.g_width);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Y##RDR"), &g_settings.radar_y, 0.f, (float)g_globals.g_height);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text(xorstr_("Colors"));

				color_wrapper(xorstr_("Teammates##RDR"), &g_settings.radar_teammates_color);
				color_wrapper(xorstr_("Ememies##RDR"), &g_settings.radar_enemies_color);
				color_wrapper(xorstr_("Teammate vehicles##RDR"), &g_settings.radar_teammate_vehicles_color);
				color_wrapper(xorstr_("Ememy vehicles##RDR"), &g_settings.radar_enemy_vehicles_color);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Spectator list")))
			{
				ImGui::Checkbox(xorstr_("Spectator list"), &g_settings.spectator_list);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Raw drawing"), &g_settings.spectator_raw_drawing);

				ImGui::Separator();

				ImGui::Text(xorstr_("Positioning"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("X##SP"), &g_settings.spectator_x, 0.f, (float)g_globals.g_width);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Y##SP"), &g_settings.spectator_y, 0.f, (float)g_globals.g_height);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text(xorstr_("Colors"));

				color_wrapper(xorstr_("Text##SPC"), &g_settings.spectator_color);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Infantry alert")))
			{
				ImGui::Checkbox(xorstr_("Alert of nearby enemies"), &g_settings.infantry_alert);
				ImGui::Checkbox(xorstr_("Count infantry vehicles"), &g_settings.infantry_alert_light_tech);

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Alert distance"), &g_settings.infantry_alert_distance, 1.f, 500.f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text(xorstr_("Colors"));
				color_wrapper(xorstr_("Text##IALRT"), &g_settings.infantry_alert_color);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Misc")))
			{
				ImGui::Checkbox(xorstr_("Auto-spot"), &g_settings.minimap);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Unspot when using OBS"), &g_settings.obs_check);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Auto jet speed"), &g_settings.jet_speed);


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Blacklist")))
			{
				ImGui::Checkbox(xorstr_("Draw blacklisted players"), &g_settings.blacklist);

				ImGui::Separator();

				if (ImGui::TreeNode(xorstr_("Blacklisted players")))
				{
					int i = 0;
					for (const auto& rs : plugins::blacklisted)
					{
						if (ImGui::Selectable(rs.c_str(), false))
						{
							plugins::selected = i;
					    }

						i++;
					}

					ImGui::TreePop();
				}

				if (ImGui::Button(xorstr_("Delete player from blacklist")))
				{
					std::string nickname = plugins::blacklisted.at(plugins::selected);
					if (!nickname.empty())
						plugins::delete_from_blacklist(nickname);
				}

				ImGui::Separator();

				static char nick[50]{};
				ImGui::PushItemWidth(300.f);
				ImGui::InputText(xorstr_("Nickname"), nick, IM_ARRAYSIZE(nick));
				ImGui::PopItemWidth();

				if (ImGui::Button(xorstr_("Add player to blacklist")))
					plugins::add_to_blacklist(nick);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Settings")))
			{
				ImGui::Separator();
				
				ImGui::Text(xorstr_("Config"));

				if (ImGui::Button(xorstr_("Load")))
					g_config.load();

				ImGui::SameLine();

				if (ImGui::Button(xorstr_("Save")))
					g_config.attempt_save();

				ImGui::Separator();

				if (ImGui::Button(xorstr_("Unload")))
					g_globals.g_running = false;

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}
