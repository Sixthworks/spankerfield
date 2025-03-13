#include "infantry_alert.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/other.h"
#include "../../Utilities/math.h"
#include "../../Rendering/draw-list.h"

#define M_PI 3.14159265358979323846

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
		if (!IsValidPtrWithVTable(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtrWithVTable(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!IsValidPtrWithVTable(player))
				continue;

			if (player == local_player)
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			const auto vehicle = player->GetVehicle();
			if (IsValidPtrWithVTable(vehicle))
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
			if (abs(difference_y) > 45.f)
				continue;

			// Vertical indication
			const char* vertical_indicator;

			if (difference_y > 5.f) // Adding a small threshold to avoid flickering
				vertical_indicator = xorstr_("DOWN");
			else if (difference_y < -5.f)
				vertical_indicator = xorstr_("UP");
			else
				vertical_indicator = ""; // Enemy is roughly at your level

			if (!drawing)
			{
				struct text_line
				{
					const char* text;
					float size_multiplier;
					ImColor color;
				};

				text_line lines[] = {
					{ xorstr_("Enemy nearby"), 1.0f, g_settings.infantry_alert_color  },
					{ vertical_indicator, 0.9f, g_settings.infantry_alert_indicator_color } // You can add other indicators if you want, you can even add a horizontal indicator (pointing arrows)
				};

				const float line_spacing = 20.f;
				const float base_x = g_settings.infantry_alert_use_default_pos ? g_globals.g_width / 2.f : g_settings.infantry_alert_x;
				const float base_y = g_settings.infantry_alert_use_default_pos ? (g_globals.g_height / 2.f) + 100.f : g_settings.infantry_alert_y;

				for (int i = 0; i < 2; i++)
				{
					// Check if we should draw indicators
					if (i > 0 && !g_settings.infantry_alert_indicators)
						continue;

					m_drawing->AddText(
						base_x,
						base_y + (i * line_spacing),
						lines[i].color,
						g_settings.infantry_alert_text_size * lines[i].size_multiplier,
						FL_CENTER_X,
						lines[i].text
					);
				}

				drawing = true;
			}
		}

		drawing = false;
	}
}