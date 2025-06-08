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
#include "../../Features/Missiles/missiles.h"
#include "../../SDK/class_info.h"

#pragma warning( disable : 4244 )

// For some reason, prediction math on close targets that are exactly <=6.57f distance far than you works really badly
// This float was manually tuned
#define CLOSE_TARGET_DIST_FLOAT 6.57f

namespace big
{
	void AimbotPredictor::PredictLinearMove(const Vector3& linearVelocity, const double predictionTime, const Vector3& curPosition, Vector3* out)
	{
		*out = curPosition + (linearVelocity * predictionTime);
	}

	void AimbotPredictor::PredictRotation(const Vector3& angularVelocity, const Quaternion& orientation, const double predictionTime, Quaternion* out)
	{
		float angle = angularVelocity.Length() * static_cast<float>(predictionTime);
		Vector3 rotation_axis = angularVelocity / angularVelocity.Length();
		Quaternion rotation_from_angular_velocity = Quaternion::CreateFromAxisAngle(rotation_axis, angle);

		*out = orientation * rotation_from_angular_velocity;
	}

	void AimbotPredictor::PredictFinalRotation(const Vector3& linearVel, const Vector3& angularVel, const double predTime, const Quaternion& orientation, const Vector3& curPosition, Quaternion* predOrientationOut, Vector3* predLinearVelOut)
	{
		auto translation = [](const Quaternion& quat, const Vector3& vec) -> Vector3
			{
				float num = quat.x * 2.f;
				float num2 = quat.y * 2.f;
				float num3 = quat.z * 2.f;
				float num4 = quat.x * num;
				float num5 = quat.y * num2;
				float num6 = quat.z * num3;
				float num7 = quat.x * num2;
				float num8 = quat.x * num3;
				float num9 = quat.y * num3;
				float num10 = quat.w * num;
				float num11 = quat.w * num2;
				float num12 = quat.w * num3;

				Vector3 result;
				result.x = (1.f - (num5 + num6)) * vec.x + (num7 - num12) * vec.y + (num8 + num11) * vec.z;
				result.y = (num7 + num12) * vec.x + (1.f - (num4 + num6)) * vec.y + (num9 - num10) * vec.z;
				result.z = (num8 - num11) * vec.x + (num9 + num10) * vec.y + (1.f - (num4 + num5)) * vec.z;

				return result;
			};

		Quaternion pred_orientation = orientation;
		Vector3 pred_lin_vel = linearVel;
		Vector3 pred_displacement = curPosition;

		// Compile time variables
		constexpr int SIMULATION_STEPS = 16;
		constexpr float TIME_MULTIPLIER = 0.69f;

		float delta_time = (predTime * TIME_MULTIPLIER / 100.0f) / SIMULATION_STEPS;
		for (int i = 0; i < 16; ++i)
		{
			PredictLinearMove(pred_lin_vel, delta_time, pred_displacement, &pred_displacement);
			PredictRotation(angularVel, pred_orientation, delta_time, &pred_orientation);

			pred_lin_vel = translation(pred_orientation, Vector3(0.0f, 0.0f, pred_lin_vel.Length()));
		}

		*predLinearVelOut = pred_lin_vel;
		*predOrientationOut = pred_orientation;
	}

	float AimbotPredictor::ComputeMissileFinalVelocity(float initSpd, float maxSpd, float accel, float engIgnTime, float dist, float* travelTime)
	{
		if ((accel == 0.0f) || (maxSpd < initSpd)) return 0.0f;

		auto before_ignition_distance = initSpd * engIgnTime;
		auto acceleration_time = (maxSpd - initSpd) / accel;
		auto acceleration_dist = (initSpd * acceleration_time) + (accel * acceleration_time * acceleration_time) / 2.f;
		auto postAccelerationDist = dist - before_ignition_distance - acceleration_dist;
		auto post_acceleration_time = postAccelerationDist / maxSpd;
		auto final_air_time = engIgnTime + acceleration_time + post_acceleration_time;
		auto final_distance = before_ignition_distance + acceleration_dist + postAccelerationDist;

		if (final_air_time <= 0.0f) return 0.0f;

		if (travelTime) *travelTime = final_air_time;

		auto velocity = final_distance / final_air_time;
		velocity = std::clamp(velocity, initSpd, maxSpd);

		return velocity;
	}

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
		Vector3 initial_speed;
		Vector3 my_velocity;
		WeaponZeroingEntry zero_entry(-1.0f, -1.0f);
		AngularPredictionData_s angular_data;

