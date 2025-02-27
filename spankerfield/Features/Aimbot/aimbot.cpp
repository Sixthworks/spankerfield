#include "aimbot.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/math.h"
#include "../../Utilities/w2s.h"
#include "../../Utilities/quartic.h"
#include "../../Utilities/other.h"
#include "../../Rendering/draw-list.h"

#pragma warning( disable : 4244 )

using namespace big;

namespace big
{
	float AimbotPredictor::PredictLocation(ClientSoldierEntity* local_entity, ClientControllableEntity* enemy, Vector3& aim_point, const Matrix& shoot_space)
	{
		if (!IsValidPtr(local_entity))
			return 0;

		if (!IsValidPtr(enemy))
			return 0;

		const auto weapon_component = local_entity->m_pWeaponComponent;
		if (!IsValidPtr(weapon_component))
			return 0;

		const auto weapon = weapon_component->GetActiveSoldierWeapon();
		if (!IsValidPtr(weapon))
			return 0;

		const auto client_weapon = weapon->m_pWeapon;
		if (!IsValidPtr(client_weapon))
			return 0;

		const auto weapon_firing = WeaponFiring::GetInstance();
		if (!IsValidPtrWithVTable(weapon_firing))
			return 0;

		const auto firing = weapon_firing->m_pPrimaryFire;
		if (!IsValidPtrWithVTable(firing))
			return 0;

		const auto firing_data = firing->m_FiringData;
		if (!IsValidPtrWithVTable(firing_data))
			return 0;

		const auto weapon_modifier = client_weapon->m_pWeaponModifier;
		auto zero_entry = WeaponZeroingEntry(-1.0f, -1.0f);
		if (IsValidPtr(weapon_modifier))
		{
			const auto zeroing = weapon_modifier->m_ZeroingModifier;
			if (IsValidPtr(zeroing))
			{
				int level_index = weapon_component->m_ZeroingDistanceLevel;
				zero_entry = zeroing->GetZeroLevelAt(level_index);
			}
		}

		Vector4 spawn_offset_4 = firing_data->m_ShotConfigData.m_PositionOffset;
		Vector3 spawn_offset = Vector3(spawn_offset_4.x, spawn_offset_4.y, spawn_offset_4.z);
		Vector4 initial_speed_4 = firing_data->m_ShotConfigData.m_Speed;
		Vector3 initial_speed = Vector3(initial_speed_4.x, initial_speed_4.y, initial_speed_4.z);

		const auto bullet = firing_data->m_ShotConfigData.m_ProjectileData;
		if (!IsValidPtr(bullet))
			return 0;

		float gravity = bullet->m_Gravity;
		Vector3 my_velocity = *local_entity->GetVelocity();
		Vector3 enemy_velocity = *enemy->GetVelocity();

		return DoPrediction(shoot_space.Translation() + spawn_offset, aim_point, my_velocity, enemy_velocity, initial_speed, gravity, zero_entry);
	}

