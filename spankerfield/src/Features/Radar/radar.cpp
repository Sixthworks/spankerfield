#include "radar.h"
#include "../../settings.h"
#include "../../Utilities/other.h"
#include "../../Utilities/math.h"
#include "../../Rendering/draw-list.h"
#include "../../Features/Friend List/friend_list.h"

#pragma warning( disable : 4244 )

#define M_PI 3.14159265358979323846

using namespace big;
namespace plugins
{
	float get_soldier_yaw(ClientPlayer* player)
	{
		if (!IsValidPtr(player))
			return 0.0f;

		const auto soldier = player->GetSoldier();
		if (!IsValidPtr(soldier) || !soldier->IsAlive())
			return 0.0f;

		// Get transform
		TransformAABBStruct transform = get_transform(player);

		// Convert the AABB to SimpleMath Matrix
		Matrix transform_matrix(
			transform.Transform.m[0][0], transform.Transform.m[0][1], transform.Transform.m[0][2], transform.Transform.m[0][3],
			transform.Transform.m[1][0], transform.Transform.m[1][1], transform.Transform.m[1][2], transform.Transform.m[1][3],
			transform.Transform.m[2][0], transform.Transform.m[2][1], transform.Transform.m[2][2], transform.Transform.m[2][3],
			transform.Transform.m[3][0], transform.Transform.m[3][1], transform.Transform.m[3][2], transform.Transform.m[3][3]
		);

		// Extract forward vector (Z axis in DirectX)
		Vector3 forward = transform_matrix.Forward();

		// Calculate yaw from forward vector (in XZ plane)
		float yaw = atan2f(forward.x, forward.z);

		return yaw;
	}

	float get_vehicle_yaw(ClientVehicleEntity* vehicle)
	{
		if (!IsValidPtr(vehicle))
			return 0.0f;

		// Get vehicle transform
		Matrix vehicleTransform;
		vehicle->GetTransform(&vehicleTransform);

		// Extract forward vector
		Vector3 forward = vehicleTransform.Forward();

		// Calculate yaw from forward vector (in XZ plane)
		float yaw = atan2f(forward.x, forward.z);

		return yaw;
	}

	float get_reliable_yaw(ClientPlayer* player)
	{
		if (!IsValidPtr(player))
			return 0.0f;

		// Vehicle
		const auto vehicle = player->GetVehicle();

		// Universal
		return IsValidPtr(vehicle) ? get_vehicle_yaw(vehicle) : get_soldier_yaw(player);
	}

	void draw_radar()
	{
		if (!g_settings.radar) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		// Negative yaw
		float player_yaw = -get_reliable_yaw(local_player);

		// Radar positions
		ImVec2 center(g_settings.radar_x + g_settings.radar_width / 2.0f, g_settings.radar_y + g_settings.radar_height / 2.0f);
		float radius = min(g_settings.radar_width, g_settings.radar_height) / 2.0f;

		// Radar filler
		if (g_settings.radar_circular)
		{
			m_drawing->AddCircleFilled(center, radius, g_settings.radar_background_color);

			if (g_settings.radar_outline)
				m_drawing->AddCircle(center, radius, g_settings.radar_outline_color);
		}
		else
		{
			m_drawing->DrawFillArea(g_settings.radar_x, g_settings.radar_y, g_settings.radar_width, g_settings.radar_height, g_settings.radar_background_color);

			if (g_settings.radar_outline)
				m_drawing->DrawBoxOutline(g_settings.radar_x, g_settings.radar_y, g_settings.radar_width, g_settings.radar_height, g_settings.radar_outline_color);
		}

		// Icon scale factor based on size
		float icon_scale_factor = min(g_settings.radar_width, g_settings.radar_height) / 320.0f; // Assuming 320 is the base size

		// You
		if (g_settings.radar_draw_you)
			m_drawing->AddCircleFilled(center, 3.5f * icon_scale_factor, g_settings.radar_you_color);

		// Cross (for both rectangular and circular radars)
		if (g_settings.radar_cross)
		{
			if (g_settings.radar_circular)
			{
				m_drawing->AddLine(ImVec2(center.x - radius, center.y), ImVec2(center.x + radius, center.y), g_settings.radar_cross_color, 1.0f);
				m_drawing->AddLine(ImVec2(center.x, center.y - radius), ImVec2(center.x, center.y + radius), g_settings.radar_cross_color, 1.0f);
			}
			else
			{
				m_drawing->AddLine(ImVec2(g_settings.radar_x, center.y), ImVec2(g_settings.radar_x + g_settings.radar_width, center.y), g_settings.radar_cross_color, 1.0f);
				m_drawing->AddLine(ImVec2(center.x, g_settings.radar_y), ImVec2(center.x, g_settings.radar_y + g_settings.radar_height), g_settings.radar_cross_color, 1.0f);
			}
		}
		
		// Players
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!IsValidPtr(player))
				continue;