		// Get weapon data (either from vehicle or infantry)
		if (is_in_vehicle)
		{
			auto vehicle_turret = VehicleTurret::GetInstance();
			if (!IsValidPtr(vehicle_turret))
				return result;

			fire_position = vehicle_turret->GetVehicleCameraOrigin();
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

		// Get firing data
		const auto weapon_firing = get_weapon_firing();
		if (!IsValidPtr(weapon_firing))
			return result;

		// Hyperhook had issues with crashes when entering the vehicle because the firing data was invalid muddafuka
		// Fixed it by delaying the firing data for 150ms
		const auto firing_data = WeaponFiringDataRetriever::GetSafeFiringData(weapon_firing, is_in_vehicle);
		if (!IsValidPtr(firing_data))
			return result;

		// Projectile data
		const auto bullet = firing_data->m_ShotConfigData.m_ProjectileData;
		if (!IsValidPtr(bullet))
			return result;

		// Gravity
		float gravity = bullet->m_Gravity;

		if (gravity < -500.f || gravity > 500.f) gravity = 0.f;

		// Initial speed (z component is our bullet velocity)
		Vector4 initial_speed_4 = firing_data->m_ShotConfigData.m_Speed;
		initial_speed = Vector3(initial_speed_4.x, initial_speed_4.y, initial_speed_4.z);

		// Get enemy velocity
		Vector3 enemy_velocity = *enemy->GetVelocity();

		if (IsValidPtrWithVTable(enemy->m_Data))
		{
			// We check if it's actually a vehicle by checking the number of seats available
			if (enemy->m_Data->m_MaxPlayersInVehicle > 0)
			{
				const auto vehicle = reinterpret_cast<ClientVehicleEntity*>(enemy);
				enemy_velocity = vehicle->m_VelocityVec;

				if (IsValidPtr(vehicle->GetChassisComponent()))
				{
					Matrix model_matrix;
					vehicle->GetTransform(&model_matrix);

					angular_data.orientation = matrix_to_quaternion(model_matrix);
					angular_data.angular_velocity = vehicle->GetChassisComponent()->m_AngularVelocity;
					angular_data.valid = true;
				}
			}
		}

		// Support for AT Launcher velocity and gravity
		if (!is_in_vehicle)
		{
			// Check for explosion hit type
			if (bullet->m_HitReactionWeaponType == ProjectileEntityData::AntHitReactionWeaponType_Explosion)
			{
				// Check the weapon type for AT (anti-tank)
				const auto weapon_type = weapon_firing->GetWeaponClass();
				if (weapon_type == WeaponClass::At)
				{
					// Travel time of the rocket
					float travel_time = 0.0f;

					// Prediction code itself
					const auto missile_data = reinterpret_cast<MissileEntityData*>(bullet);
					if (IsValidPtr(missile_data))
					{
						const auto is_laser_guided = missile_data->IsLaserGuided();
						const auto& ignition_time = missile_data->m_EngineTimeToIgnition;
						const auto& acceleration = missile_data->m_EngineStrength;
						const auto& max_speed = missile_data->m_MaxSpeed;

						// Local missile
						VeniceClientMissileEntity* my_missile = plugins::get_local_missile();

						// Start position
						auto start_position = !is_laser_guided ? fire_position : IsValidPtr(my_missile) ? my_missile->m_Position : fire_position;
						float distance = (start_position - aim_point).Length();

						// Calculating final velocity first time to solve 't' at given 'dst' to target
						initial_speed.z = ComputeMissileFinalVelocity(initial_speed.z, max_speed, acceleration, ignition_time, distance, &travel_time);

						// Calculate where target will be after 't' seconds
						const auto position_after_time = (aim_point - fire_position) + (enemy_velocity * travel_time);
						distance = (position_after_time - aim_point).Length();

						// Calculate final velocity again for new target position 
						initial_speed.z = ComputeMissileFinalVelocity(initial_speed.z, max_speed, acceleration, ignition_time, distance, &travel_time);
						gravity = missile_data->m_Gravity > 0.f ? 0.0f : missile_data->m_Gravity;
					}
				}
			}
		}

		// Force update
		*(BYTE*)((uintptr_t)enemy + 0x1A) = 159;

		// Calculate prediction
		bool prediction_success = DoPrediction(
			fire_position,
			result.predicted_position,
			my_velocity,
			enemy_velocity,
			initial_speed,
			gravity,
			zero_entry,
			&angular_data);

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
		const WeaponZeroingEntry& zero_entry,
		const AngularPredictionData_s* angular_data)
	{
		Vector3 relative_pos = aim_point - shoot_space;
		float distance = relative_pos.Length();

		// Skip complex prediction for close targets
		if (distance <= CLOSE_TARGET_DIST_FLOAT)
			return false;

		// Use the bullet speed magnitude for calculations
		float bullet_velocity = bullet_speed.z;
		float bullet_gravity = gravity;
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

					// Initialize target velocity as original enemy velocity
					Vector3 target_vel = enemy_velocity;

					if (angular_data && angular_data->valid)
					{
						PredictFinalRotation(
							enemy_velocity,
							angular_data->angular_velocity,
							current_time,
							angular_data->orientation,
							aim_point,
							&g_globals.g_predicted_orientation,
							&target_vel
						);
					}

					// Calculate predicted hit position
					Vector3 predicted_hit_pos;
					predicted_hit_pos.x = shoot_space.x + ((relative_pos.x / current_time) + target_vel.x) * current_time;
					predicted_hit_pos.y = shoot_space.y + ((relative_pos.y / current_time) + target_vel.y - (0.5f * bullet_gravity * current_time)) * current_time;
					predicted_hit_pos.z = shoot_space.z + ((relative_pos.z / current_time) + target_vel.z) * current_time;

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
						Vector3 target_vel = enemy_velocity;

						if (angular_data && angular_data->valid)
						{
							PredictFinalRotation(
								enemy_velocity,
								angular_data->angular_velocity,
								best_travel_time,
								angular_data->orientation,
								aim_point,
								&g_globals.g_predicted_orientation,
								&target_vel
							);
						}

						hit_pos.x = shoot_space.x + target_vel.x * best_travel_time;
						hit_pos.y = shoot_space.y + target_vel.y * best_travel_time;
						hit_pos.z = shoot_space.z + target_vel.z * best_travel_time;
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

					// Just a new variable
					Vector3 target_vel = enemy_velocity;

					if (angular_data && angular_data->valid)
					{
						PredictFinalRotation(
							enemy_velocity,
							angular_data->angular_velocity,
							best_travel_time,
							angular_data->orientation,
							aim_point,
							&g_globals.g_predicted_orientation,
							&target_vel
						);
					}

					// Calculate hit position
					hit_pos.x = shoot_space.x + ((relative_pos.x / best_travel_time) + target_vel.x) * best_travel_time;
					hit_pos.y = shoot_space.y + ((relative_pos.y / best_travel_time) + target_vel.y) * best_travel_time;
					hit_pos.z = shoot_space.z + ((relative_pos.z / best_travel_time) + target_vel.z) * best_travel_time;
				}
				else
					return false; // No real solutions
			}
		}

		// Apply zeroing compensation if needed
		if (zero_angle != 0.0f)
			hit_pos.y += sinf(zero_angle) * distance;

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

	// Helper function to get bone position from ragdoll component
	bool BotPlayerManager::get_bone_position(RagdollComponent* ragdoll, Vector3& position)
	{
		if (!IsValidPtr(ragdoll))
			return false;

		// Try to get bone based on auto or manual selection
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

			// Try each bone in priority order
			for (const auto& bone : bone_priority)
			{
				if (ragdoll->GetBone(bone, position))
					return true;
			}

			return false;
		}
		else
		{
			// Use manually selected bone
			return ragdoll->GetBone((UpdatePoseResultData::BONES)g_settings.aim_bone, position);
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

			// Main targeting logic
			Vector3 head_vec;
			bool got_bone = false;

			// Get player ragdoll component
			const auto ragdoll = soldier->m_pRagdollComponent;

			// Check if player is in vehicle
			const auto vehicle = player->GetVehicle();
			const bool in_vehicle = IsValidPtr(vehicle);

			// Visibility check for infantry
			if (!in_vehicle && g_settings.aim_must_be_visible && soldier->m_Occluded)
				continue;

			// Visibility check for vehicles
			if (in_vehicle && g_settings.aim_must_be_visible)
			{
				TransformAABBStruct vehicle_transform = get_transform(vehicle);
				if (!is_entity_visible(vehicle_transform, local_player))
					continue;
			}

			// Try to get bone position
			got_bone = get_bone_position(ragdoll, head_vec);

			// If in vehicle and bone targeting failed, use vehicle center
			if (in_vehicle && !got_bone)
			{
				TransformAABBStruct transform = get_transform(vehicle);
				head_vec = transform.Transform.Translation() + (transform.AABB.m_Min + transform.AABB.m_Max) * 0.5f;
				got_bone = true;
			}

			// Skip if we couldn't get a target position
			if (!got_bone)
				continue;

			// Convert world position to screen position
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
	void mouse_aim(AimbotTarget& target)
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

		float target_x = -delta_vec.x / g_settings.aim_vehicle_smooth;
		float target_y = -delta_vec.y / g_settings.aim_vehicle_smooth;

		device->m_Buffer.x = target_x;
		device->m_Buffer.y = target_y;
	}

	void perform_prediction_and_aim(
		ClientSoldierEntity* local_soldier,
		ClientControllableEntity* prediction_target,
		AimbotTarget& target,
		const Matrix& space_matrix,
		float delta_time,
		bool perform_aim,
		SoldierAimingSimulation* aiming_simulation = nullptr,
		AimAssist* aim_assist = nullptr)
	{
		// Perform prediction
		auto prediction = m_AimbotPredictor.PredictTarget(
			local_soldier,
			prediction_target,
			target.m_WorldPosition,
			space_matrix
		);

		if (!prediction.success)
			return;

		// Update target position with prediction
		target.m_WorldPosition = prediction.predicted_position;

		// Update global prediction point
		g_globals.g_pred_aim_point = target.m_WorldPosition;
		g_globals.g_has_pred_aim_point = target.m_HasTarget;

		// Skip aiming part if not requested
		if (!perform_aim || !g_settings.aimbot_snap_to_target)
			return;

		// Handle vehicle aiming
		if (!aim_assist)
		{
			mouse_aim(target);
			return;
		}

		// Handle infantry aiming with standard weapons
		if (g_settings.aim_max_time_to_target <= 0.f)
			return;

		// Reset smoothing if target changed
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

		// Calculate aim angles
		Vector3 v_dir = target.m_WorldPosition - space_matrix.Translation();
		v_dir.Normalize();

		float horizontal_angle = -atan2(v_dir.x, v_dir.z);
		float vertical_angle = atan2(v_dir.y, sqrt(v_dir.x * v_dir.x + v_dir.z * v_dir.z));

		Vector2 aim_angles =
		{
			horizontal_angle,
			vertical_angle
		};

		// Apply final aiming
		aim_angles -= aiming_simulation->m_Sway;
		m_AimbotSmoother.SmoothAngles(aim_assist->m_AimAngles, aim_angles);
		aim_assist->m_AimAngles = aim_angles;
		m_PreviousTarget = target;
	}

	void aimbot(float delta_time)
	{
		// Check if aim key is pressed
		bool using_controller = g_settings.aim_support_controller && is_left_trigger_pressed(0.5f);
		bool is_aim_pressed = GetAsyncKeyState(g_settings.aim_key) != 0 || using_controller;

		if (!is_aim_pressed)
			return;

		// Get essential game objects
		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier) || !local_soldier->IsAlive()) return;

		// Check if player is in vehicle
		const auto local_vehicle = local_player->GetVehicle();
		bool in_vehicle = IsValidPtr(local_vehicle);

		// Early termination conditions
		if (in_vehicle && !g_settings.aimbot_vehicle)
			return;

		if (!in_vehicle && !g_settings.aimbot)
			return;

		// Skip for jets or helicopters
		if (in_vehicle && IsValidPtrWithVTable(local_vehicle->m_Data))
		{
			if (local_vehicle->m_Data->IsInHeli() || local_vehicle->m_Data->IsInJet())
				return;
		}

		// Find target
		AimbotTarget target = m_PlayerManager.get_closest_crosshair_player();
		if (!IsValidPtr(target.m_Player) || !target.m_HasTarget)
			return;

		// Friends list support
		if (g_settings.aim_ignore_friends)
		{
			uint64_t persona_id = target.m_Player->m_onlineId.m_personaid;
			if (plugins::is_friend(persona_id))
				return;
		}

		// Get correct entity for prediction
		ClientControllableEntity* prediction_target = nullptr;
		if (IsValidPtr(target.m_Player->GetVehicle()))
			prediction_target = target.m_Player->GetVehicle();
		else
			prediction_target = target.m_Player->GetSoldier();

		// Controller support for infantry
		if (!in_vehicle && using_controller)
		{
			// Simulate a little bit of mouse movement
			const auto mouse_device = BorderInputNode::GetInstance()->m_pMouse->m_pDevice;
			if (IsValidPtr(mouse_device))
				mouse_device->m_Buffer.x = mouse_device->m_Buffer.x - 1;
		}

		// Handle prediction and aiming based on context (vehicle or infantry)
		if (in_vehicle)
		{
			// Vehicle aiming
			Matrix empty; // Empty matrix since vehicle doesn't use shoot space
			perform_prediction_and_aim(local_soldier, prediction_target, target, empty, delta_time, true);
		}
		else
		{
			// Infantry aiming
			const auto weapon_component = local_soldier->m_pWeaponComponent;
			if (!IsValidPtr(weapon_component)) return;

			const auto weapon = weapon_component->GetActiveSoldierWeapon();

			// Non-standard weapons (like rocket launchers)
			if (!IsValidPtr(weapon))
			{
				if (g_settings.aimbot_non_standart)
				{
					const auto game_renderer = GameRenderer::GetInstance();
					if (!game_renderer) return;

					const auto render_view = game_renderer->m_pRenderView;
					if (!render_view) return;

					// Only perform prediction, no aiming for non-standard weapons
					perform_prediction_and_aim(local_soldier, prediction_target, target, render_view->m_ViewInverse, delta_time, false);
				}

				return;
			}

			// Standard weapons
			const auto client_weapon = weapon->m_pWeapon;
			if (!IsValidPtr(client_weapon)) return;

			const auto primary_fire = weapon->m_pPrimary;
			if (!IsValidPtr(primary_fire) || (uintptr_t)primary_fire == 0x10F00000030) return;

			// Don't aim while reloading if setting enabled
			if (g_settings.aim_must_not_reload && primary_fire->m_ReloadTimer >= 0.01f)
				return;

			const auto aiming_simulation = weapon->m_pAuthoritativeAiming;
			if (!IsValidPtr(aiming_simulation)) return;

			const auto aim_assist = aiming_simulation->m_pFPSAimer;
			if (!IsValidPtr(aim_assist)) return;

			// Perform prediction and aiming with standard weapon
			Matrix shoot_space = client_weapon->m_ShootSpace;
			perform_prediction_and_aim(local_soldier, prediction_target, target, shoot_space, delta_time, true, aiming_simulation, aim_assist);
		}
	}

	void draw_fov()
	{
		if (!g_settings.aimbot && !g_settings.aimbot_vehicle) return;
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