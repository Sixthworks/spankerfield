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
			if (IsValidPtr(vehicle))
			{
				if (g_settings.infantry_alert_light_tech)
				{
					const auto data = get_vehicle_data(vehicle);
					if (IsValidPtrWithVTable(data))
					{
						const auto type = data->GetVehicleType();
						const VehicleData::VehicleType light_types[] =
						{
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

			float distance = get_distance(pos, local_pos);
			if (distance > g_settings.infantry_alert_distance)
				continue;

			float difference_y = local_pos.y - pos.y;
			if (difference_y > 45.f)
				continue;

			if (!drawing)
			{
				m_drawing->AddText(g_settings.infantry_alert_use_default_pos ? g_globals.g_width / 2.f : g_settings.infantry_alert_x, g_settings.infantry_alert_use_default_pos ? (g_globals.g_height / 2.f) + 100.f : g_settings.infantry_alert_y, g_settings.infantry_alert_color, g_settings.infantry_alert_text_size, FL_CENTER_X, xorstr_("Enemy nearby"));
				drawing = true;
			}
		}

		drawing = false;
	}
}