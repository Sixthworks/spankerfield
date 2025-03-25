#include "aimbot.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/math.h"
#include "../../Utilities/w2s.h"
#include "../../Utilities/quartic.h"
#include "../../Utilities/other.h"
#include "../../Utilities/firing_data.h"
#include "../../Rendering/draw-list.h"
#include "../../Features/Friend List/friend_list.h"

#pragma warning( disable : 4244 )

// For some reason, prediction math on close targets that are exactly <=6.57f distance far than you works really badly
// This float was manually tuned
#define CLOSE_TARGET_DIST_FLOAT 6.57f

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


		// Check if player is in vehicle
		const auto local_player = ClientGameContext::GetInstance()->m_pPlayerManager->m_pLocalPlayer;
		const auto local_vehicle = local_player->GetVehicle();
		bool is_in_vehicle = IsValidPtr(local_vehicle);

		// Weapon variables we need to populate
		Vector3 fire_position;
		Vector3 spawn_offset;
		Vector3 initial_speed;
		Vector3 my_velocity;
		WeaponZeroingEntry zero_entry(-1.0f, -1.0f);

		// Get weapon data (either from vehicle or infantry)
		if (is_in_vehicle)
		{
			auto vehicle_turret = VehicleTurret::GetInstance();
			if (!IsValidPtr(vehicle_turret))
				return result;

			fire_position = vehicle_turret->GetVehicleCrosshair();
			my_velocity = local_vehicle->m_VelocityVec;
		}
		else
		{
			fire_position = shoot_space.Translation();
			my_velocity = *local_entity->GetVelocity();

			// Get zeroing information for infantry
			if (auto weapon_component = local_entity->m_pWeaponComponent; IsValidPtr(weapon_component))
			{
				if (auto weapon = weapon_component->GetActiveSoldierWeapon(); IsValidPtr(weapon))
				{
					if (auto client_weapon = weapon->m_pWeapon; IsValidPtr(client_weapon) && IsValidPtr(client_weapon->m_pWeaponModifier))
					{
						auto* zeroing_modifier = client_weapon->m_pWeaponModifier->m_ZeroingModifier;
						if (IsValidPtr(zeroing_modifier))
						{
							int zero_level_index = weapon_component->m_ZeroingDistanceLevel;
							zero_entry = zeroing_modifier->GetZeroLevelAt(zero_level_index);
						}
					}
				}
			}
		}

		// Get common firing data (works for both vehicle and infantry)
		const auto weapon_firing = get_weapon_firing();
		if (!IsValidPtr(weapon_firing))
			return result;

		const auto firing_data = WeaponFiringDataRetriever::GetSafeFiringData(weapon_firing, is_in_vehicle);
		if (!IsValidPtr(firing_data))
			return result;

		const auto bullet = firing_data->m_ShotConfigData.m_ProjectileData;
		if (!IsValidPtr(bullet))
			return result;

		Vector4 initial_speed_4 = firing_data->m_ShotConfigData.m_Speed;
		initial_speed = Vector3(initial_speed_4.x, initial_speed_4.y, initial_speed_4.z);

		// Get enemy velocity
		Vector3 enemy_velocity = *enemy->GetVelocity();

		// This is really retarded, but it's better to access the VelocityVec of the vehicle instead of GetVelocity()
		if (IsValidPtr(enemy->m_Data))
		{
			if (enemy->m_Data->GetVehicleCategory() != VehicleData::VehicleCategory::UNUSED)
			{
				const auto vehicle = reinterpret_cast<ClientVehicleEntity*>(enemy);
				enemy_velocity = vehicle->m_VelocityVec;
			}
		}

		// Force update
		*(BYTE*)((uintptr_t)enemy + 0x1A) = 159;

		// Gravity
		float gravity = bullet->m_Gravity;

		if (gravity < -500.f || gravity > 500.f) gravity = 0.f;

		// Calculate prediction
		bool prediction_success = DoPrediction(
			fire_position + spawn_offset,
			result.predicted_position,
			my_velocity,
			enemy_velocity,
			initial_speed,
			gravity,
			zero_entry);

		result.success = prediction_success;
		return result;
	}

	bool AimbotPredictor::DoPrediction(
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
			return false;

		// Use the bullet speed magnitude for calculations
		float bullet_velocity = bullet_speed.Length();
		float bullet_gravity = fabs(gravity);
		Vector3 hit_pos = aim_point;

		// Calculate zeroing adjustment
		float zero_angle = 0.0f;
		if (zero_entry.m_ZeroDistance != -1.0f && bullet_gravity != 0.0f)
		{
			float zero_air_time = zero_entry.m_ZeroDistance / bullet_velocity;
			float zero_drop = 0.5f * bullet_gravity * zero_air_time * zero_air_time;
			zero_angle = atan2(zero_drop, zero_entry.m_ZeroDistance);
		}

		float best_travel_time = 0.0f;

		if (bullet_gravity != 0.0f)
		{
			// Coefficients for the quartic equation
			const double a = 0.25 * bullet_gravity * bullet_gravity;
			const double b = enemy_velocity.y * bullet_gravity;
			const double c = (relative_pos.y * bullet_gravity) + enemy_velocity.Dot(enemy_velocity) - (bullet_velocity * bullet_velocity);
			const double d = 2.0 * relative_pos.Dot(enemy_velocity);
			const double e = relative_pos.Dot(relative_pos);

			// Solve the quartic equation to find time of impact
			const auto& roots = solve_quartic(b / a, c / a, d / a, e / a);

			float min_prediction_error = FLT_MAX;

			// Improved root selection with error minimization
			for (int i = 0; i < 4; ++i)
			{
				// Only consider real, positive roots
				if (roots[i].imag() == 0.0 && roots[i].real() > 0.01)
				{
					float current_time = static_cast<float>(roots[i].real());

					// Calculate predicted hit position
					Vector3 predicted_hit_pos;
					predicted_hit_pos.x = shoot_space.x + ((relative_pos.x / current_time) + enemy_velocity.x) * current_time;
					predicted_hit_pos.y = shoot_space.y + ((relative_pos.y / current_time) + enemy_velocity.y - (0.5f * bullet_gravity * current_time)) * current_time;
					predicted_hit_pos.z = shoot_space.z + ((relative_pos.z / current_time) + enemy_velocity.z) * current_time;

					// Calculate prediction error (distance from original aim point)
					float prediction_error = (predicted_hit_pos - aim_point).Length();

					// Select root with minimal prediction error
					if (prediction_error < min_prediction_error)
					{
						min_prediction_error = prediction_error;
						best_travel_time = current_time;
						hit_pos = predicted_hit_pos;
					}
				}
			}

			// Check if we found a valid solution
			if (best_travel_time <= 0.0f)
				return false;
		}
		else
		{
			// No gravity case - improved quadratic solution
			const double a = (enemy_velocity.Dot(enemy_velocity)) - (bullet_velocity * bullet_velocity);
			const double b = 2.0 * (relative_pos.Dot(enemy_velocity));
			const double c = relative_pos.Dot(relative_pos);

			// Robust handling of near-zero coefficients
			const double eps = 1e-6;
			if (std::abs(a) < eps)
			{
				// Linear approximation for very low relative velocity
				if (std::abs(b) > eps)
				{
					best_travel_time = -c / b;
					if (best_travel_time > 0.01f)
					{
						hit_pos.x = shoot_space.x + enemy_velocity.x * best_travel_time;
						hit_pos.y = shoot_space.y + enemy_velocity.y * best_travel_time;
						hit_pos.z = shoot_space.z + enemy_velocity.z * best_travel_time;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
			{
				// Calculate discriminant with improved stability
				double discriminant = b * b - (4 * a * c);

				// Robust root finding
				if (discriminant >= -eps) // Allow for small numerical errors
				{
					// Handle near-zero discriminant case
					if (std::abs(discriminant) < eps)
						discriminant = 0.0;

					// Take square root safely
					double sqrt_disc = std::sqrt(max(0.0, discriminant));

					// Calculate roots
					double t1 = (-b - sqrt_disc) / (2 * a);
					double t2 = (-b + sqrt_disc) / (2 * a);

					// Select valid travel time
					if (t1 > 0.01f && t2 > 0.01f)
						best_travel_time = static_cast<float>(min(t1, t2));
					else if (t1 > 0.01f)
						best_travel_time = static_cast<float>(t1);
					else if (t2 > 0.01f)
						best_travel_time = static_cast<float>(t2);
					else
						return false;

					// Calculate hit position
					hit_pos.x = shoot_space.x + ((relative_pos.x / best_travel_time) + enemy_velocity.x) * best_travel_time;
					hit_pos.y = shoot_space.y + ((relative_pos.y / best_travel_time) + enemy_velocity.y) * best_travel_time;
					hit_pos.z = shoot_space.z + ((relative_pos.z / best_travel_time) + enemy_velocity.z) * best_travel_time;
				}
				else
					return false; // No real solutions
			}
		}

		// Apply zeroing compensation if needed
		if (zero_angle != 0.0f)
			hit_pos.y += sin(zero_angle) * distance;

		// Update the aim point with our predicted hit position
		aim_point = hit_pos;

		return true;
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
			if (!IsValidPtr(player))
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
		if (!IsValidPtr(local_player)) return;

		float fov_radius = get_fov_radius(g_settings.aim_fov, (float)g_globals.g_width, (float)g_globals.g_height);

		for (const auto player : m_PlayerList)
		{
			if (!IsValidPtr(player))
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			const auto soldier = player->GetSoldier();
			if (!IsValidPtr(soldier))
				continue;

			if (!soldier->IsAlive())
				continue;

			// Don't aim at passengers (velocity is always 0 here)
			if (player->m_AttachedEntryId == 2)
				continue;

			Vector3 head_vec;
			bool got_bone = false;

			// Check if player is in vehicle
			const auto vehicle = player->GetVehicle();
			if (IsValidPtr(vehicle))
			{
				// Get player bones even when in vehicle
				const auto ragdoll = soldier->m_pRagdollComponent;
				if (IsValidPtr(ragdoll))
				{
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
						got_bone = ragdoll->GetBone((UpdatePoseResultData::BONES)g_settings.aim_bone, head_vec);
				}

				// If we failed to get the bone but the vehicle is valid, target a strategic point based on vehicle type
				if (!got_bone)
				{
					TransformAABBStruct transform = get_transform(vehicle);
					Vector3 center = transform.Transform.Translation() + (transform.AABB.m_Min + transform.AABB.m_Max) * 0.5f;

					// Convert Vector4 to Vector3 for size calculation
					Vector3 min = Vector3(transform.AABB.m_Min.x, transform.AABB.m_Min.y, transform.AABB.m_Min.z);
					Vector3 max = Vector3(transform.AABB.m_Max.x, transform.AABB.m_Max.y, transform.AABB.m_Max.z);
					Vector3 size = max - min;

					// Get vehicle type
					VehicleData::VehicleType type = VehicleData::VehicleType::VEHICLE; // Default
					const auto data = vehicle->m_Data;
					if (IsValidPtr(data))
						type = data->GetVehicleType();

					// Offset from center based on vehicle type
					Vector3 offset(0, 0, 0);

					switch (type)
					{
					case VehicleData::VehicleType::TANK:
					case VehicleData::VehicleType::TANKIFV:
					case VehicleData::VehicleType::TANKARTY:
					case VehicleData::VehicleType::TANKAA:
					case VehicleData::VehicleType::TANKAT:
					case VehicleData::VehicleType::TANKLC:
						// For tanks, aim for the turret/upper area
						offset = Vector3(0, size.y * 0.3f, 0);
						break;

					case VehicleData::VehicleType::HELIATTACK:
					case VehicleData::VehicleType::HELISCOUT:
					case VehicleData::VehicleType::HELITRANS:
						// For helicopters, aim for the cockpit (front upper)
						offset = Vector3(size.x * 0.3f, size.y * 0.2f, 0);
						break;

					case VehicleData::VehicleType::JET:
					case VehicleData::VehicleType::JETBOMBER:
						// For jets, aim for the cockpit or engine
						offset = Vector3(size.x * 0.2f, size.y * 0.1f, 0);
						break;

					case VehicleData::VehicleType::BOAT:
						// For boats, aim for the bridge/control area
						offset = Vector3(size.x * 0.2f, size.y * 0.3f, 0);
						break;

					case VehicleData::VehicleType::CAR:
					case VehicleData::VehicleType::JEEP:
						// For land vehicles, aim for the driver area (front upper)
						offset = Vector3(size.x * 0.25f, size.y * 0.2f, 0);
						break;

					case VehicleData::VehicleType::STATIONARY:
					case VehicleData::VehicleType::STATICAT:
					case VehicleData::VehicleType::STATICAA:
						// For stationary weapons, aim for the gunner position
						offset = Vector3(0, size.y * 0.4f, 0);
						break;

					default:
						// Default aim point (slightly above center for most vehicles)
						offset = Vector3(0, size.y * 0.1f, 0);
						break;
					}

					Matrix rotation = transform.Transform;
					rotation.Translation(Vector3(0, 0, 0)); // Clear translation component to keep only rotation

					// Apply rotation to offset
					Vector3 oriented_offset;
					oriented_offset.x = offset.x * rotation._11 + offset.y * rotation._21 + offset.z * rotation._31;
					oriented_offset.y = offset.x * rotation._12 + offset.y * rotation._22 + offset.z * rotation._32;
					oriented_offset.z = offset.x * rotation._13 + offset.y * rotation._23 + offset.z * rotation._33;

					// Set the aim point
					head_vec = center + oriented_offset;
					got_bone = true;
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
					got_bone = ragdoll->GetBone((UpdatePoseResultData::BONES)g_settings.aim_bone, head_vec);
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
				if (!IsValidPtr(local_soldier))
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

using namespace big;

namespace plugins
{
	void vehicle_aimbot(AimbotTarget& target)
	{
		const auto border_input_node = BorderInputNode::GetInstance();
		if (!border_input_node) return;

		const auto mouse = border_input_node->m_pMouse;
		if (!mouse) return;

		const auto device = mouse->m_pDevice;
		if (!device) return;

		Vector2 camera_center;
		if (auto vehicle_turret = VehicleTurret::GetInstance(); IsValidPtr(vehicle_turret))
			world_to_screen(vehicle_turret->GetVehicleCrosshair(), camera_center);
		else
		{
			Vector2 screen_size = get_screen_size();
			camera_center = { screen_size.x * .5f, screen_size.y * .5f };
		}

		Vector2 predicted;
		world_to_screen(target.m_WorldPosition, predicted);

		auto delta = get_abs_delta_at_given_points(camera_center, predicted);
		Vector2 delta_vec = camera_center - predicted;

		// Calculate target movement
		float target_x = -delta_vec.x / g_settings.aim_vehicle_smooth;
		float target_y = -delta_vec.y / g_settings.aim_vehicle_smooth;

		// Keep track of current movement (static to persist between function calls)
		static float current_x = 0.0f;
		static float current_y = 0.0f;

		// Lerp factor controls how quickly to move toward the target (0.0-1.0)
		float lerp_factor = 0.35f;

		// Linear interpolation between current and target values
		current_x = current_x + (target_x - current_x) * lerp_factor;
		current_y = current_y + (target_y - current_y) * lerp_factor;

		// Apply the lerped values to the mouse buffer
		device->m_Buffer.x = current_x;
		device->m_Buffer.y = current_y;
	}

	void aimbot(float delta_time)
	{
		// Controller support
		bool using_controller = g_settings.aim_support_controller && is_left_trigger_pressed(0.5f);

		// Pressed status
		auto is_pressed = [=]()
			{
				return GetAsyncKeyState(g_settings.aim_key) != 0 || using_controller;
			};

		if (!is_pressed())
			return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		// Check if player is in vehicle
		const auto local_vehicle = local_player->GetVehicle();

		// Handle vehicle aimbot
		if (IsValidPtr(local_vehicle))
		{
			// This aimbot wasn't designed for jets or helicopters
			if (IsValidPtr(local_vehicle->m_Data))
			{
				if (local_vehicle->m_Data->IsInHeli() || local_vehicle->m_Data->IsInJet())
					return;
			}

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
				prediction_target = target.m_Player->GetVehicle();
			else
				prediction_target = target.m_Player->GetSoldier();

			// Doing predict
			auto prediction = m_AimbotPredictor.PredictTarget(
				local_soldier,
				prediction_target,
				target.m_WorldPosition,
				Matrix() // Empty matrix since vehicle doesn't use shoot space
			);

			if (!prediction.success)
				return;

			// Update
			target.m_WorldPosition = prediction.predicted_position;

			// Update global prediction point
			g_globals.g_pred_aim_point = target.m_WorldPosition;
			g_globals.g_has_pred_aim_point = target.m_HasTarget;

			// Call vehicle aimbot
			vehicle_aimbot(target);
			return;
		}

		const auto weapon_component = local_soldier->m_pWeaponComponent;
		if (!IsValidPtr(weapon_component)) return;

		const auto weapon = weapon_component->GetActiveSoldierWeapon();
		if (!IsValidPtr(weapon)) return;

		const auto client_weapon = weapon->m_pWeapon;
		if (!IsValidPtr(client_weapon)) return;

		const auto primary_fire = weapon->m_pPrimary;
		if (!IsValidPtr(primary_fire) || (uintptr_t)primary_fire == 0x10F00000030) return;

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
		if (!IsValidPtr(aiming_simulation)) return;

		const auto aim_assist = aiming_simulation->m_pFPSAimer;
		if (!IsValidPtr(aim_assist)) return;

		Matrix shoot_space = client_weapon->m_ShootSpace;

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
			prediction_target = target.m_Player->GetVehicle();
		else
			prediction_target = target.m_Player->GetSoldier();

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

		Vector3 v_dir = target.m_WorldPosition - shoot_space.Translation();
		v_dir.Normalize();

		float horizontal_angle = -atan2(v_dir.x, v_dir.z);
		float vertical_angle = atan2(v_dir.y, sqrt(v_dir.x * v_dir.x + v_dir.z * v_dir.z));

		Vector2 aim_angles =
		{
			horizontal_angle,
			vertical_angle
		};

		aim_angles -= aiming_simulation->m_Sway;
		m_AimbotSmoother.SmoothAngles(aim_assist->m_AimAngles, aim_angles);
		aim_assist->m_AimAngles = aim_angles;
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
		if (!IsValidPtr(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		// FOV
		float fov_radius = get_fov_radius(g_settings.aim_fov, (float)g_globals.g_width, (float)g_globals.g_height);

		if (g_settings.aim_fov_method && g_settings.aim_draw_fov)
			m_drawing->AddCircle(ImVec2(g_globals.g_width / 2.f, g_globals.g_height / 2.f), fov_radius, g_settings.aim_fov_color);
	}
}