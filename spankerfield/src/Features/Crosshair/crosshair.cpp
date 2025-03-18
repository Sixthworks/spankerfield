#include "crosshair.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Rendering/draw-list.h"
using namespace big;

namespace plugins
{
	// Original cross crosshair
	void draw_cross_crosshair(ImVec2 screen_center, ImColor main_color, ImColor shadow_color)
	{
		ImVec2 horizontal_start = ImVec2(screen_center.x - g_settings.crosshair_size / 2.0f, screen_center.y);
		ImVec2 horizontal_end = ImVec2(screen_center.x + g_settings.crosshair_size / 2.0f, screen_center.y);
		ImVec2 vertical_start = ImVec2(screen_center.x, screen_center.y - g_settings.crosshair_size / 2.0f);
		ImVec2 vertical_end = ImVec2(screen_center.x, screen_center.y + g_settings.crosshair_size / 2.0f);

		// Draw the shadow lines
		if (g_settings.crosshair_shadow)
		{
			constexpr float offset = 1.2f;
			ImVec2 horizontal_start_shadow = ImVec2(horizontal_start.x + offset, horizontal_start.y + offset);
			ImVec2 horizontal_end_shadow = ImVec2(horizontal_end.x + offset, horizontal_end.y + offset);
			ImVec2 vertical_start_shadow = ImVec2(vertical_start.x + offset, vertical_start.y + offset);
			ImVec2 vertical_end_shadow = ImVec2(vertical_end.x + offset, vertical_end.y + offset);
			m_drawing->AddLine(horizontal_start_shadow, horizontal_end_shadow, shadow_color, g_settings.crosshair_thickness);
			m_drawing->AddLine(vertical_start_shadow, vertical_end_shadow, shadow_color, g_settings.crosshair_thickness);
		}

		// Drawing colored crosshair
		m_drawing->AddLine(horizontal_start, horizontal_end, main_color, g_settings.crosshair_thickness);
		m_drawing->AddLine(vertical_start, vertical_end, main_color, g_settings.crosshair_thickness);
	}

	// Simple dot crosshair
	void draw_dot_crosshair(ImVec2 screen_center, ImColor main_color, ImColor shadow_color)
	{
		float radius = g_settings.crosshair_thickness * 1.5f;

		// Draw shadow
		if (g_settings.crosshair_shadow)
		{
			constexpr float offset = 1.2f;
			m_drawing->AddCircleFilled(ImVec2(screen_center.x + offset, screen_center.y + offset), radius, shadow_color);
		}

		// Draw dot
		m_drawing->AddCircleFilled(screen_center, radius, main_color);
	}

	// Circle crosshair
	void draw_circle_crosshair(ImVec2 screen_center, ImColor main_color, ImColor shadow_color)
	{
		float radius = g_settings.crosshair_size / 2.0f;

		// Draw shadow
		if (g_settings.crosshair_shadow)
		{
			constexpr float offset = 1.2f;
			m_drawing->AddCircle(ImVec2(screen_center.x + offset, screen_center.y + offset), radius, shadow_color);
		}

		// Draw circle
		m_drawing->AddCircle(screen_center, radius, main_color);
	}

	// Triangle (chevron) crosshair
	void draw_triangle_crosshair(ImVec2 screen_center, ImColor main_color, ImColor shadow_color)
	{
		float size = g_settings.crosshair_size / 2.0f;

		ImVec2 points[3] =
		{
			ImVec2(screen_center.x, screen_center.y - size),
			ImVec2(screen_center.x - size, screen_center.y + size / 2),
			ImVec2(screen_center.x + size, screen_center.y + size / 2)
		};

		if (g_settings.crosshair_shadow)
		{
			constexpr float offset = 1.2f;
			ImVec2 shadow_points[3] =
			{
				ImVec2(points[0].x + offset, points[0].y + offset),
				ImVec2(points[1].x + offset, points[1].y + offset),
				ImVec2(points[2].x + offset, points[2].y + offset)
			};

			for (int i = 0; i < 3; i++)
			{
				m_drawing->AddLine(
					shadow_points[i],
					shadow_points[(i + 1) % 3],
					shadow_color,
					g_settings.crosshair_thickness
				);
			}
		}

		// Draw triangle
		for (int i = 0; i < 3; i++)
		{
			m_drawing->AddLine(
				points[i],
				points[(i + 1) % 3],
				main_color,
				g_settings.crosshair_thickness
			);
		}
	}