	float AimbotPredictor::DoPrediction(const Vector3& shoot_space, Vector3& aim_point, const Vector3& my_velocity, const Vector3& enemy_velocity, const Vector3& bullet_speed, const float gravity, const WeaponZeroingEntry& zero_entry)
	{
		Vector3 relative_pos = aim_point - shoot_space;
		Vector3 gravity_vec(0, -fabs(gravity), 0);
		auto f_approx_pos = [](Vector3& cur_pos, const Vector3& velocity, const Vector3& accel, const float time)->Vector3
		{
			return cur_pos + velocity * time + .5f * accel * time * time;
		};

		float shortest_air_time = 0.0f;

		const double a = 0.25 * gravity * gravity;
		const double b = enemy_velocity.y * gravity;
		const double c = (relative_pos.y * gravity) + enemy_velocity.Dot(enemy_velocity) - bullet_speed.LengthSquared();
		const double d = 2.0 * relative_pos.Dot(enemy_velocity);
		const double e = relative_pos.Dot(relative_pos);

		const auto& roots = solve_quartic(b / (a), c / (a), d / (a), e / (a));

		for (int i = 0; i < 4; ++i)
		{
			if (roots[i].real() > 0.0f && (shortest_air_time == 0.0f || roots[i].real() < shortest_air_time))
				shortest_air_time = roots[i].real();
		}

		if (shortest_air_time <= 0.0f)
			return 0.0f;
		
		// Extrapolate position on velocity, and account for bullet drop
		aim_point = f_approx_pos(aim_point, enemy_velocity, gravity_vec, shortest_air_time);

		float zero_angle = 0.0f;
		if (zero_entry.m_ZeroDistance > 0.0f)
		{
			float zero_air_time = zero_entry.m_ZeroDistance / bullet_speed.Length();
			float zero_drop = 0.5f * fabs(gravity) * zero_air_time * zero_air_time;
			zero_angle = atan2(zero_drop, zero_entry.m_ZeroDistance);
		}

		return zero_angle;
	}

	AimbotSmoother::AimbotSmoother()
	{

	}

	void AimbotSmoother::Update(float DeltaTime)
	{
		m_CurAimTime += Vector2(DeltaTime, DeltaTime);
		if (m_CurAimTime.x > m_TimeToTarget.x)
			m_CurAimTime.x = m_TimeToTarget.x;

		if (m_CurAimTime.y > m_TimeToTarget.y)
			m_CurAimTime.y = m_TimeToTarget.y;
	}

	void AimbotSmoother::ResetTimes(Vector2& NewAimTimes)
	{
		m_TimeToTarget = NewAimTimes;
		m_CurAimTime = Vector2(0, 0);
	}

	void AimbotSmoother::SmoothAngles(Vector2& CurrentAngles, Vector2& TargetAngles)
	{
		Vector2 AimPercent = m_CurAimTime / m_TimeToTarget;
		cerp_angle(CurrentAngles, TargetAngles, AimPercent.x, AimPercent.y);
	}

	BotPlayerManager::BotPlayerManager()
	{

	}

	void BotPlayerManager::update_players()
	{
		update_all_player_list();
		update_closest_crosshair_list();
	}

	void BotPlayerManager::update_all_player_list()
	{
		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		m_PlayerList.clear();
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!IsValidPtrWithVTable(player))
				continue;

