#include "c4_bot.h"
#include "../../settings.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/w2s.h"
#include "../../SDK/class_info.h"
#include "../../Utilities/other.h"
#include "../../Features/Friend List/friend_list.h"

using namespace big;
namespace plugins
{
    // Track hotkey state
    bool was_hotkey_pressed = false;

    // Timestamp for last detonation to implement cooldown
    ULONGLONG last_detonation_time = 0;

    // Helper function to check if a raycast would hit a wall between two points
    // Returns true if a wall is likely between the points
    bool is_probably_behind_wall(const Vector3& start, const Vector3& end, float max_vertical_diff)
    {
        // Check if vertical difference is too large
        if (abs(start.z - end.z) > max_vertical_diff)
            return true;

        // Without proper ray tracing, we can use a simpler approach
        // Check vertical difference with intermediate points
        const int check_points = 3; // Number of intermediate points to check

        for (int i = 1; i < check_points; i++)
        {
            float t = static_cast<float>(i) / check_points;
            Vector3 intermediate = Vector3(
                start.x + (end.x - start.x) * t,
                start.y + (end.y - start.y) * t,
                start.z + (end.z - start.z) * t
            );

            // Check if intermediate point has significant vertical difference
            // This is a heuristic: if the z-value changes significantly between check points
            // it might indicate a wall or obstacle
            if (i > 1)
            {
                float prev_t = static_cast<float>(i - 1) / check_points;
                Vector3 prev_point = Vector3(
                    start.x + (end.x - start.x) * prev_t,
                    start.y + (end.y - start.y) * prev_t,
                    start.z + (end.z - start.z) * prev_t
                );

                if (abs(intermediate.z - prev_point.z) > max_vertical_diff / check_points)
                    return true;
            }
        }

        return false;
    }

    // Enhanced enemy position prediction with acceleration and confidence
    Vector3 predict_enemy_position(const Vector3& current_pos, const Vector3& current_velocity, const Vector3& last_velocity, float delay_sec)
    {
        // Calculate acceleration
        Vector3 acceleration = Vector3(
            (current_velocity.x - last_velocity.x) / delay_sec,
            (current_velocity.y - last_velocity.y) / delay_sec,
            (current_velocity.z - last_velocity.z) / delay_sec
        );

        // Limit acceleration to reasonable values (prevents extreme predictions)
        const float max_accel = 15.0f;
        float accel_mag_squared = acceleration.x * acceleration.x +
            acceleration.y * acceleration.y +
            acceleration.z * acceleration.z;

        if (accel_mag_squared > max_accel * max_accel)
        {
            float scale = max_accel / sqrt(accel_mag_squared);
            acceleration.x *= scale;
            acceleration.y *= scale;
            acceleration.z *= scale;
        }

        // Calculate predicted position using physics formula: pos + vel*t + 0.5*accel*t^2
        Vector3 predicted = Vector3(
            current_pos.x + current_velocity.x * delay_sec + 0.5f * acceleration.x * delay_sec * delay_sec,
            current_pos.y + current_velocity.y * delay_sec + 0.5f * acceleration.y * delay_sec * delay_sec,
            current_pos.z + current_velocity.z * delay_sec + 0.5f * acceleration.z * delay_sec * delay_sec
        );

        return predicted;
    }

