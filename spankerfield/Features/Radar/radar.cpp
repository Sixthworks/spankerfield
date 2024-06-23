#include "radar.h"
#include "../../settings.h"
#include "../../Utilities/other.h"
#include "../../Rendering/draw-list.h"

using namespace big;
namespace plugins
{
	void draw_radar()
	{
		if (!g_settings.radar) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		const auto component = local_soldier->m_pWeaponComponent;
		if (!component) return;

		const auto weapon = component->GetActiveSoldierWeapon();
		if (!weapon) return;

		const auto aiming = weapon->m_pAuthoritativeAiming;
		if (!aiming) return;

		const auto yaw = aiming->m_Yaw;
		if (!yaw) return;

		// Radar
		m_drawing->DrawFillArea(g_settings.radar_x, g_settings.radar_y, g_settings.radar_width, g_settings.radar_height, ImColor(0, 0, 0, 160));
		
		// Outline
		if (g_settings.radar_outline)
		    m_drawing->DrawBoxOutline(g_settings.radar_x, g_settings.radar_y, g_settings.radar_width, g_settings.radar_height, g_settings.radar_outline_color);

		// You
		if (g_settings.radar_draw_you)
		    m_drawing->AddCircleFilled(ImVec2(g_settings.radar_x + g_settings.radar_width / 2.0f, g_settings.radar_y + g_settings.radar_height / 2.0f), 3.5f, ImColor(255, 255, 255, 200));

		// Cross
		if (g_settings.radar_cross)
		{
			m_drawing->AddLine(
				ImVec2(g_settings.radar_x, g_settings.radar_y + g_settings.radar_height / 2.0f),
				ImVec2(g_settings.radar_x + g_settings.radar_width, g_settings.radar_y + g_settings.radar_height / 2.0f),
				g_settings.radar_cross_color,
				1.0f
			);

			m_drawing->AddLine(
				ImVec2(g_settings.radar_x + g_settings.radar_width / 2.0f, g_settings.radar_y),
				ImVec2(g_settings.radar_x + g_settings.radar_width / 2.0f, g_settings.radar_y + g_settings.radar_height),
				g_settings.radar_cross_color,
				1.0f
			);
		}
		
		// Dots
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			bool teammate = player->m_TeamId == local_player->m_TeamId;
			if (teammate && !g_settings.radar_draw_teammates)
				continue;

			TransformAABBStruct transform = get_transform(player);
			TransformAABBStruct local_transform = get_transform(local_player);;

			Vector3 local_pos = (Vector3)local_transform.Transform.m[3];
			Vector3 pos = (Vector3)transform.Transform.m[3];

			float distance = get_distance(pos, local_pos);
			double angle = -(double)yaw;

			float difference_z = local_pos.z - pos.z;
			float difference_x = local_pos.x - pos.x;
			float x = difference_x * (float)cos(angle) - difference_z * (float)sin(angle);
			float y = difference_x * (float)sin(angle) + difference_z * (float)cos(angle);

			x *= 2;
			x += g_settings.radar_x + (g_settings.radar_width / 2.f);
			y *= 2;
			y += g_settings.radar_y + (g_settings.radar_height / 2.f);

			if (x < g_settings.radar_x) x = g_settings.radar_x;
			if (y < g_settings.radar_y) y = g_settings.radar_y;
			if (x > g_settings.radar_x + g_settings.radar_width - 3) x = g_settings.radar_x + g_settings.radar_width - 3;
			if (y > g_settings.radar_y + g_settings.radar_height - 3) y = g_settings.radar_y + g_settings.radar_height - 3;

			if (distance >= 0.f && distance <= g_settings.radar_distance)
			{
				const auto vehicle = player->GetVehicle();
				if (IsValidPtrWithVTable(vehicle))
				{
					const auto data = get_vehicle_data(vehicle);
					if (IsValidPtrWithVTable(data))
					{
						const auto name = data->m_NameID;
						if (IsValidPtr(name))
						{
							ImColor color = teammate ? g_settings.radar_teammate_vehicles_color : g_settings.radar_enemy_vehicles_color;
							m_drawing->AddText(x, y - 3.f, color, 15.f, FL_CENTER_X, format_vehicle(name).c_str());
						}
					}
				}
				else
				{
					ImColor color = teammate ? g_settings.radar_teammates_color : g_settings.radar_enemies_color;
					m_drawing->AddCircleFilled(ImVec2(x, y), 3.5f, color);
				}
			}
		}

	}
}