			m_PlayerList.push_back(player);
		}
	}

	void BotPlayerManager::update_closest_crosshair_list()
	{
		float closest_distance = 99999.0f;
		ClientPlayer* closest_player = nullptr;
		Vector3 closest_world_pos;
		bool target_found_this_frame = false;
		m_ClosestPlayer.m_HasTarget = false;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtrWithVTable(local_player)) return;

		float fov_radius = get_fov_radius(g_settings.aim_fov, (float)g_globals.g_width, (float)g_globals.g_height);

		for (const auto player : m_PlayerList)
		{
			if (!IsValidPtrWithVTable(player))
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			const auto soldier = player->GetSoldier();
			if (!IsValidPtrWithVTable(soldier))
				continue;

			if (!soldier->IsAlive())
				continue;

			Vector3 head_vec;
			bool got_bone = false;

			// Check if player is in vehicle
			const auto vehicle = player->GetVehicle();
			if (IsValidPtr(vehicle))
			{
				// Get player bones even when in vehicle
				const auto ragdoll = soldier->m_pRagdollComponent;
				if (!IsValidPtr(ragdoll))
					continue;

				// Force update pose for vehicle passenger
				*(BYTE*)((uintptr_t)soldier + 0x1A) = 159;
				soldier->m_Occluded = false;

				if (g_settings.aim_auto_bone)
				{
					static const UpdatePoseResultData::BONES bone_priority[] =
					{
						UpdatePoseResultData::BONES::Head,
						UpdatePoseResultData::BONES::Neck,
						UpdatePoseResultData::BONES::Spine1,
						UpdatePoseResultData::BONES::Spine2,
						UpdatePoseResultData::BONES::Hips
					};

					for (const auto& bone : bone_priority)
					{
						if (ragdoll->GetBone(bone, head_vec))
						{
							got_bone = true;
							break;
						}
					}
				}
				else
				{
					got_bone = ragdoll->GetBone((UpdatePoseResultData::BONES)g_settings.aim_bone, head_vec);
				}
			}
			else 
			{
				if (!g_settings.aim_must_be_visible)
				{
					if (soldier->m_Occluded)
						continue;
				}

				const auto ragdoll = soldier->m_pRagdollComponent;
				if (!IsValidPtr(ragdoll))
					continue;

				// Bone selection for infantry
				if (g_settings.aim_auto_bone)
				{
					static const UpdatePoseResultData::BONES bone_priority[] =
					{
						UpdatePoseResultData::BONES::Head,
						UpdatePoseResultData::BONES::Neck,
						UpdatePoseResultData::BONES::Spine1,
						UpdatePoseResultData::BONES::Spine2,
						UpdatePoseResultData::BONES::Hips
					};

					for (const auto& bone : bone_priority)
					{
						if (ragdoll->GetBone(bone, head_vec))
						{
							got_bone = true;
							break;
						}
					}
				}
				else
				{
					got_bone = ragdoll->GetBone((UpdatePoseResultData::BONES)g_settings.aim_bone, head_vec);
				}
			}

			if (!got_bone)
				continue;

			Vector3 screen_vec;
			if (!world_to_screen(head_vec, screen_vec))
				continue;

			Vector2 screen_size = get_screen_size();
			if (screen_vec.x > screen_size.x || screen_vec.y > screen_size.y)
				continue;

			float screen_distance = get_screen_distance(screen_vec, screen_size);
			
			// FOV check anyway
			if (screen_distance > fov_radius)
				continue;

			// Target selection based on settings
			bool should_update_target = false;
			
			if (g_settings.aim_target_selection == 0) // FOV
			{
				should_update_target = screen_distance < closest_distance;
			}
			else if (g_settings.aim_target_selection == 1) // FOV + Distance
			{
				const auto local_soldier = local_player->GetSoldier();
				if (!IsValidPtrWithVTable(local_soldier))
					continue;

				Matrix transform;
				local_soldier->GetTransform(&transform);

				float world_distance = (head_vec - transform.Translation()).Length();
				float fov_weight = 1.0f - (screen_distance / fov_radius); // 0 to 1, where 1 - is the center of the screen
				float weighted_distance = world_distance * (1.0f - fov_weight * 0.5f); // FOV is 50% in terms of preferability
				
				should_update_target = weighted_distance < closest_distance;
				closest_distance = should_update_target ? weighted_distance : closest_distance;
			}

			if (should_update_target)
			{
				closest_player = player;
				closest_world_pos = head_vec;
				target_found_this_frame = true;
			}
		}

		if (!target_found_this_frame)
			return;

		m_ClosestPlayer.m_HasTarget = true;
		m_ClosestPlayer.m_Player = closest_player;
		m_ClosestPlayer.m_WorldPosition = closest_world_pos;
	}

	std::vector<ClientPlayer*>& BotPlayerManager::get_player_list()
	{
		return m_PlayerList;
	}

	AimbotTarget& BotPlayerManager::get_closest_crosshair_player()
	{
		return m_ClosestPlayer;
	}

	float BotPlayerManager::get_screen_distance(Vector3& enemy_screen, Vector2 screen_size)
	{
		Vector2 screen_center = screen_size / 2.0f;
		Vector2 distance_from_center = Vector2(enemy_screen.x, enemy_screen.y) - screen_center;

		return distance_from_center.Length();
	}
}