    void c4_bot()
    {
        if (!g_settings.c4_bot) return;

        const auto game_context = ClientGameContext::GetInstance();
        if (!game_context) return;

        const auto player_manager = game_context->m_pPlayerManager;
        if (!player_manager) return;

        const auto local_player = player_manager->m_pLocalPlayer;
        if (!IsValidPtr(local_player)) return;

        const auto local_soldier = local_player->GetSoldier();
        if (!IsValidPtr(local_soldier) || !local_soldier->IsAlive()) return;

        // Check if player has C4 equipped
        const auto weapon = WeaponFiring::GetInstance();
        if (!IsValidPtr(weapon)) return;

        // Check weapon type for C4
        bool has_c4_equipped = false;

        if (weapon->GetWeaponClass() == WeaponClass::C4)
            has_c4_equipped = true;

        if (!has_c4_equipped && !g_settings.c4_bot_always_active) return;

        // Find C4 explosives in the world
        if (!class_info.ExplosionEntity)
        {
            update_class_info();
            return;
        }

        const auto level = game_context->m_pLevel;
        if (!level) return;

        const auto game_world = level->m_pGameWorld;
        if (!game_world) return;

        // Check for detonation cooldown
        ULONGLONG current_time = GetTickCount64();
        if (current_time - last_detonation_time < 600) // 600ms cooldown, you won't be able to place a C4 again in such time
            return;

        // Get player's position for self-damage check
        TransformAABBStruct player_transform = get_transform(local_soldier);
        Vector3 player_pos = Vector3(player_transform.Transform.m[3]);

        // Track if any C4 would cause self-damage
        bool self_damage_risk = false;

        // Check hotkey state for manual and auto-detonation
        bool hotkey_pressed = GetAsyncKeyState(g_settings.c4_bot_key) != 0;
        bool hotkey_triggered = hotkey_pressed && !was_hotkey_pressed;
        was_hotkey_pressed = hotkey_pressed;

        // Track if any enemy is within detonation radius of any C4
        bool enemy_in_radius_of_any = false;

        // Maximum vertical difference for wall detection
        const float max_vertical_diff = 1.5f;

        // The detonation delay after the click animation begings in seconds
        const float detonation_delay = 0.53f; // 530ms, this was manually tested by me with recording software

        // Cache for player velocity tracking (for acceleration-based prediction)
        static std::unordered_map<int, Vector3> last_enemy_velocities;

        // First, check if ANY C4 would trigger self-damage if detonated
        // And if ANY enemy is within detonation radius of any C4
        std::vector<Vector3> c4_positions;

        EntityIterator<ClientExplosionEntity> explosives(game_world, class_info.ExplosionEntity);
        if (!explosives.front()) return;
        do
        {
            ClientExplosionEntity* explosive = explosives.front()->getObject();
            if (!IsValidPtr(explosive)) continue;

            ClientControllableEntity* explosive_controllable = (ClientControllableEntity*)explosive;
            if (!IsValidPtr(explosive_controllable)) continue;

            // Check if data is valid
            if (!IsValidPtr(explosive_controllable->m_Data)) continue;

            // Check if this is C4 
            auto explosive_type = explosive_controllable->m_Data->GetVehicleType();
            if (explosive_type != VehicleData::VehicleType::UNUSED) continue; // For some reason the C4 type is UNUSED

            // Check if it's the player's C4
            auto explosive_owner = explosive->m_pOwner;
            auto explosive_owner_data = explosive_owner.GetData();

            if (!IsValidPtr(explosive_owner_data))
                continue;

            if (explosive_owner_data != local_player) continue;

            // Get C4 position
            TransformAABBStruct explosive_transform = get_transform(explosive_controllable);
            Vector3 explosive_pos = Vector3(explosive_transform.Transform.m[3]);

            // Store C4 position for later use
            c4_positions.push_back(explosive_pos);

            // Check for self-damage risk
            Vector3 diff_player = player_pos - explosive_pos;
            float distance_player = sqrt(diff_player.x * diff_player.x + diff_player.y * diff_player.y + diff_player.z * diff_player.z);

            // Check if player is within dangerous radius
            if (distance_player <= g_settings.c4_bot_lethal_radius)
                self_damage_risk = true;

        } while (explosives.next());

        // If no C4 found, return
        if (c4_positions.empty()) return;

        // Get player health to determine if we can safely detonate
        float health_player = 0.f, max_health_player = 0.f;

        // Get player's current health and check if we're in hardcore mode
        if (IsValidPtr(local_soldier->m_pHealthComp))
        {
            health_player = local_soldier->m_pHealthComp->m_Health;
            max_health_player = local_soldier->m_pHealthComp->m_MaxHealth;
        }

        // Check if any enemy is within detonation radius and not behind a wall
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            ClientPlayer* player = player_manager->GetPlayerById(i);
            if (!IsValidPtr(player) || player == local_player) continue;

            // Check team (skip teammates)
            if (player->m_TeamId == local_player->m_TeamId) continue;

            // Friends list support
            if (g_settings.c4_bot_ignore_friends)
            {
                uint64_t persona_id = player->m_onlineId.m_personaid;
                if (plugins::is_friend(persona_id))
                    continue;
            }

            ClientSoldierEntity* soldier = player->GetSoldier();
            if (!IsValidPtr(soldier) || !soldier->IsAlive()) continue;

            // Get enemy position
            TransformAABBStruct enemy_transform = get_transform(soldier);
            Vector3 enemy_pos = Vector3(enemy_transform.Transform.m[3]);

            // Get enemy velocity for prediction
            Vector3 enemy_velocity = Vector3(0, 0, 0);
            enemy_velocity = *soldier->GetVelocity();

            // Get last known velocity for this player or initialize
            Vector3 last_velocity = Vector3(0, 0, 0);
            auto it = last_enemy_velocities.find(i);
            if (it != last_enemy_velocities.end())
                last_velocity = it->second;

            // Store current velocity for next check
            last_enemy_velocities[i] = enemy_velocity;

            // Predict enemy position after detonation delay
            Vector3 predicted_enemy_pos = predict_enemy_position(enemy_pos, enemy_velocity, last_velocity, detonation_delay);

            // Get enemy health
            float enemy_health = 0.f, enemy_max_health = 0.f;
            if (IsValidPtr(soldier->m_pHealthComp))
            {
                enemy_health = soldier->m_pHealthComp->m_Health;
                enemy_max_health = soldier->m_pHealthComp->m_MaxHealth;
            }

            // Check if the game is in hardcore mode
            bool is_hardcore = false;
            if (abs(enemy_max_health - 60.0f) < 0.1f)
                is_hardcore = true;

            // Check if enemy is within detonation radius of any C4
            for (const auto& c4_pos : c4_positions)
            {
                // Calculate distance to current and predicted position
                float current_distance = get_distance(enemy_pos, c4_pos);
                float predicted_distance = get_distance(predicted_enemy_pos, c4_pos);

                // Skip if enemy is outside the damage radius now and predicted to be outside
                if (current_distance > g_settings.c4_bot_damage_radius &&
                    predicted_distance > g_settings.c4_bot_damage_radius)
                    continue;

                // Check if enemy is likely behind a wall
                if (is_probably_behind_wall(c4_pos, enemy_pos, max_vertical_diff))
                    continue;

                // Also check if the vertical difference is too great
                if (abs(c4_pos.z - enemy_pos.z) > max_vertical_diff)
                    continue;

                // Calculate damage based on distance
                float damage_factor = 0.f;

                // Use the predicted distance for damage calculation
                float distance_for_damage = predicted_distance;

                if (distance_for_damage <= g_settings.c4_bot_lethal_radius)
                {
                    // Inside lethal radius - damage ranges from 100% to about 90% of max health
                    float t = distance_for_damage / g_settings.c4_bot_lethal_radius;
                    damage_factor = 1.0f - (0.1f * t); // Damage ranges from 1.0 to 0.9 within lethal radius
                }
                else if (distance_for_damage <= g_settings.c4_bot_damage_radius)
                {
                    // Between lethal and damage radius - falloff curve
                    float t = (distance_for_damage - g_settings.c4_bot_lethal_radius) /
                        (g_settings.c4_bot_damage_radius - g_settings.c4_bot_lethal_radius);
                    damage_factor = 0.9f * (1.0f - (t * t)); // Quadratic falloff
                }

                // Calculate estimated damage to the enemy
                float estimated_damage = enemy_max_health * damage_factor;

                // Adjust the min_damage_to_enemy setting for hardcore mode
                float min_damage_to_enemy = g_settings.c4_bot_min_damage_to_enemy;
                if (is_hardcore)
                    min_damage_to_enemy *= (60.0f / 100.0f);

                // Check if the damage is enough to kill or significantly harm the enemy
                if (estimated_damage >= enemy_health || estimated_damage >= min_damage_to_enemy)
                {
                    enemy_in_radius_of_any = true;
                    break;
                }
            }

            if (enemy_in_radius_of_any) break; // No need to check further
        }

