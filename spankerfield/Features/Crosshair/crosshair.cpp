#include "crosshair.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Rendering/draw-list.h"

using namespace big;
namespace plugins
{
	void draw_crosshair()
	{
		if (!g_settings.draw_crosshair) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		// Vehicle check
		ClientVehicleEntity* vehicle = local_player->GetVehicle();
		if (vehicle && !g_settings.crosshair_in_vehicles) return;

		// Positions
		ImVec2 screen_center = ImVec2(g_globals.g_width / 2.0f, g_globals.g_height / 2.0f);
		ImVec2 horizontal_start = ImVec2(screen_center.x - g_settings.crosshair_size / 2.0f, screen_center.y);
		ImVec2 horizontal_end = ImVec2(screen_center.x + g_settings.crosshair_size / 2.0f, screen_center.y);
		ImVec2 vertical_start = ImVec2(screen_center.x, screen_center.y - g_settings.crosshair_size / 2.0f);
		ImVec2 vertical_end = ImVec2(screen_center.x, screen_center.y + g_settings.crosshair_size / 2.0f);
		
		// Draw the shadow lines
		if (g_settings.crosshair_shadow)
		{
			ImVec2 horizontal_start_shadow = ImVec2(horizontal_start.x + 1.35f, horizontal_start.y + 1.35f);
			ImVec2 horizontal_end_shadow = ImVec2(horizontal_end.x + 1.35f, horizontal_end.y + 1.35f);
			ImVec2 vertical_start_shadow = ImVec2(vertical_start.x + 1.35f, vertical_start.y + 1.35f);
			ImVec2 vertical_end_shadow = ImVec2(vertical_end.x + 1.35f, vertical_end.y + 1.35f);

			m_drawing->AddLine(horizontal_start_shadow, horizontal_end_shadow, ImColor(0.0f, 0.0f, 0.0f, 0.7f), g_settings.crosshair_thickness);
			m_drawing->AddLine(vertical_start_shadow, vertical_end_shadow, ImColor(0.0f, 0.0f, 0.0f, 0.7f), g_settings.crosshair_thickness);
		}

		// Drawing colored crosshair
		m_drawing->AddLine(horizontal_start, horizontal_end, g_settings.crosshair_color, g_settings.crosshair_thickness);
		m_drawing->AddLine(vertical_start, vertical_end, g_settings.crosshair_color, g_settings.crosshair_thickness);
	}
}