namespace plugins
{
	void aimbot(float delta_time)
	{
		// Controller support
		bool using_controller = g_settings.aim_support_controller && is_left_trigger_pressed(0.5f);

		// Pressed status
		auto is_pressed = [=]() {
			return GetAsyncKeyState(g_settings.aim_key) != 0 || using_controller;
		};

		if (!is_pressed())
			return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtrWithVTable(local_player)) return;

		if (local_player->GetVehicle()) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtrWithVTable(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		const auto weapon_component = local_soldier->m_pWeaponComponent;
		if (!weapon_component) return;

		const auto weapon = weapon_component->GetActiveSoldierWeapon();
		if (!weapon) return;

		const auto primary_fire = weapon->m_pPrimary;
		if (!primary_fire) return;

		const auto client_weapon = weapon->m_pWeapon;
		if (!client_weapon) return;

		// Get weapon zeroing capabilities
		std::vector<float> available_zeroing_distances;
		const auto weapon_modifier = client_weapon->m_pWeaponModifier;
		if (IsValidPtr(weapon_modifier))
		{
			const auto zeroing = weapon_modifier->m_ZeroingModifier;
			if (IsValidPtr(zeroing))
			{
				// Try all possible zeroing levels (usually 0-4 in BF)
				for (int i = 0; i < 5; i++)
				{
					const auto zero_entry = zeroing->GetZeroLevelAt(i);
					if (zero_entry.m_ZeroDistance > 0.0f)
					{
						available_zeroing_distances.push_back(zero_entry.m_ZeroDistance);
					}
				}
			}
		}

		if (g_settings.aim_must_not_reload)
		{
			if (primary_fire->m_ReloadTimer >= 0.01f) // 0.01 is the best value we can use, because sometimes the value can be approx 0.0000012517 when not reloading
			{
				// Don't run the code if reloading
				return;
			}
		}

		// Controller support
		if (using_controller)
		{
			// Simulate a little bit of mouse movement, otherwise aiming simulation won't be valid
			const auto mouse_device = BorderInputNode::GetInstance()->m_pMouse->m_pDevice;
			if (IsValidPtr(mouse_device))
				mouse_device->m_Buffer.x = mouse_device->m_Buffer.x - 1;
		}

		const auto aiming_simulation = weapon->m_pAuthoritativeAiming;
		if (!aiming_simulation) return;

		const auto aim_assist = aiming_simulation->m_pFPSAimer;
		if (!aim_assist) return;

		Matrix shoot_space = client_weapon->m_ShootSpace;

		AimbotTarget target = m_PlayerManager.get_closest_crosshair_player();
		if (!IsValidPtr(target.m_Player)) return;

		if (!target.m_HasTarget) return;

		Vector3 temporary_aim = target.m_WorldPosition;

		// Calculate distance to target before prediction
		float distance_to_target = (temporary_aim - shoot_space.Translation()).Length();

		// Auto adjust zeroing based on distance
		float current_zeroing_distance = 0.0f;
		if (!available_zeroing_distances.empty())
		{
			// Find best zeroing distance
			float best_zeroing = available_zeroing_distances[0];
			float min_diff = std::abs(distance_to_target - best_zeroing);

			for (const float zeroing_distance : available_zeroing_distances)
			{
				float diff = std::abs(distance_to_target - zeroing_distance);
				if (diff < min_diff)
				{
					min_diff = diff;
					best_zeroing = zeroing_distance;
				}
			}

			// Find and set the zeroing level
			if (IsValidPtr(weapon_modifier))
			{
				const auto zeroing = weapon_modifier->m_ZeroingModifier;
				if (IsValidPtr(zeroing))
				{
					// Try to find matching zeroing level
					for (int i = 0; i < 5; i++)
					{
						const auto zero_entry = zeroing->GetZeroLevelAt(i);
						if (std::abs(zero_entry.m_ZeroDistance - best_zeroing) < 0.1f)
						{
							weapon_component->m_ZeroingDistanceLevel = i;
							current_zeroing_distance = zero_entry.m_ZeroDistance;
							break;
						}
					}
				}
			}
		}

		// Get correct entity for prediction
		ClientControllableEntity* prediction_target = nullptr;
		if (IsValidPtr(target.m_Player->GetVehicle()))
		{
			prediction_target = target.m_Player->GetVehicle();
		}
		else
		{
			prediction_target = target.m_Player->GetSoldier();
		}

		float zero_theta_offset = m_AimbotPredictor.PredictLocation(local_soldier, prediction_target, temporary_aim, shoot_space);

		// Apply zeroing correction to prediction
		if (current_zeroing_distance > 0.0f)
		{
			// Calculate correction factor based on distance difference
			float distance_diff = current_zeroing_distance - distance_to_target;
			
			// Calculate correction based on distance
			float base_correction = 1.0f;
			if (distance_diff != 0.0f) {
				float correction_strength = g_settings.aim_zeroing_correction * 0.1f; // Reducing force
				float distance_ratio = std::abs(distance_diff) / current_zeroing_distance;
				
				if (distance_diff > 0) { // Target is closer
					base_correction = 1.0f - (distance_ratio * correction_strength);
				} else { // Target is further
					base_correction = 1.0f + (distance_ratio * correction_strength);
				}
			}

			// Apply correction to aim point
			Vector3 aim_direction = temporary_aim - shoot_space.Translation();
			float current_distance = aim_direction.Length();
			aim_direction.Normalize();

			// Applying correction only to the vertical part
			Vector3 horizontal(aim_direction.x, 0.0f, aim_direction.z);
			horizontal.Normalize();
			Vector3 vertical(0.0f, aim_direction.y, 0.0f);
			
			temporary_aim = shoot_space.Translation() + 
				(horizontal * current_distance) +  // Horizontal
				(vertical * current_distance * base_correction); // Vertical
		}

		target.m_WorldPosition = temporary_aim;
		
		// Credit VincentVega
		g_globals.g_pred_aim_point = target.m_WorldPosition;
		g_globals.g_has_pred_aim_point = target.m_HasTarget;

		if (g_settings.aim_max_time_to_target <= 0.f) return;

		if (target.m_Player != m_PreviousTarget.m_Player)
		{
			Vector2 vec_rand =
			{
				generate_random_float(g_settings.aim_min_time_to_target, g_settings.aim_max_time_to_target),
				generate_random_float(g_settings.aim_min_time_to_target, g_settings.aim_max_time_to_target)
			};
			m_AimbotSmoother.ResetTimes(vec_rand);
		}

		m_AimbotSmoother.Update(delta_time);

		Vector3 vDir = target.m_WorldPosition - shoot_space.Translation();
		vDir.Normalize();

		// Fix vertical angle calculation
		float vertical_angle = atan2(vDir.y, sqrt(vDir.x * vDir.x + vDir.z * vDir.z));
		float elevation_adjustment = vertical_angle * (1.0f - exp(-vDir.Length() / 175.0)); 

		Vector2 BotAngles = {
			-atan2(vDir.x, vDir.z),
			vertical_angle + elevation_adjustment  // Changed from - to +
		};

		BotAngles -= aiming_simulation->m_Sway;
		BotAngles.y += zero_theta_offset;  // Changed from - to +
		m_AimbotSmoother.SmoothAngles(aim_assist->m_AimAngles, BotAngles);
		aim_assist->m_AimAngles = BotAngles;
		m_PreviousTarget = target;
	}

	void draw_fov()
	{
		if (!g_settings.aim_fov_method) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtrWithVTable(local_player)) return;

		if (local_player->GetVehicle()) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtrWithVTable(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		// FOV
		float fov_radius = get_fov_radius(g_settings.aim_fov, (float)g_globals.g_width, (float)g_globals.g_height);

		if (g_settings.aim_fov_method && g_settings.aim_draw_fov)
			m_drawing->AddCircle(ImVec2(g_globals.g_width / 2.f, g_globals.g_height / 2.f), fov_radius, g_settings.aim_fov_color);
	}
}