        // Determine if we should detonate based on the smart self-damage logic
        bool smart_override = false;
        if (g_settings.c4_bot_smart_self_damage && self_damage_risk)
        {
            // Find closest C4 to player
            float closest_c4_distance = 999.f;
            for (const auto& c4_pos : c4_positions)
            {
                // Calculate distance to player
                float distance = get_distance(player_pos, c4_pos);
                if (distance < closest_c4_distance)
                {
                    closest_c4_distance = distance;
                }
            }

            // Calculate approximate damage based on distance
            float damage_factor = 0.f;
            if (closest_c4_distance <= g_settings.c4_bot_lethal_radius)
            {
                // Inside lethal radius - damage ranges from 100% to about 90% of max health
                float t = closest_c4_distance / g_settings.c4_bot_lethal_radius;
                damage_factor = 1.0f - (0.1f * t); // Damage ranges from 1.0 to 0.9 within lethal radius
            }
            else if (closest_c4_distance <= g_settings.c4_bot_damage_radius)
            {
                // Between lethal and damage radius - falloff curve
                float t = (closest_c4_distance - g_settings.c4_bot_lethal_radius) /
                    (g_settings.c4_bot_damage_radius - g_settings.c4_bot_lethal_radius);

                // Starting from 0.9 at lethal radius, down to minimal damage
                damage_factor = 0.9f * (1.0f - (t * t)); // Quadratic falloff
            }

            // Use the correct max health based on game mode
            float estimated_damage = max_health_player * damage_factor;

            // Allow detonation if player would survive with health buffer
            if (health_player > estimated_damage + g_settings.c4_bot_health_buffer)
                smart_override = true;
        }

        // Determine if we should detonate based on the new logic:
        // 1. If hotkey is pressed for manual detonation
        // 2. OR if auto-detonate is enabled AND (hotkey is held down OR auto-detonate-independently flag is set)
        bool auto_detonate_condition = enemy_in_radius_of_any &&
            (hotkey_pressed || g_settings.c4_bot_auto_detonate_independently);

        bool should_detonate = hotkey_triggered ||
            (g_settings.c4_bot_auto_detonate && auto_detonate_condition);

        // Apply self-damage prevention logic
        if (should_detonate && g_settings.c4_bot_prevent_self_damage && self_damage_risk && !smart_override)
            should_detonate = false;

        // If we should detonate, do it once for all C4s (they all detonate together)
        if (should_detonate)
        {
            // Update the last detonation time for cooldown
            last_detonation_time = current_time;

            // Trigger detonation - simulate the appropriate key press or click
            const auto border_input_node = BorderInputNode::GetInstance();
            if (!border_input_node) return;

            const auto input_cache = border_input_node->m_InputCache;
            if (!input_cache) return;

            const auto input = input_cache->m_Event;
            if (!input) return;

            // Trigger the fire action
            input[ConceptFire] = 1.0f;
        }
    }
}