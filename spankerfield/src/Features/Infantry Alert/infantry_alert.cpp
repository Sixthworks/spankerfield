#include "infantry_alert.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/other.h"
#include "../../Utilities/math.h"
#include "../../Rendering/draw-list.h"

#define M_PI 3.14159265358979323846f

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
        if (!IsValidPtr(local_player)) return;

        const auto local_soldier = local_player->GetSoldier();
        if (!IsValidPtr(local_soldier)) return;

        if (!local_soldier->IsAlive()) return;

        // Get local player transform and position once
        TransformAABBStruct local_transform = get_transform(local_player);
        Vector3 local_pos = (Vector3)local_transform.Transform.m[3];

        // Get local player forward direction
        Vector3 local_forward =
        {
            local_transform.Transform.m[0][2],
            local_transform.Transform.m[1][2],
            local_transform.Transform.m[2][2]
        };
        local_forward.Normalize();

        // Detection for indicators
        bool enemy_detected = false;
        float closest_distance = FLT_MAX;
        Vector3 closest_enemy_pos;
        float closest_height_diff = 0.0f;

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            const auto player = player_manager->m_ppPlayers[i];
            if (!IsValidPtr(player))
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
                        if (!data->IsGroundVehicle())
                            continue;
                    }
                }
                else
                    continue;
            }

            TransformAABBStruct transform = get_transform(player);
            Vector3 pos = (Vector3)transform.Transform.m[3];
            float distance = get_distance(pos, local_pos);

            // Skip if beyond alert distance
            if (distance > g_settings.infantry_alert_distance)
                continue;

            // Calculate height difference
            float height_diff = pos.y - local_pos.y;
            if (abs(height_diff) > 45.f)
                continue;

            // Track closest enemy
            if (distance < closest_distance)
            {
                enemy_detected = true;
                closest_distance = distance;
                closest_enemy_pos = pos;
                closest_height_diff = height_diff;
            }
        }

        if (enemy_detected && !drawing)
        {
            // Determine vertical direction with better precision
            const char* vertical_indicator;
            if (closest_height_diff > 5.0f)
                vertical_indicator = xorstr_("UP");
            else if (closest_height_diff > 2.5f)
                vertical_indicator = xorstr_("SLIGHTLY UP");
            else if (closest_height_diff < -5.0f)
                vertical_indicator = xorstr_("DOWN");
            else if (closest_height_diff < -2.5f)
                vertical_indicator = xorstr_("SLIGHTLY DOWN");
            else
                vertical_indicator = xorstr_("SAME");

            // Format distance string
            char distance_str[32];
            sprintf_s(distance_str, xorstr_("%.1fm"), closest_distance);

            struct text_line
            {
                const char* text;
                float size_multiplier;
                ImColor color;
            };

            text_line lines[] = {
                { xorstr_("Enemy nearby"), 1.0f, g_settings.infantry_alert_color },
                { vertical_indicator, 0.9f, g_settings.infantry_alert_indicator_color },
                { distance_str, 0.85f, g_settings.infantry_alert_indicator_color }
            };

            const float line_spacing = 20.f;
            const float base_x = g_settings.infantry_alert_use_default_pos ? g_globals.g_width / 2.f : g_settings.infantry_alert_x;
            const float base_y = g_settings.infantry_alert_use_default_pos ? (g_globals.g_height / 2.f) + 100.f : g_settings.infantry_alert_y;

            // Draw the alert text and indicators
            for (int i = 0; i < 3; i++)
            {
                // Check if we should draw indicators
                if (i > 0 && !g_settings.infantry_alert_indicators)
                    continue;

                m_drawing->AddText(
                    base_x,
                    base_y + (i * line_spacing),
                    lines[i].color,
                    g_settings.infantry_alert_text_size * lines[i].size_multiplier,
                    FL_CENTER_X | FL_SHADOW,
                    lines[i].text
                );
            }

            drawing = true;
        }

        drawing = false;
    }
}