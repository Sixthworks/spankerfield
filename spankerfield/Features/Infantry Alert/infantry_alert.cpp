#include "infantry_alert.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/other.h"
#include "../../Utilities/math.h"
#include "../../Rendering/draw-list.h"

using namespace big;
namespace plugins
{
	static bool drawing = false;
	void infantry_alert()
	{
		if (!g_settings.infantry_alert) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			const auto vehicle = player->GetVehicle();
			if (vehicle)
			{
				if (g_settings.infantry_alert_light_tech)
				{
					const auto data = get_vehicle_data(vehicle);
					if (IsValidPtrWithVTable(data))
					{
						const auto type = data->GetVehicleType();
						const VehicleData::VehicleType light_types[] = { 
							VehicleData::VehicleType::HELITRANS, 
							VehicleData::VehicleType::JEEP, 
							VehicleData::VehicleType::CAR 
						};

						bool indeed = false;
						for (int i = 0; i < sizeof(light_types) / sizeof(*light_types); i++)
						{
							if (type == light_types[i])
								indeed = true;
						}

						if (!indeed)
							continue;
					}
				}
				else
					continue;
			}

			TransformAABBStruct transform = get_transform(player);
			TransformAABBStruct local_transform = get_transform(local_player);;

			Vector3 local_pos = (Vector3)local_transform.Transform.m[3];
			Vector3 pos = (Vector3)transform.Transform.m[3];

			float difference_z = local_pos.z - pos.z;
			if (difference_z > 75.f)
				continue;

			float distance = get_distance(pos, local_pos);
			if (distance > g_settings.infantry_alert_distance)
				continue;

			/*
			TODO: Aim assist angles are Vec2, figure out a way with Vec3
			Right now it's just a distance check without FOV :(
			
			Vector3 angle = calc_angle(local_pos, pos);
			Vector3 eye_angles = { aim_assist->m_AimAngles.x, aim_assist->m_AimAngles.y, 0.f };

			float fov = calc_fov(eye_angles, angle);
			if (fov < 70.0f)
				continue;
			*/

			if (!drawing)
			{
				m_drawing->AddText(g_globals.g_width / 2.f, (g_globals.g_height / 2.f) + 100.f, g_settings.infantry_alert_color, 20.f, FL_CENTER_X, xorstr_("Enemy nearby"));
				drawing = true;
			}
		}

		drawing = false;
	}
}