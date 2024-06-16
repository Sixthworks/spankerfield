#include "health_bar.h"
#include "../../settings.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/other.h"
#include "../../global.h"

using namespace big;
namespace plugins
{
	void draw_health_bar()
	{
		if (!g_settings.draw_health_bar) return;

		// Idk why would you use it, you don't even select anything
		if (!g_settings.health_bar_soldier && !g_settings.health_bar_vehicle) return;
		
		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		// Vehicle, we'll use it later
		ClientVehicleEntity* vehicle = local_player->GetVehicle();
		bool using_vehicle = false;

		// Variables
		float health_player = 0.f, max_health_player = 0.f, health_vehicle = 0.f, max_health_vehicle = 0.f;
		
		// Player
		if (IsValidPtr(local_soldier->m_pHealthComp))
		{
			health_player = local_soldier->m_pHealthComp->m_Health;
			max_health_player = local_soldier->m_pHealthComp->m_MaxHealth;
		}

		// Vehicle
		if (IsValidPtr(vehicle))
		{
			if (IsValidPtr(vehicle->m_pHealthComp) && vehicle->m_pHealthComp->m_VehicleHealth)
				health_vehicle = vehicle->m_pHealthComp->m_VehicleHealth;

			const auto data = get_vehicle_data(vehicle);
			if (IsValidPtrWithVTable(data))
			{
				if (data->m_MaxHealth)
					max_health_vehicle = data->m_MaxHealth;
			}

			using_vehicle = true;
		}

		// We will modify it later
		float pos_y = g_settings.health_bar_y;

		// Lambda drawing solution
		auto draw_health_bar = [&](float screen_width, float pos_y, float rect_width, float rect_height, float health, float max_health)
		{
			// Fixed black rectangle position based on settings
			ImVec2 black_rect_pos(g_settings.health_bar_use_center_x ? (screen_width - rect_width) / 2 : g_settings.health_bar_x, pos_y);

			m_drawing->DrawBoxOutline(black_rect_pos.x, black_rect_pos.y, rect_width, rect_height, ImColor(0, 0, 0, 200));

			// Calculate health bar width based on health (clamped to max width)
			float health_bar_width = fminf((health / max_health) * rect_width, rect_width); // Use fminf for max width clamping
			ImVec2 health_bar_pos(black_rect_pos.x, black_rect_pos.y);
			ImVec2 health_bar_size(health_bar_pos.x + health_bar_width, black_rect_pos.y + rect_height);

			// Classic
			float normalized_health = static_cast<float>(health) / max_health;
			BYTE red = static_cast<BYTE>(255 - normalized_health * 255);
			BYTE green = static_cast<BYTE>(normalized_health * 255);
			ImColor health_bar_color(red, green, 0, 255);

			m_drawing->AddRectFilled(health_bar_pos, health_bar_size, g_settings.health_bar_use_default_color ? health_bar_color : g_settings.health_bar_color, 0.0f);
		};
		
		// Drawing
		if (g_settings.health_bar_soldier)
		{
			draw_health_bar((float)g_globals.g_width, pos_y, g_settings.health_bar_width, g_settings.health_bar_height, health_player, max_health_player);
			pos_y += g_settings.health_bar_height + g_settings.health_bar_spacing;
		}

		if (using_vehicle && g_settings.health_bar_vehicle)
			draw_health_bar((float)g_globals.g_width, pos_y, g_settings.health_bar_width, g_settings.health_bar_height, health_vehicle, max_health_vehicle);
	}
}