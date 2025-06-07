#include "spot_spam.h"
#include "../../settings.h"
#include "../../Utilities/math.h"
#include "../../Utilities/w2s.h"
#include "../../Utilities/other.h"

constexpr ULONGLONG SPOT_COOLDOWN_MS = 5000;
constexpr ULONGLONG CLEANUP_INTERVAL_MS = 10000;

using namespace big;
namespace plugins
{
    bool should_spot_entity(ClientPlayer* player)
    {
        // Get player's bounding box
        Vector2 box_coords[2];
        TransformAABBStruct transform = get_transform(player);

        if (!get_box_coords(transform, box_coords))
            return false;

        // Calculate box center on screen
        Vector2 box_center;
        box_center.x = (box_coords[0].x + box_coords[1].x) * 0.5f;
        box_center.y = (box_coords[0].y + box_coords[1].y) * 0.5f;

        // Get screen center
        Vector2 screen_center =
        {
            (float)g_globals.g_width * 0.5f,
            (float)g_globals.g_height * 0.5f
        };

        // Calculate distance from screen center to box center
        float distance = sqrtf(powf(screen_center.x - box_center.x, 2) + powf(screen_center.y - box_center.y, 2));

        // Check if entity is close enough to crosshair
        return (distance < g_settings.spot_spam_threshold);
    }

    // Map to track spotted players and their last spotted time
    static std::unordered_map<uint64_t, ULONGLONG> spotted_players;
    static ULONGLONG last_cleanup_time = 0;

    // Individual cooldown system for players
    bool can_spot_player(ClientPlayer* player, ULONGLONG current_time)
    {
        auto it = spotted_players.find(player->m_onlineId.m_personaid);
        if (it != spotted_players.end())
        {
            if (current_time - it->second < SPOT_COOLDOWN_MS)
                return false;

            // Update existing entry
            it->second = current_time;
            return true;
        }

        // Add new entry
        spotted_players.emplace(player->m_onlineId.m_personaid, current_time);
        return true;
    }

    void clean_up_old_entries(ULONGLONG current_time)
    {
        // Only clean up periodically to avoid doing it every frame
        if (current_time - last_cleanup_time < CLEANUP_INTERVAL_MS)
            return;

        last_cleanup_time = current_time;

        for (auto it = spotted_players.begin(); it != spotted_players.end(); )
        {
            if (current_time - it->second > SPOT_COOLDOWN_MS)
                it = spotted_players.erase(it);
            else
                ++it;
        }
    }


    // Love this so much
    static ULONGLONG last_spot_time = 0;
    static ULONGLONG last_damage_time = 0;
    static float last_health = 0.f;

	void spot_spam()
	{
		if (!g_settings.spot_spam) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

        // Update latest damage dealt to the player
        const auto health_comp = local_soldier->m_pHealthComp;
        if (IsValidPtr(health_comp))
        {
            float current_health = health_comp->m_Health;

            // Update
            if (current_health < last_health)
                last_damage_time = GetTickCount64();

            last_health = current_health;

            // Don't spot if health is critically low (might need to focus on survival)
            if (current_health <= 25.f)
                return;

            // Cooldown after taking damage
            if (last_damage_time > 0 && (GetTickCount64() - last_damage_time < 2000))
                return;
        }

        const auto border_input_node = BorderInputNode::GetInstance();
        if (!border_input_node) return;

        const auto input_cache = border_input_node->m_InputCache;
        if (!input_cache) return;

        const auto input = input_cache->m_Event;
        if (!input) return;

        // Current time
        ULONGLONG current_time = GetTickCount64();

        if (current_time - last_spot_time < g_settings.spot_spam_cooldown)
            return;

        // Clean up old entries in our spotted list
        clean_up_old_entries(current_time);

        // Local positioning
        TransformAABBStruct local_transform = get_transform(local_soldier);
        Vector3 local_pos = Vector3(local_transform.Transform.m[3]);

        // Check if there's a target to spot
        bool found_target = false;

        // Iterate through players
        int nearby_enemy_count = 0;

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            const auto player = player_manager->m_ppPlayers[i];
            if (!IsValidPtr(player))
                continue;

            if (player == local_player)
                continue;

            if (player->m_TeamId == local_player->m_TeamId)
                continue;

            const auto soldier = player->GetSoldier();
            if (!IsValidPtr(soldier)) continue;

            if (!soldier->IsAlive()) continue;

            // Update nearby enemies
            TransformAABBStruct transform = get_transform(soldier);
            Vector3 pos = Vector3(transform.Transform.m[3]);

            if (get_distance(local_pos, pos) <= 12.5f)
                nearby_enemy_count++;

            // Should we spot?
            if (should_spot_entity(player) && can_spot_player(player, current_time))
            {
                found_target = true;
                break;
            }
        }

        // Don't spot if there are 2+ enemies nearby
        if (nearby_enemy_count >= 2)
            return;

        if (found_target)
        {
            // Press the spot key (ConceptCommMenu1)
            input[ConceptCommMenu1] = 1.0f;

            // Update last spot time
            last_spot_time = current_time;
        }
	}
}