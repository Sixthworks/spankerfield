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

		ImGui::SetNextWindowSize(ImVec2(850, 750));
	}

	void gui::dx_on_tick()
	{
		if (ImGui::Begin(xorstr_("Spankerfield")))
		{
			ImGui::BeginTabBar(xorstr_("Tab bar"));

			if (ImGui::BeginTabItem(xorstr_("Weapons")))
			{
				ImGui::Checkbox(xorstr_("Aimbot"), &g_settings.aimbot);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("FOV target selection"), &g_settings.aim_fov_method);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("No recoil"), &g_settings.no_recoil);

				if (g_settings.aim_fov_method)
				{
					ImGui::Separator();

					ImGui::Checkbox(xorstr_("Draw FOV"), &g_settings.aim_draw_fov);

					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("FOV (pixels)##Aimbot"), &g_settings.aim_fov, 0.f, (float)g_globals.g_height);
					ImGui::PopItemWidth();

					ImGui::Separator();
				}

				ImGui::Text(xorstr_("Aim key"));
				ImGui::PushItemWidth(300.f);
				ImGui::InputInt("Key (info on keys in thread)##Aimbot", &g_settings.aim_key);
				ImGui::PopItemWidth();

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

				// Not the best way
				ImGui::Text(xorstr_("Weapon editor"));

				static bool enable_editor = false;
				ImGui::Checkbox(xorstr_("Enable weapon editor"), &enable_editor);

				if (enable_editor)
				{
					const auto data = get_weapon_firing();
					if (IsValidPtrWithVTable(data))
					{
						ImGui::PushItemWidth(300.f);
						ImGui::InputInt(xorstr_("Bullets per shot##WP"), &data->m_ShotConfigData.m_BulletsPerShot, 1, 5);
						ImGui::InputInt(xorstr_("Bullets per burst##WP"), &data->m_ShotConfigData.m_BulletsPerBurst, 1, 5);
						ImGui::InputInt(xorstr_("Bullets per shell##WP"), &data->m_ShotConfigData.m_BulletsPerShell, 1, 5);
						ImGui::PopItemWidth();

						ImGui::Text(xorstr_("Speed"));
						ImGui::PushItemWidth(300.f);
						ImGui::InputFloat(xorstr_("X##WP"), &data->m_ShotConfigData.m_Speed.x, 0.5f, 10.f);
						ImGui::InputFloat(xorstr_("Y##WP"), &data->m_ShotConfigData.m_Speed.y, 0.5f, 10.f);
						ImGui::InputFloat(xorstr_("Z##WP"), &data->m_ShotConfigData.m_Speed.w, 0.5f, 10.f);
						ImGui::InputFloat(xorstr_("W##WP"), &data->m_ShotConfigData.m_Speed.w, 0.5f, 10.f);
						ImGui::PopItemWidth();

						ImGui::Text(xorstr_("Other"));
						ImGui::PushItemWidth(300.f);
						ImGui::InputFloat(xorstr_("Gravity##WP"), &data->m_ShotConfigData.m_ProjectileData->m_Gravity, 0.5f, 10.f);
						ImGui::PopItemWidth();
					}
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Visuals")))
			{
				ImGui::Checkbox(xorstr_("Draw ESP"), &g_settings.esp);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Distance##ESP"), &g_settings.esp_distance, 1.f, 10000.f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw vehicles##ESP"), &g_settings.esp_draw_vehicles);
				ImGui::Checkbox(xorstr_("Draw teammates##ESP"), &g_settings.esp_draw_teammates);
				
				ImGui::Text(xorstr_("Teammates color"));

				color_wrapper(xorstr_("Teammate##ESP"), &g_settings.esp_teammate_color);

				ImGui::Text(xorstr_("Additional tags color"));

				color_wrapper(xorstr_("Tags##ESP"), &g_settings.esp_additional_tags_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw box"), &g_settings.esp_draw_box);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Box style"), &g_settings.esp_box_style, 1, 6);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Box color"));

				color_wrapper(xorstr_("Not visible##BX"), &g_settings.esp_box_color_occluded);
				color_wrapper(xorstr_("Visible##BX"), &g_settings.esp_box_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw line"), &g_settings.esp_draw_line);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Line point location"), &g_settings.esp_draw_line_from, 0, 8);
				ImGui::SliderFloat(xorstr_("Line thickness"), &g_settings.esp_line_thickness, 0.1f, 10.f);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Line color"));

				color_wrapper(xorstr_("Not visible##LC"), &g_settings.esp_line_color);
				color_wrapper(xorstr_("Visible##LC"), &g_settings.esp_line_color_occluded);

				ImGui::Separator();
				
				ImGui::Text(xorstr_("Text spacing"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Text spacing##ESP"), &g_settings.esp_text_spacing, 0.f, 50.f);
				ImGui::PopItemWidth();

				ImGui::Spacing();

				ImGui::Checkbox(xorstr_("Draw health"), &g_settings.esp_draw_health);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw name"), &g_settings.esp_draw_name);

				ImGui::Checkbox(xorstr_("Draw distance"), &g_settings.esp_draw_distance);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw vehicle tag"), &g_settings.esp_draw_vehicle_tag);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw explosives"), &g_settings.explosives);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw laser guided missiles"), &g_settings.missiles_own);

				ImGui::Text(xorstr_("Colors"));

				color_wrapper(xorstr_("Explosives##BX"), &g_settings.explosives_color);
				color_wrapper(xorstr_("Missiles##BX"), &g_settings.missiles_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw crosshair"), &g_settings.draw_crosshair);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw shadow"), &g_settings.crosshair_shadow);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw in vehicles"), &g_settings.crosshair_in_vehicles);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Crosshair size"), &g_settings.crosshair_size, 0.1f, 100.f);
				ImGui::SliderFloat(xorstr_("Crosshair thickness"), &g_settings.crosshair_thickness, 1.f, 100.f);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Colors"));
				color_wrapper(xorstr_("Crosshair##CS"), &g_settings.crosshair_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw skeleton"), &g_settings.skeleton);
				ImGui::SameLine();
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

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw personal health bar"), &g_settings.draw_health_bar);
				ImGui::Checkbox(xorstr_("Player health"), &g_settings.health_bar_soldier);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Vehicle health"), &g_settings.health_bar_vehicle);

				ImGui::Text(xorstr_("Size"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Width##HB"), &g_settings.health_bar_width, 1.f, (float)g_globals.g_width);
				ImGui::SliderFloat(xorstr_("Height##HB"), &g_settings.health_bar_height, 1.f, (float)g_globals.g_height);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text(xorstr_("Position"));

				ImGui::Checkbox(xorstr_("Use centered X position"), &g_settings.health_bar_use_center_x);
				if (!g_settings.health_bar_use_center_x)
				{
					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("X##HB"), &g_settings.health_bar_x, 1.f, (float)g_globals.g_width);
					ImGui::PopItemWidth();

					ImGui::SameLine();
				}

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Y##HB"), &g_settings.health_bar_y, 1.f, (float)g_globals.g_height);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Specifics"));

				ImGui::Checkbox(xorstr_("Use health based color"), &g_settings.health_bar_use_default_color);
				if (!g_settings.health_bar_use_default_color)
					color_wrapper(xorstr_("Bar##HB"), &g_settings.health_bar_color);

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Spacing between health bars##HB"), &g_settings.health_bar_spacing, 1.f, 100.f);
				ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Radar")))
			{
				ImGui::Checkbox(xorstr_("Radar"), &g_settings.radar);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Distance##RDR"), &g_settings.radar_distance, 1.f, 10000.f);
				ImGui::PopItemWidth();

				ImGui::SameLine();
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

				ImGui::Text(xorstr_("Positioning (Raw drawing)"));

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
				ImGui::Checkbox(xorstr_("Nearby enemies"), &g_settings.infantry_alert);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Infantry vehicles"), &g_settings.infantry_alert_light_tech);

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Alert distance"), &g_settings.infantry_alert_distance, 1.f, 500.f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Use default screen position"), &g_settings.infantry_alert_use_default_pos);
				if (!g_settings.infantry_alert_use_default_pos)
				{
					ImGui::Text(xorstr_("Position"));
					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("X##IA"), &g_settings.infantry_alert_x, 0.f, (float)g_globals.g_width);
					ImGui::SameLine();
					ImGui::SliderFloat(xorstr_("Y##IA"), &g_settings.infantry_alert_y, 0.f, (float)g_globals.g_height);
					ImGui::PopItemWidth();
					
					// If we add this section give it a space
					ImGui::Spacing();
				}

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Text size"), &g_settings.infantry_alert_text_size, 1.f, 150.f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text(xorstr_("Colors"));
				color_wrapper(xorstr_("Text##IALRT"), &g_settings.infantry_alert_color);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Misc")))
			{
				ImGui::Checkbox(xorstr_("Overheat control"), &g_settings.overheat_control);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Critical overheat value"), &g_settings.overheat_control_critical, 0.f, 1.f);
				ImGui::PopItemWidth();
				
				ImGui::Spacing();

				ImGui::Checkbox(xorstr_("Auto-spot"), &g_settings.minimap);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Unspot when using OBS"), &g_settings.obs_check);

				ImGui::Checkbox(xorstr_("Auto jet speed"), &g_settings.jet_speed);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Unlock everything"), &g_settings.unlock_all);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("No hardcore restrictions"), &g_settings.no_hc_restrictions);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Anti-AFK"), &g_settings.anti_afk);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Timer"), &g_settings.anti_afk_timer, 0, 180000);
				ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Blacklist")))
			{
				ImGui::Checkbox(xorstr_("Draw blacklisted players"), &g_settings.blacklist);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Text size"), &g_settings.blacklist_text_size, 0.f, 150.f);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Text color"));

				color_wrapper(xorstr_("Text##BL"), &g_settings.blacklist_color);

				ImGui::Separator();

				if (ImGui::TreeNode(xorstr_("Blacklisted players")))
				{
					int i = 0;
					for (const auto& rs : plugins::blacklisted)
					{
						if (ImGui::Selectable(rs.name.c_str(), false))
						{
							plugins::selected = i;
					    }

						i++;
					}

					ImGui::TreePop();
				}

				if (ImGui::Button(xorstr_("Delete player from blacklist")))
				{
					std::string nickname = plugins::blacklisted.at(plugins::selected).name;
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
				ImGui::Checkbox(xorstr_("Draw PB & FF screenshots amount"), &g_settings.screenshots);
				ImGui::SameLine();
				
				if (ImGui::Button(xorstr_("Reset counters")))
				{
					g_globals.screenshots_ff = NULL;
					g_globals.screenshots_pb = NULL;
				}

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Spoof local name"), &g_settings.spoof_name);
				ImGui::SameLine();
				
				if (ImGui::Button(xorstr_("Restore local name")))
				{
					*g_settings.spoofed_name = '\0';

					g_settings.spoof_restore = true;
				}

				ImGui::PushItemWidth(300.f);
				ImGui::InputText(xorstr_("Nickname"), g_settings.spoofed_name, 16);
				ImGui::PopItemWidth();

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This new nickname will only be visible to you."));

				ImGui::Spacing();
				
				ImGui::Text(xorstr_("This affects ESP and Spectator List only!"));
				ImGui::Checkbox(xorstr_("Streamer mode"), &g_settings.streamer_mode);

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

				ImGui::SameLine();

				ImGui::Text(fmt::format(xorstr_("Release rev: {}"), xorstr_(__DATE__)).c_str());

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}
