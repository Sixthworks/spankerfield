#include "aimbot.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/math.h"
#include "../../Utilities/w2s.h"
#include "../../Utilities/quartic.h"
#include "../../Utilities/other.h"
#include "../../Rendering/draw-list.h"
#include "../../Features/Friend List/friend_list.h"

#pragma warning( disable : 4244 )

// For some reason, prediction math on close targets that are exactly <=6.55f distance far than you works really badly
// This float was manually tuned
#define CLOSE_TARGET_DIST_FLOAT 6.55f

using namespace big;

namespace big
{
	AimbotPredictor::PredictionResult AimbotPredictor::PredictTarget(
		ClientSoldierEntity* local_entity,
		ClientControllableEntity* enemy,
		const Vector3& aim_point,
		const Matrix& shoot_space)
	{
		PredictionResult result;
		result.predicted_position = aim_point;

		if (!IsValidPtr(local_entity) || !IsValidPtr(enemy))
			return result;

		const auto weapon_component = local_entity->m_pWeaponComponent;
		if (!IsValidPtr(weapon_component))
			return result;

		const auto weapon = weapon_component->GetActiveSoldierWeapon();
		if (!IsValidPtr(weapon))
			return result;

		const auto client_weapon = weapon->m_pWeapon;
		if (!IsValidPtr(client_weapon))
			return result;

		const auto weapon_firing = WeaponFiring::GetInstance();
		if (!IsValidPtrWithVTable(weapon_firing))
			return result;

		const auto firing = weapon_firing->m_pPrimaryFire;
		if (!IsValidPtrWithVTable(firing))
			return result;

		const auto firing_data = firing->m_FiringData;
		if (!IsValidPtrWithVTable(firing_data))
			return result;

		Vector4 spawn_offset_4 = firing_data->m_ShotConfigData.m_PositionOffset;
		Vector3 spawn_offset = Vector3(spawn_offset_4.x, spawn_offset_4.y, spawn_offset_4.z);
		Vector4 initial_speed_4 = firing_data->m_ShotConfigData.m_Speed;
		Vector3 initial_speed = Vector3(initial_speed_4.x, initial_speed_4.y, initial_speed_4.z);

		const auto bullet = firing_data->m_ShotConfigData.m_ProjectileData;
		if (!IsValidPtr(bullet))
			return result;

		float gravity = bullet->m_Gravity;
		Vector3 my_velocity = *local_entity->GetVelocity();
		Vector3 enemy_velocity = *enemy->GetVelocity();
		*(BYTE*)((uintptr_t)enemy + 0x1A) = 159;

		result.zero_angle = DoPrediction(shoot_space.Translation() + spawn_offset,
			result.predicted_position,
			my_velocity,
			enemy_velocity,
			initial_speed,
			gravity,
			WeaponZeroingEntry(-1.0f, -1.0f));
		result.success = true;

		return result;
	}

	float AimbotPredictor::DoPrediction(
		const Vector3& shoot_space,
		Vector3& aim_point,
		const Vector3& my_velocity,
		const Vector3& enemy_velocity,
		const Vector3& bullet_speed,
		const float gravity,
		const WeaponZeroingEntry& zero_entry)
	{
		Vector3 relative_pos = aim_point - shoot_space;
		float distance = relative_pos.Length();

		// Skip complex prediction for close targets
		if (distance <= CLOSE_TARGET_DIST_FLOAT)
		{
			// For close targets, just aim directly without adjusting the aim point
			return atan2(relative_pos.y, sqrt(relative_pos.x * relative_pos.x + relative_pos.z * relative_pos.z));
		}

		float adjusted_gravity = fabs(gravity) * (1.0f + 0.02f * distance / 100.0f);
		Vector3 gravity_vec(0, -adjusted_gravity, 0);

		auto f_approx_pos = [](Vector3& cur_pos, const Vector3& velocity, const Vector3& accel, const float time)->Vector3
			{
				return cur_pos + velocity * time + .5f * accel * time * time;
			};

		float shortest_air_time = 0.0f;

		// Use quartic equation to find time of impact, accounting for full 3D motion
		const double a = 0.25 * gravity * gravity;
		const double b = enemy_velocity.y * gravity;
		const double c = (relative_pos.y * gravity) + enemy_velocity.Dot(enemy_velocity) - bullet_speed.LengthSquared();
		const double d = 2.0 * relative_pos.Dot(enemy_velocity);
		const double e = relative_pos.Dot(relative_pos);

		const auto& roots = solve_quartic(b / (a), c / (a), d / (a), e / (a));

		for (int i = 0; i < 4; ++i)
		{
			if (roots[i].imag() == 0.0 && roots[i].real() > 0.01f)
			{
				if (shortest_air_time == 0.0f || roots[i].real() < shortest_air_time)
					shortest_air_time = static_cast<float>(roots[i].real());
			}
		}

		if (shortest_air_time <= 0.0f)
			return 0.0f;

		// Future target position
		Vector3 future_target_pos = f_approx_pos(aim_point, enemy_velocity, gravity_vec, shortest_air_time);

		// Calculate direction to future position
		Vector3 dir_to_target = future_target_pos - shoot_space;
		float horizontal_dist = sqrt(dir_to_target.x * dir_to_target.x + dir_to_target.z * dir_to_target.z);

		// Calculate vertical angle with bullet drop compensation
		float bullet_speed_mag = bullet_speed.Length();

		// Use projectile motion equations to solve for proper angle
		// For vertical component we need to solve: y = v0y*t + 0.5*g*t^2
		// Where we know y (height difference), t (time of flight), and g (gravity)

		// Calculate required initial vertical velocity component
		float required_v0y = (dir_to_target.y - 0.5f * -adjusted_gravity * shortest_air_time * shortest_air_time) / shortest_air_time;

		// Calculate horizontal velocity needed
		float required_v0_horiz = horizontal_dist / shortest_air_time;

		// Calculate total required initial velocity and check if it's within weapon capabilities
		float required_v0 = sqrt(required_v0y * required_v0y + required_v0_horiz * required_v0_horiz);

		if (required_v0 > bullet_speed_mag * 1.05f) // Allow slight buffer for numerical errors
		{
			// Target is too far for weapon capabilities at this angle
			// Aim at best possible angle instead
			float theta = atan2(required_v0y, required_v0_horiz);
			float max_theta = asin(required_v0 * sin(theta) / bullet_speed_mag);

			// Recalculate target position using best possible angle
			float max_v0y = bullet_speed_mag * sin(max_theta);
			float t_flight = (max_v0y + sqrt(max_v0y * max_v0y + 2 * adjusted_gravity * dir_to_target.y)) / adjusted_gravity;

			// Update aim point with adjusted prediction
			future_target_pos = aim_point + enemy_velocity * t_flight + Vector3(0, 0.5f * -adjusted_gravity * t_flight * t_flight, 0);
		}

		// Update aim point with our calculated future position
		aim_point = future_target_pos;

		// Calculate zero angle adjustment
		return CalculateZeroAngle(shoot_space, aim_point, bullet_speed_mag, adjusted_gravity);
	}

