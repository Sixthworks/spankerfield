#include "gui.h"
#include "renderer.h"
#include "Maps/bone_manager.h"
#include "../common.h"
#include "../global.h"
#include "../settings.h"
#include "../Features/main.h"
#include "../Utilities/other.h"
#include "../Utilities/tooltip.h"
#include "../Utilities/firing_data.h"

namespace big
{
	void gui::dx_init()
	{
		auto& style = ImGui::GetStyle();

		// Modernisierte Style-Einstellungen
		style.WindowPadding = { 10.f, 10.f };
		style.FramePadding = { 8.f, 6.f };
		style.ItemSpacing = { 10.f, 8.f };
		style.ItemInnerSpacing = { 8.f, 8.f };
		style.ScrollbarSize = 12.f;
		style.GrabMinSize = 10.f;

		// Abgerundete Ecken für ein modernes Aussehen
		style.WindowRounding = 8.f;
		style.ChildRounding = 6.f;
		style.FrameRounding = 6.f;
		style.PopupRounding = 6.f;
		style.ScrollbarRounding = 6.f;
		style.GrabRounding = 6.f;
		style.TabRounding = 6.f;

		// Minimalistische Rahmen
		style.WindowBorderSize = 1.f;
		style.ChildBorderSize = 1.f;
		style.PopupBorderSize = 1.f;
		style.FrameBorderSize = 1.f;
		style.TabBorderSize = 1.f;

		style.WindowTitleAlign = { 0.5f, 0.5f };
		style.ButtonTextAlign = { 0.5f, 0.5f };

		// Modernes Farbschema
		ImVec4* colors = style.Colors;

		colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.95f); // Dunkler Hintergrund mit leichter Transparenz
		colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.18f, 0.95f);
		colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);

		// Textfarben
		colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

		// Buttons
		colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.50f, 0.75f, 1.00f); // Blaue Akzentfarbe für aktive Tabs

		// Slider und Scrollbars
		colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.50f, 0.75f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.60f, 0.85f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

		// Transparente Overlays
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.60f);

		// Setze die Fenstergröße auf einen Standardwert
		ImGui::SetNextWindowSize(ImVec2(850, 750));
	}

	void gui::dx_on_tick()
	{
		// Begrenzung der Fenstergröße auf 3/4 der Monitorgröße
		ImVec2 display_size = ImGui::GetIO().DisplaySize;
		ImVec2 max_size = ImVec2(display_size.x * 0.75f, display_size.y * 0.75f); // Manuelle Multiplikation

		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), max_size);

		if (ImGui::Begin(xorstr_("Spankerfield"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BeginTabBar(xorstr_("Tab bar"));

			// Aimbot Tab
			if (ImGui::BeginTabItem(xorstr_("Aimbot")))
			{
				if (ImGui::CollapsingHeader(xorstr_("General Settings"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox(xorstr_("Enable Aimbot"), &g_settings.aimbot);
					ImGui::Checkbox(xorstr_("Vehicle Aimbot"), &g_settings.aimbot_vehicle);
					ImGui::Checkbox(xorstr_("Snap to Target"), &g_settings.aimbot_snap_to_target);
					ImGui::Checkbox(xorstr_("Support Non-Standard Weapons"), &g_settings.aimbot_non_standart);
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader(xorstr_("Targeting Options"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox(xorstr_("FOV Target Selection"), &g_settings.aim_fov_method);
					ImGui::Checkbox(xorstr_("Target Must Be Visible"), &g_settings.aim_must_be_visible);
					ImGui::Checkbox(xorstr_("Ignore Friends"), &g_settings.aim_ignore_friends);
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader(xorstr_("Visual Aids"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox(xorstr_("Draw Aim Point"), &g_settings.esp_draw_aim_point);
					if (g_settings.esp_draw_aim_point)
					{
						ImGui::SliderFloat(xorstr_("Point Scale"), &g_settings.esp_aim_point_size, 0.1f, 10.f);
						color_wrapper(xorstr_("Point Color"), &g_settings.esp_aim_point_color);
					}
				}

				if (g_settings.aim_fov_method)
				{
					ImGui::Separator();

					ImGui::Checkbox(xorstr_("Draw FOV"), &g_settings.aim_draw_fov);

					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("FOV##Aimbot"), &g_settings.aim_fov, 0.f, 180.f);
					ImGui::PopItemWidth();

					if (g_settings.aim_fov >= 80.f)
						ImGui::WarningTooltip(xorstr_("Having FOV this high might get you banned."));

					color_wrapper(xorstr_("FOV color##FOV"), &g_settings.aim_fov_color);

					ImGui::Separator();
				}

				render_hotkey_selector(xorstr_("Aim key"), &g_settings.aim_key, 200.f, false);

				// Controller support
				if (is_controller_connected())
				{
					ImGui::Text(xorstr_("Controller detected, additional settings added"));

					ImGui::Checkbox(xorstr_("Use left trigger (LT) for aiming"), &g_settings.aim_support_controller);

					ImGui::Spacing();
				}

				ImGui::Separator();
				
				ImGui::Text(xorstr_("Target Selection"));

				std::vector<const char*> obfuscated_items;
				obfuscated_items.push_back(xorstr_("FOV"));
				obfuscated_items.push_back(xorstr_("Distance with FOV"));

				const char** target_methods = obfuscated_items.data();
				ImGui::PushItemWidth(300.f);
				ImGui::Combo(xorstr_("Target selection method##Aimbot"), &g_settings.aim_target_selection, target_methods, (int)obfuscated_items.size());
				ImGui::PopItemWidth();

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

				ImGui::Text(xorstr_("Soldier"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Minimum time to target (sec)##Aimbot"), &g_settings.aim_min_time_to_target, 0.01f, g_settings.aim_max_time_to_target);

				if (g_settings.aim_min_time_to_target < 0.15f)
					ImGui::WarningTooltip(xorstr_("Having this option that low will most likely get you banned."));

				ImGui::SliderFloat(xorstr_("Maximum time to target (sec)##Aimbot"), &g_settings.aim_max_time_to_target, g_settings.aim_min_time_to_target, 10.f);

				if (g_settings.aim_max_time_to_target < 0.2f)
					ImGui::WarningTooltip(xorstr_("Having this option that low will most likely get you banned."));

				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Vehicle"));

				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Smoothing factor##Aimbot"), &g_settings.aim_vehicle_smooth, 0.f, 20.f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::EndTabItem();
			}

			// Weapons Tab
			if (ImGui::BeginTabItem(xorstr_("Weapons")))
			{
				if (ImGui::CollapsingHeader(xorstr_("C4 Bot Settings"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text(xorstr_("It's recommended to use the default settings, since they are tweaked specifically for the script."));

					ImGui::Checkbox(xorstr_("Enable C4 Bot"), &g_settings.c4_bot);
					ImGui::Checkbox(xorstr_("Always Active"), &g_settings.c4_bot_always_active);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("This will make the C4 Bot always active, even when not using a C4"));

					// Auto-detonate settings
					ImGui::Checkbox(xorstr_("Auto detonate"), &g_settings.c4_bot_auto_detonate);
					if (g_settings.c4_bot_auto_detonate)
						ImGui::Checkbox(xorstr_("No key required"), &g_settings.c4_bot_auto_detonate_independently);
					ImGui::WarningTooltip(xorstr_("This will make the auto detonate work without the key, which means it can get obvious that you're cheating."));

					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("Enemy detection radius"), &g_settings.c4_bot_radius, 0.f, 7.f);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("This is the base enemy detection radius, not recommended to change"));

					ImGui::SliderFloat(xorstr_("Minimum amount of damage dealt"), &g_settings.c4_bot_min_damage_to_enemy, 0.f, 95.f);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("How much damage is going to be dealt, configure for normal mode, will be adjusted to hardcore automatically."));
					
					ImGui::SliderFloat(xorstr_("Damage radius"), &g_settings.c4_bot_damage_radius, 0.f, 5.f);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("Radius where the player is starting to take even a little bit of damage, recommended to leave at default."));

					ImGui::SliderFloat(xorstr_("Lethal radius"), &g_settings.c4_bot_lethal_radius, 0.f, 3.f);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("Radius where the player is guaranteed to die, recommended to leave at default."));

					ImGui::PopItemWidth();

					// Hotkey dropdown
					render_hotkey_selector(xorstr_("Detonation key"), &g_settings.c4_bot_key, 200.f, false);

					ImGui::Spacing();

					// Self-damage prevention settings
					ImGui::Checkbox(xorstr_("Prevent self damage"), &g_settings.c4_bot_prevent_self_damage);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("Will prevent self damage despite all conditions"));
					if (g_settings.c4_bot_prevent_self_damage)
					{
						ImGui::Checkbox(xorstr_("Smart self damage"), &g_settings.c4_bot_smart_self_damage);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(xorstr_("This will try to kill the player near you while trying to avoid your death"));
						if (g_settings.c4_bot_smart_self_damage)
						{
							ImGui::PushItemWidth(200.f);
							ImGui::SliderFloat(xorstr_("Health Buffer"), &g_settings.c4_bot_health_buffer, 0.f, 50.f);
							ImGui::PopItemWidth();
						}
					}

					// Add ignore friends checkbox
					ImGui::Checkbox(xorstr_("Ignore friends"), &g_settings.c4_bot_ignore_friends);
				}

				ImGui::Separator();

				if (ImGui::CollapsingHeader(xorstr_("Weapon sway"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox(xorstr_("No recoil"), &g_settings.no_recoil);
					ImGui::WarningTooltip(xorstr_("If you are being spectated, the spectator will immediately notice the recoil and report you to BF4DB."));

					if (g_settings.no_recoil)
					{
						ImGui::PushItemWidth(200.f);

						ImGui::SliderFloat(xorstr_("Recoil decrease scale##RC"), &g_settings.recoil_decrease_scale, 0.0f, 100.0f, xorstr_("%.2f"));
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(xorstr_("Controls up-down recoil (100 = full no recoil)"));

						ImGui::SliderFloat(xorstr_("Recoil multiplier##RC"), &g_settings.recoil_multiplier, 0.0f, 100.0f, xorstr_("%.2f"));
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(xorstr_("Controls side-to-side recoil (should be 0)"));

						ImGui::PopItemWidth();
					}

					ImGui::Checkbox(xorstr_("No spread"), &g_settings.no_spread);
					ImGui::WarningTooltip(xorstr_("If you are being spectated, the spectator will immediately notice the weapon spread and report you to BF4DB."));

					if (g_settings.no_spread)
					{
						ImGui::PushItemWidth(200.f);
						ImGui::SliderFloat(xorstr_("Spread control##SP"), &g_settings.spread_control, 0.0f, 1.0f, xorstr_("%.2f"));
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(xorstr_("0 = no spread)"));

						ImGui::PopItemWidth();
					}
				}

				ImGui::Separator();

				// Not the best way
				if (ImGui::CollapsingHeader(xorstr_("Weapon editor")))
				{
					static bool enable_editor = false;
					ImGui::Checkbox(xorstr_("Enable weapon editor"), &enable_editor);
					ImGui::WarningTooltip(xorstr_("Highly do not recommend this while playing on servers, it was made for testing purposes only."));

					// This is for current weapon only, and made for debugging, you can make all of these as standalone plugins
					if (enable_editor)
					{
						const auto weapon_firing = get_weapon_firing();
						if (!IsValidPtr(weapon_firing))
						{
							ImGui::Text(xorstr_("Weapon firing not found - you are not in world or address is invalid."));
						}
						else
						{
							const auto data = WeaponFiringDataRetriever::GetSafeFiringData(weapon_firing, is_localplayer_in_vehicle());
							if (!IsValidPtr(data))
							{
								ImGui::Text(xorstr_("Firing data not found - you are not in world or address is invalid."));
							}
							else if (IsValidPtr(weapon_firing) && IsValidPtr(data))
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

								if (IsValidPtr(data->m_ShotConfigData.m_ProjectileData))
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

								ImGui::Separator();

								ImGui::Text(xorstr_("These values are unknown to me"));
								ImGui::WarningTooltip(xorstr_("You can test them at your own risk."));

								ImGui::Checkbox(xorstr_("Relative target aiming"), reinterpret_cast<bool*>(&data->m_ShotConfigData.m_RelativeTargetAiming));
								ImGui::Checkbox(xorstr_("Force spawn to camera"), reinterpret_cast<bool*>(&data->m_ShotConfigData.m_ForceSpawnToCamera));
								ImGui::Checkbox(xorstr_("Spawn visual at weapon bone"), reinterpret_cast<bool*>(&data->m_ShotConfigData.m_SpawnVisualAtWeaponBone));
								ImGui::Checkbox(xorstr_("Active force spawn to camera"), reinterpret_cast<bool*>(&data->m_ShotConfigData.m_ActiveForceSpawnToCamera));
							}
						}
					}
				}

				ImGui::EndTabItem();
			}

			// Visuals Tab
			if (ImGui::BeginTabItem(xorstr_("Visuals")))
			{
				if (ImGui::CollapsingHeader(xorstr_("ESP Settings"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox(xorstr_("Enable ESP"), &g_settings.esp);
					ImGui::SliderFloat(xorstr_("ESP Distance"), &g_settings.esp_distance, 1.f, 10000.f);
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader(xorstr_("Box Settings"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox(xorstr_("Draw 2D Box"), &g_settings.esp_draw_box);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("Draws a simple 2D box around the player/vehicle model."));
					ImGui::SameLine();
					ImGui::Checkbox(xorstr_("Fill box"), &g_settings.esp_box_fill);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("Fills/paints the inside of a box with a solid color."));
					ImGui::PushItemWidth(300.f);
					ImGui::SliderInt(xorstr_("Box style"), &g_settings.esp_box_style, 1, 6);
					ImGui::PopItemWidth();
					color_wrapper(xorstr_("Box color"), &g_settings.esp_box_color);
					color_wrapper(xorstr_("Box color (occluded)"), &g_settings.esp_box_color_occluded);

					if (g_settings.esp_box_fill)
					    color_wrapper(xorstr_("Box fill color"), &g_settings.esp_box_fill_color);
				}

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw 3D box"), &g_settings.esp_draw_3d_box);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws a 3D box fully showing the player/vehicle model."));
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("3D box thickness"), &g_settings.esp_3d_box_thickness, 0.5f, 3.0f);
				ImGui::PopItemWidth();
				color_wrapper(xorstr_("3D box color"), &g_settings.esp_3d_box_color);
				color_wrapper(xorstr_("3D box color (occluded)"), &g_settings.esp_3d_box_color_occluded);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw eye tracer"), &g_settings.esp_draw_eye_tracer);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws a line showing where the enemy is facing forward right now."));
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Eye tracer distance"), &g_settings.esp_eye_tracer_distance, 1.0f, 30.0f);
				ImGui::SliderFloat(xorstr_("Eye tracer thickness"), &g_settings.esp_eye_tracer_thickness, 0.5f, 3.0f);
				ImGui::PopItemWidth();
				color_wrapper(xorstr_("Eye tracer color"), &g_settings.esp_eye_tracer_color);
				color_wrapper(xorstr_("Eye tracer color (occluded)"), &g_settings.esp_eye_tracer_color_occluded);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw line"), &g_settings.esp_draw_line);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws a line from the selected location on the screen to an enemy position."));
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
				ImGui::SliderInt(xorstr_("Text position"), &g_settings.esp_text_position, 0, 2);
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
					ImGui::SetTooltip(xorstr_("Draws boxes for TOW/SRAW/Laser guided missiles."));

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
				ImGui::SliderInt(xorstr_("Crosshair type"), &g_settings.crosshair_type, 0, 5);
				ImGui::SliderFloat(xorstr_("Crosshair size"), &g_settings.crosshair_size, 0.1f, 100.f);

				// There is no thickness implemented for dot and circle
				if (g_settings.crosshair_type != 1 && g_settings.crosshair_type != 2)
				    ImGui::SliderFloat(xorstr_("Crosshair thickness"), &g_settings.crosshair_thickness, 1.f, 100.f);

				ImGui::PopItemWidth();

				ImGui::Text(xorstr_("Colors"));
				color_wrapper(xorstr_("Crosshair##CS"), &g_settings.crosshair_color);

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Draw skeleton"), &g_settings.skeleton);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Use skeleton dots"), &g_settings.skeleton_use_dots);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws little dots on locations where 2 connected skeleton bones end."));
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
				ImGui::Checkbox(xorstr_("Soldier##HB"), &g_settings.health_bar_soldier);
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
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This will make the health bar use the traditional red (being lowest) -> green (being highest) system for coloring."));
				
				if (!g_settings.health_bar_use_default_color)
				{
					ImGui::SameLine();
					color_wrapper(xorstr_("Bar##HB"), &g_settings.health_bar_color);
				}

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
				ImGui::Checkbox(xorstr_("Draw friends##RDR"), &g_settings.radar_draw_friends);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws your friends on the radar if enabled."));
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
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Draws lines on the radar for better orientation in space."));
				ImGui::SameLine();
				
				// Show detailed cross only if the regular one is enabled
				if (g_settings.radar_cross)
				{
					ImGui::Checkbox(xorstr_("Detailed cross##RDR"), &g_settings.radar_cross_detailed);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(xorstr_("Makes the radar cross have a more detailed and professional look."));
					ImGui::SameLine();
				}

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

				if (ImGui::CollapsingHeader(xorstr_("Colors"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (g_settings.radar_cross)
						color_wrapper(xorstr_("Cross##RDR"), &g_settings.radar_cross_color);

					if (g_settings.radar_outline)
						color_wrapper(xorstr_("Outline##RDR"), &g_settings.radar_outline_color);

					if (g_settings.radar_draw_you)
						color_wrapper(xorstr_("Self##RDR"), &g_settings.radar_you_color);

					color_wrapper(xorstr_("Background##RDR"), &g_settings.radar_background_color);
					color_wrapper(xorstr_("Friends##RDR"), &g_settings.radar_friends_color);
					color_wrapper(xorstr_("Teammates##RDR"), &g_settings.radar_teammates_color);
					color_wrapper(xorstr_("Ememies##RDR"), &g_settings.radar_enemies_color);
					color_wrapper(xorstr_("Friend vehicles##RDR"), &g_settings.radar_friend_vehicles_color);
					color_wrapper(xorstr_("Teammate vehicles##RDR"), &g_settings.radar_teammate_vehicles_color);
					color_wrapper(xorstr_("Ememy vehicles##RDR"), &g_settings.radar_enemy_vehicles_color);
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Spectator list")))
			{
				ImGui::Checkbox(xorstr_("Spectator list"), &g_settings.spectator_list);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Raw drawing"), &g_settings.spectator_raw_drawing);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Uses the traditional drawing for the spectator list, and not a window."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Warn about new spectators"), &g_settings.spectator_warn_new);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("When a new spectator joins the server, you will get a warning on your screen."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Stealth mode when spectated"), &g_settings.spectator_disable_esp_aim);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("When a spectator is on the server, it will disable the aimbot and visuals so that you don't risk getting caught."));

				if (g_settings.spectator_warn_new || g_settings.spectator_disable_esp_aim)
				{
					ImGui::PushItemWidth(300.f);
					ImGui::SliderFloat(xorstr_("Warning text size##SP"), &g_settings.spectator_warnings_size, 1.f, 100.f);
					ImGui::PopItemWidth();
				}

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

				if (g_settings.spectator_warn_new || g_settings.spectator_disable_esp_aim)
					color_wrapper(xorstr_("Warning text##SPC"), &g_settings.spectator_warnings_color);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Infantry alert")))
			{
				ImGui::Checkbox(xorstr_("Nearby enemies"), &g_settings.infantry_alert);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Infantry vehicles"), &g_settings.infantry_alert_light_tech);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This will also check for transport helicopters, jeeps, transport boats... etc."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Draw indicators"), &g_settings.infantry_alert_indicators);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This will show indicators like height and distance below the main enemy nearby text."));

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
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Allows you to keep firing even if your vehicle turret/gun overheats."));
				ImGui::PushItemWidth(300.f);
				ImGui::SliderFloat(xorstr_("Critical overheat value"), &g_settings.overheat_control_critical, 0.f, 1.f);
				ImGui::PopItemWidth();

				ImGui::Spacing();

				ImGui::Checkbox(xorstr_("Auto-spot"), &g_settings.minimap);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Unspot when using recording software"), &g_settings.obs_check);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Big random chance of unspotting enemies when OBS, Streamlabs, Bandicam, Action... etc. is running."));

				ImGui::Spacing();

				ImGui::Checkbox(xorstr_("Spot-spam"), &g_settings.spot_spam);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This is not the same as auto-spot, this presses the spot key when needed, and the enemy is spotted for everyone."));
				ImGui::SameLine();

				ImGui::PushItemWidth(200.f);
				ImGui::SliderFloat(xorstr_("Threshold"), &g_settings.spot_spam_threshold, 0.f, 100.f);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This is the threshold of pixels in the screen center to analyze targest in."));
				ImGui::SameLine();
				ImGui::SliderInt(xorstr_("Cooldown"), &g_settings.spot_spam_cooldown, 0, 10000);
				ImGui::PopItemWidth();

				ImGui::Spacing();

				ImGui::Checkbox(xorstr_("Auto jet speed"), &g_settings.jet_speed);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Automatically turns the jet to achieve the best possible speed."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Unlock everything"), &g_settings.unlock_all);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Unlocks everything, but works for attachments only."));
				ImGui::WarningTooltip(xorstr_("A BF4DB risk is present, because your account doesn't actually have those attachments, risk of a manual check."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("No hardcore restrictions"), &g_settings.no_hc_restrictions);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("The risk is small, although you can get screenshotted with your crosshair visible while playing HC."));
				ImGui::WarningTooltip(xorstr_("You can get screenshotted by PB with your crosshair visible while playing HC."));

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Anti-AFK"), &g_settings.anti_afk);
				ImGui::WarningTooltip(xorstr_("This option may not work when you are away from game window."));
				ImGui::PushItemWidth(300.f);
				ImGui::SliderInt(xorstr_("Timer (ms)"), &g_settings.anti_afk_timer, 0, 210000);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Cooldown before the script executes another Anti-AFK move."));
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Checkbox(xorstr_("Kill sound"), &g_settings.kill_sound);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("This plays a custom sound of your choice when you kill an enemy."));
				ImGui::WarningTooltip(xorstr_("This triggers a screenshot by FairFight the first time you use it, but it's safe."));

				ImGui::PushItemWidth(550.f);
				ImGui::InputText(xorstr_("Path to file (.wav)"), g_settings.kill_sound_path, MAX_PATH);
				ImGui::PopItemWidth();
				ImGui::WarningTooltip(xorstr_("Make sure the file exists, has roman/latin characters only in the name, and is a WAVE audio file"));

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Friends")))
			{
				ImGui::PushItemFlag(ImGuiTreeNodeFlags_DefaultOpen, true);
				if (ImGui::TreeNode(xorstr_("Friends list")))
				{
					int i = 0;
					for (const auto& rs : plugins::friends_list)
					{
						if (ImGui::Selectable(rs.name.c_str(), false))
							plugins::selected_friend = i;
						i++;
					}
					ImGui::TreePop();
				}
				ImGui::PopItemFlag();

				if (ImGui::Button(xorstr_("Delete friend")))
				{
					if (!plugins::friends_list.empty() && plugins::selected_friend < plugins::friends_list.size())
					{
						std::string nickname = plugins::friends_list.at(plugins::selected_friend).name;
						if (!nickname.empty())
							plugins::delete_from_friends(nickname);
					}
				}

				ImGui::Separator();
				static char friend_nick[50]{};
				ImGui::PushItemWidth(250.f);
				ImGui::InputText(xorstr_("Friend Nickname"), friend_nick, IM_ARRAYSIZE(friend_nick));
				ImGui::PopItemWidth();
				if (ImGui::Button(xorstr_("Add friend")))
					plugins::add_to_friends(friend_nick);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Blacklist")))
			{
				ImGui::Checkbox(xorstr_("Blacklist##BL"), &g_settings.blacklist);
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Warn about blacklisted players"), &g_settings.blacklist_warn_on_screen);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Screen warning of blacklisted players on the server."));
				ImGui::SameLine();
				ImGui::Checkbox(xorstr_("Show blacklisted tag in ESP"), &g_settings.blacklist_tag_in_esp);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(xorstr_("Adds a tag to ESP if the player is blacklisted."));

				if (g_settings.blacklist_warn_on_screen)
				{
					ImGui::PushItemWidth(300.f);
				    ImGui::SliderFloat(xorstr_("Text size"), &g_settings.blacklist_text_size, 0.f, 150.f);
				    ImGui::PopItemWidth();
			    }

				if (g_settings.blacklist_warn_on_screen || g_settings.blacklist_tag_in_esp)
				{
					ImGui::Text(xorstr_("Colors"));

					if (g_settings.blacklist_warn_on_screen)
						color_wrapper(xorstr_("Text##BL"), &g_settings.blacklist_color);

					if (g_settings.blacklist_tag_in_esp)
						color_wrapper(xorstr_("ESP##BL"), &g_settings.blacklist_tag_color);
				}
				else
					ImGui::Text(xorstr_("Colors unavailable, no options selected."));

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

				ImGui::Spacing();

				static char nick[50]{};
				ImGui::PushItemWidth(250.f);
				ImGui::InputText(xorstr_("Nickname"), nick, IM_ARRAYSIZE(nick));
				ImGui::PopItemWidth();

				if (ImGui::Button(xorstr_("Add player to blacklist")))
					plugins::add_to_blacklist(nick);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(xorstr_("Settings")))
			{
				if (ImGui::CollapsingHeader(xorstr_("General Settings"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox(xorstr_("Streamer Mode"), &g_settings.streamer_mode);
					ImGui::Checkbox(xorstr_("Disable Watermark"), &g_settings.disable_watermark);
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader(xorstr_("Configuration"), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text(xorstr_("Current Config:"));
					ImGui::InputText(xorstr_("##ConfigName"), g_config.current_config.data(), g_config.current_config.size());
					if (ImGui::Button(xorstr_("Save Config")))
						g_config.save();
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}