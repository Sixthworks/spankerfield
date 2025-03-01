#include "gui.h"
#include "renderer.h"
#include "Maps/bone_manager.h"
#include "Maps/key_map.h"
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

			if (ImGui::BeginTabItem(xorstr_("Aimbot")))
			{
				ImGui::Checkbox(xorstr_("Aimbot"), &g_settings.aimbot);
				ImGui::Checkbox(xorstr_("FOV target selection"), &g_settings.aim_fov_method);
				ImGui::Checkbox(xorstr_("Aim through walls"), &g_settings.aim_must_be_visible);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Locks on targets without them being fully visible to you."));
				ImGui::Checkbox(xorstr_("Don't aim while reloading"), &g_settings.aim_must_not_reload);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Will not aim at enemies if your weapon is reloading."));

				ImGui::Separator();

				ImGui::SliderFloat(xorstr_("Aim zeroing correction"), &g_settings.aim_zeroing_correction, 0.0f, 1.0f, "%.4f");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Adjusts the zeroing correction factor for aiming."));


				ImGui::Separator();

				
				ImGui::Checkbox(xorstr_("Draw aim point"), &g_settings.esp_draw_aim_point);

				if (g_settings.esp_draw_aim_point)
					color_wrapper(xorstr_("Point color##AP"), &g_settings.esp_aim_point_color);

				if (g_settings.aim_fov_method)
				{
					ImGui::Separator();

					ImGui::Checkbox(xorstr_("Draw FOV"), &g_settings.aim_draw_fov);

					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("FOV##Aimbot"), &g_settings.aim_fov, 0.f, 180.f);
					ImGui::PopItemWidth();

					color_wrapper(xorstr_("FOV color##FOV"), &g_settings.aim_fov_color);

					ImGui::Separator();
				}

				ImGui::Text(xorstr_("Aim key"));

				// Controller support
				if (is_controller_connected())
				{
					ImGui::Text(xorstr_("Controller detected, additional settings added"));

					ImGui::Checkbox(xorstr_("Use left trigger (LT) for aiming"), &g_settings.aim_support_controller);

					ImGui::Spacing();
				}

				ImGui::PushItemWidth(300.f);

				static bool custom_aim_key = false;
				if (custom_aim_key)
					ImGui::InputInt(xorstr_("Key##Aimbot"), &g_settings.aim_key);
				else
				{
					const char* current_item = xorstr_("Select a key");
					auto it = key_map.find(g_settings.aim_key);
					if (it != key_map.end())
						current_item = it->second.c_str();

					if (ImGui::BeginCombo(xorstr_("##Aimbot"), current_item))
					{
						for (const auto& [key, name] : key_map)
						{
							bool is_selected = (g_settings.aim_key == key);
							if (ImGui::Selectable(name.c_str(), is_selected))
								g_settings.aim_key = key;

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}

				ImGui::PopItemWidth();

				ImGui::Checkbox(xorstr_("Use old aim key selector (legacy)"), &custom_aim_key);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Allows you to use the key selection from legacy versions of spankerfield."));

				ImGui::Separator();
				
				ImGui::Text(xorstr_("Target Selection"));
				const char* target_methods[] = { "FOV", "Distance with FOV" };
				ImGui::Combo(xorstr_("Target selection method##Aimbot"), &g_settings.aim_target_selection, target_methods, IM_ARRAYSIZE(target_methods));
				
				ImGui::Checkbox(xorstr_("Auto aim bone"), &g_settings.aim_auto_bone);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Automatically selects the best bone for shooting (head -> torso -> hips)"));

				if (!g_settings.aim_auto_bone)
				{
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
				}

				ImGui::Separator();

				ImGui::Text(xorstr_("Smoothing"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Minimum time to target (seconds)##Aimbot"), &g_settings.aim_min_time_to_target, 0.01f, g_settings.aim_max_time_to_target);
				ImGui::SliderFloat(xorstr_("Maximum time to target (seconds)##Aimbot"), &g_settings.aim_max_time_to_target, g_settings.aim_min_time_to_target, 10.f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Weapons")))
			{
				ImGui::Checkbox(xorstr_("No recoil (BF4DB risk)"), &g_settings.no_recoil);
				if (g_settings.no_recoil)
				{
					ImGui::Indent();
					ImGui::PushItemWidth(200.f);

					ImGui::SliderFloat(xorstr_("Recoil decrease scale##RC"), &g_settings.recoil_decrease_scale, 0.0f, 100.0f, "%.2f");
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("Controls up-down recoil (100 = full no recoil)"));
						
					ImGui::SliderFloat(xorstr_("Recoil multiplier##RC"), &g_settings.recoil_multiplier, 0.0f, 100.0f, "%.2f");
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("Controls side-to-side recoil (should be 0)"));
						
					ImGui::PopItemWidth();
					ImGui::Unindent();
				}

				ImGui::Separator();
				
				ImGui::Checkbox(xorstr_("No spread (BF4DB risk)"), &g_settings.no_spread);
				if (g_settings.no_spread)
				{
					ImGui::Indent();
					ImGui::PushItemWidth(200.f);
					ImGui::SliderFloat(xorstr_("Spread control##SP"), &g_settings.spread_control, 0.0f, 1.0f, "%.2f");
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("0 = no spread)"));
						
					ImGui::PopItemWidth();
					ImGui::Unindent();
				}

				ImGui::Separator();


				// Not the best way
				ImGui::Text(xorstr_("Weapon editor"));

				static bool enable_editor = false;
				ImGui::Checkbox(xorstr_("Enable weapon editor (Risky)"), &enable_editor);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Highly do not recommend this while playing on servers, it was made for testing purposes only."));

				// This is for current weapon only, and made for debugging, you can make all of these as standalone plugins
				if (enable_editor)
				{
					const auto weapon_firing = get_weapon_firing();
					const auto primary_fire = weapon_firing->m_pPrimaryFire;
					const auto data = primary_fire->m_FiringData;

					if (IsValidPtrWithVTable(weapon_firing) && IsValidPtrWithVTable(primary_fire) && IsValidPtrWithVTable(data))
					{
						ImGui::Text(xorstr_("Bullet count"));
						ImGui::PushItemWidth(300.f);
						ImGui::InputInt(xorstr_("Bullets per shot##WP"), &data->m_ShotConfigData.m_BulletsPerShot, 1, 100);
						ImGui::InputInt(xorstr_("Bullets per burst##WP"), &data->m_ShotConfigData.m_BulletsPerBurst, 1, 100);
						ImGui::InputInt(xorstr_("Bullets per shell##WP"), &data->m_ShotConfigData.m_BulletsPerShell, 1, 100);
						ImGui::PopItemWidth();

						ImGui::Text(xorstr_("Overheat"));
						ImGui::InputFloat(xorstr_("Overheat drop multiplier##WP"), &data->m_OverHeatData.m_HeatDropPerSecond, 0.0f, 1000.f);
						ImGui::InputFloat(xorstr_("Overheat per bullet##WP"), &data->m_OverHeatData.m_HeatPerBullet, 0.0f, 100.f);

						static bool is_overheated = false;
						if (ImGui::Checkbox(xorstr_("Overheated"), &is_overheated))
							weapon_firing->m_IsOverheated = is_overheated;

						ImGui::Text(xorstr_("Bullet speed"));
						ImGui::PushItemWidth(300.f);
						ImGui::InputFloat(xorstr_("X##WP"), &data->m_ShotConfigData.m_Speed.x, 0.0f, 10000.f);
						ImGui::InputFloat(xorstr_("Y##WP"), &data->m_ShotConfigData.m_Speed.y, 0.0f, 10000.f);
						ImGui::InputFloat(xorstr_("Z##WP"), &data->m_ShotConfigData.m_Speed.w, 0.0f, 10000.f);
						ImGui::InputFloat(xorstr_("W##WP"), &data->m_ShotConfigData.m_Speed.w, 0.0f, 10000.f);
						ImGui::PopItemWidth();

						ImGui::Text(xorstr_("Recoil"));

						ImGui::InputFloat(xorstr_("Recoil time multiplier##WP"), &weapon_firing->m_RecoilTimeMultiplier, 0.0f, 10000.f);
						ImGui::InputFloat(xorstr_("Recoil angle X##WP"), &weapon_firing->m_RecoilAngleX, 0.0f, 1000.f);
						ImGui::InputFloat(xorstr_("Recoil angle Y##WP"), &weapon_firing->m_RecoilAngleY, 0.0f, 1000.f);
						ImGui::InputFloat(xorstr_("Recoil angle Z##WP"), &weapon_firing->m_RecoilAngleZ, 0.0f, 1000.f);

						ImGui::Text(xorstr_("Other"));

						if (IsValidPtrWithVTable(data->m_ShotConfigData.m_ProjectileData))
						{
							static bool instant_hit = false;
							if (ImGui::Checkbox(xorstr_("Instant Hit"), &instant_hit))
								data->m_ShotConfigData.m_ProjectileData->m_InstantHit = instant_hit;

							ImGui::PushItemWidth(300.f);
							ImGui::InputFloat(xorstr_("Gravity##WP"), &data->m_ShotConfigData.m_ProjectileData->m_Gravity, 0.0f, 10.f);
							ImGui::InputFloat(xorstr_("Time to live##WP"), &data->m_ShotConfigData.m_ProjectileData->m_TimeToLive, 0.0f, 10.f);
							ImGui::InputFloat(xorstr_("End damage##WP"), &data->m_ShotConfigData.m_ProjectileData->m_EndDamage, 0.0f, 10.f);
							ImGui::PopItemWidth();
						}
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
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw teammates##ESP"), &g_settings.esp_draw_teammates);

				ImGui::Text(xorstr_("Teammates color"));

				color_wrapper(xorstr_("Teammate##ESP"), &g_settings.esp_teammate_color);

				ImGui::Text(xorstr_("Additional tags color"));

				color_wrapper(xorstr_("Tags##ESP"), &g_settings.esp_additional_tags_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw box"), &g_settings.esp_draw_box);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Fill box"), &g_settings.esp_box_fill);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Fills/paints the inside of a box with a solid color."));
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Box style"), &g_settings.esp_box_style, 1, 6);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Box color"));

				if (g_settings.esp_box_fill)
					color_wrapper(xorstr_("Filler##BX"), &g_settings.esp_box_fill_color);

				color_wrapper(xorstr_("Not visible##BX"), &g_settings.esp_box_color_occluded);
				color_wrapper(xorstr_("Visible##BX"), &g_settings.esp_box_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw line"), &g_settings.esp_draw_line);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws a line from the selected location to an enemy."));
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Line point location"), &g_settings.esp_draw_line_from, 0, 8);
				ImGui::SliderFloat(xorstr_("Line thickness"), &g_settings.esp_line_thickness, 0.1f, 10.f);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Line color"));

				color_wrapper(xorstr_("Not visible##LC"), &g_settings.esp_line_color);
				color_wrapper(xorstr_("Visible##LC"), &g_settings.esp_line_color_occluded);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw name"), &g_settings.esp_draw_name);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw distance"), &g_settings.esp_draw_distance);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw vehicle tag"), &g_settings.esp_draw_vehicle_tag);

				ImGui::Spacing();

				ImGui::Text(xorstr_("Text spacing"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Text spacing##ESP"), &g_settings.esp_text_spacing, 0.f, 50.f);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Text color"));

				color_wrapper(xorstr_("Not visible##TX"), &g_settings.text_color_occluded);
				color_wrapper(xorstr_("Visible##TX"), &g_settings.text_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw health bar"), &g_settings.esp_draw_health);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Health location"), &g_settings.esp_health_location, 0, 3);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw explosives"), &g_settings.explosives);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws a number sign for every explosive entity on the server."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw laser guided missiles"), &g_settings.missiles_own);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws boxes for TOW/SRAW missiles."));

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
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws little dots on locations where bones end."));
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Dots distance"), &g_settings.skeleton_dots_distance, 1.f, 5000.f);
				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Skeleton color"));

				color_wrapper(xorstr_("Not visible##SK"), &g_settings.skeleton_color_occluded);
				color_wrapper(xorstr_("Visible##SK"), &g_settings.skeleton_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw personal health bar"), &g_settings.draw_health_bar);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws a health bar indicating the local player/vehicle health state."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Player##HB"), &g_settings.health_bar_soldier);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Vehicle##HB"), &g_settings.health_bar_vehicle);

				ImGui::Text(xorstr_("Size"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Width##HB"), &g_settings.health_bar_width, 1.f, (float)g_globals.g_width);
				ImGui::SliderFloat(xorstr_("Height##HB"), &g_settings.health_bar_height, 1.f, (float)g_globals.g_height);
				ImGui::PopItemWidth();

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

				// Add spacing UI only if both are activated
				if (g_settings.health_bar_soldier && g_settings.health_bar_vehicle)
				{
					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("Spacing between health bars##HB"), &g_settings.health_bar_spacing, 1.f, 100.f);
					ImGui::PopItemWidth();
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("How many space in pixels there is between two health bars."));
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Radar")))
			{
				ImGui::Checkbox(xorstr_("Radar"), &g_settings.radar);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Circular shape"), &g_settings.radar_circular);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Makes the radar have a circle shape instead of the default square one."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw teammates##RDR"), &g_settings.radar_draw_teammates);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw self##RDR"), &g_settings.radar_draw_you);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws a little dot in the center to help you better orient."));

				ImGui::PushItemWidth(300.f);

				// We do this so it gets the correct number in the first run
				static float radar_size = -1.f;
				if (radar_size < 0)
					radar_size = g_settings.radar_width; // You can even use height, since they are the same

				if (ImGui::SliderFloat(xorstr_("Radar size##RDR"), &radar_size, 100.f, (float)g_globals.g_height))
				{
					// Should've made this one variable, honestly
					g_settings.radar_width = radar_size;
					g_settings.radar_height = radar_size;
				}

				ImGui::PopItemWidth();


				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Distance##RDR"), &g_settings.radar_distance, 1.f, 10000.f);
				ImGui::PopItemWidth();

				ImGui::Checkbox(xorstr_("Draw cross##RDR"), &g_settings.radar_cross);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw outline##RDR"), &g_settings.radar_outline);

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

				if (g_settings.radar_cross)
					color_wrapper(xorstr_("Cross##RDR"), &g_settings.radar_cross_color);

				if (g_settings.radar_outline)
					color_wrapper(xorstr_("Outline##RDR"), &g_settings.radar_outline_color);

				if (g_settings.radar_draw_you)
					color_wrapper(xorstr_("Self##RDR"), &g_settings.radar_you_color);

				color_wrapper(xorstr_("Background##RDR"), &g_settings.radar_background_color);
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
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Uses the traditional drawing for the spectator list, and not a window."));

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
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw indicators"), &g_settings.infantry_alert_indicators);

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
				color_wrapper(xorstr_("Indicators##IALRT"), &g_settings.infantry_alert_indicator_color);

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
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Big random chance of unspotting enemies when OBS is running."));

				ImGui::Checkbox(xorstr_("Auto jet speed"), &g_settings.jet_speed);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Automatically turns the jet to achieve the best possible speed."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Unlock everything (BF4DB risk)"), &g_settings.unlock_all);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Unlocks everything but works for attachments only, do not recommend using it though."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("No hardcore restrictions (PBSS risk)"), &g_settings.no_hc_restrictions);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("The risk is small, although you can get screenshotted with your crosshair visible while playing HC."));

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Anti-AFK"), &g_settings.anti_afk);
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Timer"), &g_settings.anti_afk_timer, 0, 180000);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Kill sound (FFSS risk)"), &g_settings.kill_sound);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This triggers a screenshot by FairFight the first time you use it."));

				ImGui::PushItemWidth(550.f);
				ImGui::InputText(xorstr_("Path to file (.wav)"), g_settings.kill_sound_path, MAX_PATH);
				ImGui::PopItemWidth();
				ImGui::Text(xorstr_("Make sure the file exists, has roman/latin characters only, and is a WAVE audio file"));

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Blacklist")))
			{
				ImGui::Checkbox(xorstr_("Draw blacklisted players"), &g_settings.blacklist);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws blacklisted players in the center part of the screen."));
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
							plugins::selected = i;

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
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Shows the total amount of times you've been screenshotted by FF or PB."));


				ImGui::SameLine();

				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("PB clean screenshot duration (ms)"), &g_settings.screenhots_pb_delay, 50, 1000);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Delay before drawing visuals after PunkBuster screenshot gets taken, recommended to leave at 500 to be extra sure."));
				ImGui::PopItemWidth();

				ImGui::SameLine();

				if (ImGui::Button(xorstr_("Reset counters")))
				{
					g_globals.screenshots_ff = NULL;
					g_globals.screenshots_pb = NULL;
				}

				if (g_settings.screenshots)
					color_wrapper(xorstr_("Text##SC"), &g_settings.screenshots_color);

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

				ImGui::Checkbox(xorstr_("Streamer mode"), &g_settings.streamer_mode);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This will use fake nicknames in ESP and censor out nicknames in Spectator List."));

				ImGui::SameLine();

				ImGui::Checkbox(xorstr_("Rainbow mode"), &g_settings.rainbow_mode);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This will make every visual color have the Rainbow effect."));

				if (g_settings.rainbow_mode)
				{
					ImGui::SameLine(); // Want to keep it on the same line

					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("Speed##RM"), &g_settings.rainbow_speed, 0.1f, 2.0f);
					ImGui::PopItemWidth();
				}

				ImGui::Separator();

				ImGui::Text(xorstr_("Configuration"));

				if (ImGui::Button(xorstr_("Load")))
					g_config.load();

				ImGui::SameLine();

				if (ImGui::Button(xorstr_("Save")))
					g_config.attempt_save();

				ImGui::Separator();

				if (ImGui::Button(xorstr_("Unload")))
					g_globals.g_running = false;

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This function is not safe at all, there is a big chance your game might crash when injecting the cheat again."));

				ImGui::SameLine();

				ImGui::Text(fmt::format(xorstr_("Release rev: {}"), xorstr_(__DATE__)).c_str());

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}