	// New helper function to calculate zero angle
	float AimbotPredictor::CalculateZeroAngle(const Vector3& origin, const Vector3& target, float bullet_speed, float gravity)
	{
		Vector3 dir = target - origin;
		float dist_horiz = sqrt(dir.x * dir.x + dir.z * dir.z);
		float dist_vert = dir.y;

		// Add minimum distance check to prevent calculation issues at close range
		if (dist_horiz <= CLOSE_TARGET_DIST_FLOAT)
		{
			// For close targets, just aim directly at them
			return atan2(dist_vert, dist_horiz);
		}

		// Calculate angle needed to hit target with bullet drop
		float g = fabs(gravity);
		float v = bullet_speed;

		// Quadratic formula to solve for angle: tan(theta) = (v^2 ± sqrt(v^4 - g(g*x^2 + 2y*v^2)))/gx
		float discriminant = powf(v, 4) - g * (g * dist_horiz * dist_horiz + 2 * dist_vert * v * v);

		if (discriminant < 0)
			return atan2(dist_vert, dist_horiz); // No solution possible, just aim directly

		// We want the lower angle solution (usually)
		float tan_theta = (v * v - sqrt(discriminant)) / (g * dist_horiz);
		return atan(tan_theta);
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

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtrWithVTable(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		const auto weapon_component = local_soldier->m_pWeaponComponent;
		if (!weapon_component) return;

		const auto weapon = weapon_component->GetActiveSoldierWeapon();
		if (!weapon) return;

		const auto primary_fire = weapon->m_pPrimary;
		if (!primary_fire) return;

		if (g_settings.aim_must_not_reload)
		{
			if (primary_fire->m_ReloadTimer >= 0.01f)
				return;
		}

		// Controller support
		if (using_controller)
		{
			// Simulate a little bit of mouse movement
			const auto mouse_device = BorderInputNode::GetInstance()->m_pMouse->m_pDevice;
			if (IsValidPtr(mouse_device))
				mouse_device->m_Buffer.x = mouse_device->m_Buffer.x - 1;
		}

		const auto aiming_simulation = weapon->m_pAuthoritativeAiming;
		if (!aiming_simulation) return;

		const auto aim_assist = aiming_simulation->m_pFPSAimer;
		if (!aim_assist) return;

		Matrix shoot_space = weapon->m_pWeapon->m_ShootSpace;

		AimbotTarget target = m_PlayerManager.get_closest_crosshair_player();
		if (!IsValidPtr(target.m_Player)) return;
		if (!target.m_HasTarget) return;

		// Friends list support
		if (g_settings.aim_ignore_friends)
		{
			uint64_t persona_id = target.m_Player->m_onlineId.m_personaid;
			bool is_friend = plugins::is_friend(persona_id);

			// Quit if friend
			if (is_friend)
				return;
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

		// Doing predict
		auto prediction = m_AimbotPredictor.PredictTarget(
			local_soldier,
			prediction_target,
			target.m_WorldPosition,
			shoot_space
		);

		if (!prediction.success)
			return;

		// Update
		target.m_WorldPosition = prediction.predicted_position;

		// Update global prediction point
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

		float horizontal_angle = -atan2(vDir.x, vDir.z);
		float vertical_angle = atan2(vDir.y, sqrt(vDir.x * vDir.x + vDir.z * vDir.z));

		// Use zero angle from prediction if available
		if (prediction.zero_angle != 0.0f)
			vertical_angle = prediction.zero_angle;

		Vector2 BotAngles = {
			horizontal_angle,
			vertical_angle
		};

		BotAngles -= aiming_simulation->m_Sway;
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