	// Cross with dot in center crosshair
	void draw_cross_with_dot_crosshair(ImVec2 screen_center, ImColor main_color, ImColor shadow_color)
	{
		// Draw the cross part
		draw_cross_crosshair(screen_center, main_color, shadow_color);

		float dot_radius = g_settings.crosshair_thickness * 1.2f;

		if (g_settings.crosshair_shadow)
		{
			constexpr float offset = 1.2f;
			m_drawing->AddCircleFilled(ImVec2(screen_center.x + offset, screen_center.y + offset), dot_radius, shadow_color);
		}

		m_drawing->AddCircleFilled(screen_center, dot_radius, main_color);
	}

	// T-shape crosshair
	void draw_t_crosshair(ImVec2 screen_center, ImColor main_color, ImColor shadow_color)
	{
		ImVec2 horizontal_start = ImVec2(screen_center.x - g_settings.crosshair_size / 2.0f, screen_center.y - g_settings.crosshair_size / 3.0f);
		ImVec2 horizontal_end = ImVec2(screen_center.x + g_settings.crosshair_size / 2.0f, screen_center.y - g_settings.crosshair_size / 3.0f);
		ImVec2 vertical_start = ImVec2(screen_center.x, screen_center.y - g_settings.crosshair_size / 3.0f);
		ImVec2 vertical_end = ImVec2(screen_center.x, screen_center.y + g_settings.crosshair_size / 2.0f);

		if (g_settings.crosshair_shadow)
		{
			constexpr float offset = 1.2f;
			ImVec2 horizontal_start_shadow = ImVec2(horizontal_start.x + offset, horizontal_start.y + offset);
			ImVec2 horizontal_end_shadow = ImVec2(horizontal_end.x + offset, horizontal_end.y + offset);
			ImVec2 vertical_start_shadow = ImVec2(vertical_start.x + offset, vertical_start.y + offset);
			ImVec2 vertical_end_shadow = ImVec2(vertical_end.x + offset, vertical_end.y + offset);

			m_drawing->AddLine(horizontal_start_shadow, horizontal_end_shadow, shadow_color, g_settings.crosshair_thickness);
			m_drawing->AddLine(vertical_start_shadow, vertical_end_shadow, shadow_color, g_settings.crosshair_thickness);
		}

		m_drawing->AddLine(horizontal_start, horizontal_end, main_color, g_settings.crosshair_thickness);
		m_drawing->AddLine(vertical_start, vertical_end, main_color, g_settings.crosshair_thickness);
	}

	void draw_crosshair()
	{
		if (!g_settings.draw_crosshair) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		// Vehicle check
		ClientVehicleEntity* vehicle = local_player->GetVehicle();
		if (IsValidPtr(vehicle) && !g_settings.crosshair_in_vehicles) return;

		// Positions
		ImVec2 screen_center = ImVec2(g_globals.g_width / 2.0f, g_globals.g_height / 2.0f);

		// Get color with shadow if enabled
		ImColor main_color = g_settings.crosshair_color;
		ImColor shadow_color = ImColor(0.0f, 0.0f, 0.0f, 0.7f);

		// Draw the appropriate crosshair based on type
		switch (g_settings.crosshair_type)
		{
		case 0: // Default cross
			draw_cross_crosshair(screen_center, main_color, shadow_color);
			break;

		case 1: // Dot
			draw_dot_crosshair(screen_center, main_color, shadow_color);
			break;

		case 2: // Circle
			draw_circle_crosshair(screen_center, main_color, shadow_color);
			break;

		case 3: // Triangle
			draw_triangle_crosshair(screen_center, main_color, shadow_color);
			break;

		case 4: // Cross with dot
			draw_cross_with_dot_crosshair(screen_center, main_color, shadow_color);
			break;

		case 5: // T-shape
			draw_t_crosshair(screen_center, main_color, shadow_color);
			break;

		default: // Fallback to default cross
			draw_cross_crosshair(screen_center, main_color, shadow_color);
			break;
		}
	}
}