			if (player == local_player)
				continue;

			uint64_t persona_id = player->m_onlineId.m_personaid;
			bool is_friend = plugins::is_friend(persona_id);

			bool teammate = player->m_TeamId == local_player->m_TeamId;

			// Skip drawing teammates if the setting is disabled, unless they are also friends and the friend setting is enabled
			if (teammate && !g_settings.radar_draw_teammates && !(is_friend && g_settings.radar_draw_friends))
				continue;

			// Skip drawing friends if the setting is disabled, unless they are also teammates and the teammate setting is enabled
			if (is_friend && !g_settings.radar_draw_friends && !(teammate && g_settings.radar_draw_teammates))
				continue;

			TransformAABBStruct transform = get_transform(player);
			TransformAABBStruct local_transform = get_transform(local_player);;

			Vector3 local_pos = (Vector3)local_transform.Transform.m[3];
			Vector3 pos = (Vector3)transform.Transform.m[3];

			float distance = get_distance(pos, local_pos);
			double angle = (double)player_yaw;

			float difference_z = local_pos.z - pos.z;
			float difference_x = local_pos.x - pos.x;

			float scale_factor = radius / g_settings.radar_distance;
			float scaled_x = difference_x * scale_factor;
			float scaled_z = difference_z * scale_factor;

			float x = -scaled_z * (float)sin(angle) - scaled_x * (float)cos(angle);
			float y = -scaled_z * (float)cos(angle) + scaled_x * (float)sin(angle);

			x += center.x;
			y += center.y;

			bool in_bounds;
			float edge_threshold = 2.0f;

			if (g_settings.radar_circular)
			{
				float dist_from_center = std::sqrt(std::pow(x - center.x, 2) + std::pow(y - center.y, 2));
				in_bounds = dist_from_center <= (radius - edge_threshold);
			}
			else
			{
				in_bounds = x >= (g_settings.radar_x + edge_threshold) &&
					x <= (g_settings.radar_x + g_settings.radar_width - edge_threshold) &&
					y >= (g_settings.radar_y + edge_threshold) &&
					y <= (g_settings.radar_y + g_settings.radar_height - edge_threshold);
			}

			if (in_bounds && distance >= 0.f)
			{
				const auto vehicle = player->GetVehicle();
				if (IsValidPtr(vehicle))
				{
					const auto data = get_vehicle_data(vehicle);
					if (IsValidPtrWithVTable(data))
					{
						const auto name = data->m_NameID;
						if (IsValidPtr(name))
						{
							// Check length
							size_t length = strnlen(name, 256);

							if (length <= 0 || length > 256)
								return;

							// Determine color based on friend status
							ImColor color;
							if (is_friend)
								color = g_settings.radar_friend_vehicles_color;
							else if (teammate)
								color = g_settings.radar_teammate_vehicles_color;
							else
								color = g_settings.radar_enemy_vehicles_color;

							// Draw formatted vehicle name
							std::string vehicle_name = format_vehicle(name);
							m_drawing->AddText(x, y - 3.f * icon_scale_factor, color, 15.f * icon_scale_factor, FL_CENTER_X, vehicle_name.c_str());
						}
					}
				}
				else
				{
					ImColor color;
					if (is_friend)
						color = g_settings.radar_friends_color;
					else if (teammate)
						color = g_settings.radar_teammates_color;
					else
						color = g_settings.radar_enemies_color;

					m_drawing->AddCircleFilled(ImVec2(x, y), 3.5f * icon_scale_factor, color);
				}
			}
		}
	}
}