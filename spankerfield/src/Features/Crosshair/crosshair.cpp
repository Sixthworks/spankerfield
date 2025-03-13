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
		if (!IsValidPtrWithVTable(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtrWithVTable(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		// Vehicle check
		ClientVehicleEntity* vehicle = local_player->GetVehicle();
		if (IsValidPtrWithVTable(vehicle) && !g_settings.crosshair_in_vehicles) return;

		// Positions
		ImVec2 screen_center = ImVec2(g_globals.g_width / 2.0f, g_globals.g_height / 2.0f);
		ImVec2 horizontal_start = ImVec2(screen_center.x - g_settings.crosshair_size / 2.0f, screen_center.y);
		ImVec2 horizontal_end = ImVec2(screen_center.x + g_settings.crosshair_size / 2.0f, screen_center.y);
		ImVec2 vertical_start = ImVec2(screen_center.x, screen_center.y - g_settings.crosshair_size / 2.0f);
		ImVec2 vertical_end = ImVec2(screen_center.x, screen_center.y + g_settings.crosshair_size / 2.0f);
		
		// Draw the shadow lines
		if (g_settings.crosshair_shadow)
		{
			// Shadow offset, should be made as a setting, but nope
			constexpr float offset = 1.2f;

			ImVec2 horizontal_start_shadow = ImVec2(horizontal_start.x + offset, horizontal_start.y + offset);
			ImVec2 horizontal_end_shadow = ImVec2(horizontal_end.x + offset, horizontal_end.y + offset);
			ImVec2 vertical_start_shadow = ImVec2(vertical_start.x + offset, vertical_start.y + offset);
			ImVec2 vertical_end_shadow = ImVec2(vertical_end.x + offset, vertical_end.y + offset);

			m_drawing->AddLine(horizontal_start_shadow, horizontal_end_shadow, ImColor(0.0f, 0.0f, 0.0f, 0.7f), g_settings.crosshair_thickness);
			m_drawing->AddLine(vertical_start_shadow, vertical_end_shadow, ImColor(0.0f, 0.0f, 0.0f, 0.7f), g_settings.crosshair_thickness);
		}

		// Drawing colored crosshair
		m_drawing->AddLine(horizontal_start, horizontal_end, g_settings.crosshair_color, g_settings.crosshair_thickness);
		m_drawing->AddLine(vertical_start, vertical_end, g_settings.crosshair_color, g_settings.crosshair_thickness);
	}
}