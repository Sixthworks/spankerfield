#include "aimbot.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/math.h"
#include "../../Utilities/w2s.h"
#include "../../Utilities/poly_solver.h"
#include "../../Utilities/other.h"
#include "../../Rendering/draw-list.h"

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
		if (!IsValidPtr(weapon_firing))
			return 0;

		const auto firing = weapon_firing->m_pPrimaryFire;
		if (!IsValidPtr(firing))
			return 0;

		const auto firing_data = firing->m_FiringData;
		if (!IsValidPtr(firing_data))
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
		Vector3 relative_pos = (aim_point - shoot_space);
		Vector3 gravity_vec = Vector3(0, fabs(gravity), 0);
		auto f_approx_pos = [](Vector3& CurPos, const Vector3& Velocity, const Vector3& Accel, const float Time)->Vector3 {
			return CurPos + Velocity * Time + .5f * Accel * Time * Time;
		};

		float a = .25f * gravity_vec.Dot(gravity_vec);
		float b = enemy_velocity.Dot(gravity_vec);
		float c = relative_pos.Dot(gravity_vec) + enemy_velocity.Dot(enemy_velocity) - (bullet_speed.z * bullet_speed.z);
		float d = 2.0f * (relative_pos.Dot(enemy_velocity));
		float e = relative_pos.Dot(relative_pos);

		// Calculate time projectile is in air
		std::vector<double> solutions;
		int num_solutions = m_Solver->SolveQuartic(a, b, c, d, e, solutions);

		// Find smallest non-negative real root
		float shortest_air_time = 99999.0f;
		for (int i = 0; i < num_solutions; i++)
		{
			double air_time = solutions[i];
			if (air_time < 0)
				continue;

			if (air_time < shortest_air_time)
				shortest_air_time = (float)air_time;
		}

		// Extrapolate position on velocity, and account for bullet drop
		aim_point = f_approx_pos(aim_point, enemy_velocity, gravity_vec, shortest_air_time);

		if (zero_entry.m_ZeroDistance == -1.0f)
			return 0.0f;

		// This is still an approximation, fix later
		float zero_air_time = zero_entry.m_ZeroDistance / fabs(bullet_speed.z);
		float zero_drop = (.5f * fabs(gravity) * zero_air_time * zero_air_time);
		float theta = atan2(zero_drop, zero_entry.m_ZeroDistance);

		return theta;
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
		if (!local_player) return;

		for (const auto player : m_PlayerList)
		{
			if (!IsValidPtr(player))
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			const auto soldier = player->GetSoldier();
			if (!IsValidPtr(soldier))
				continue;

			if (soldier->m_Occluded)
				continue;

			const auto ragdoll = soldier->m_pRagdollComponent;
			if (!IsValidPtr(ragdoll))
				continue;

			Vector3 head_vec;
			if (!ragdoll->GetBone((UpdatePoseResultData::BONES)g_settings.aim_bone, head_vec))
				continue;

			Vector3 screen_vec;
			if (!world_to_screen(head_vec, screen_vec))
				continue;

			Vector2 screen_size = get_screen_size();
			if (screen_vec.x > screen_size.x || screen_vec.y > screen_size.y)
				continue;

			float screen_distance = get_screen_distance(screen_vec, get_screen_size());
			if (g_settings.aim_fov_method)
			{
				if (screen_distance > g_settings.aim_fov)
					return;
			}

			if (screen_distance < closest_distance)
			{
				closest_distance = screen_distance;
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

	float BotPlayerManager::get_screen_distance(Vector3& EnemyScreen, Vector2 ScreenSize)
	{
		ScreenSize /= 2;
		Vector2 DistanceFromCenter = Vector2(EnemyScreen.x, EnemyScreen.y) - ScreenSize;

		return DistanceFromCenter.Length();
	}
}

namespace plugins
{
	void aimbot(float delta_time)
	{
		if (!g_settings.aimbot) return;

		if (!GetAsyncKeyState(g_settings.aim_key))
			return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		if (local_player->GetVehicle()) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		const auto weapon_component = local_soldier->m_pWeaponComponent;
		if (!weapon_component) return;

		const auto weapon = weapon_component->GetActiveSoldierWeapon();
		if (!weapon) return;

		const auto client_weapon = weapon->m_pWeapon;
		if (!client_weapon) return;

		const auto aiming_simulation = weapon->m_pAuthoritativeAiming;
		if (!aiming_simulation) return;

		const auto aim_assist = aiming_simulation->m_pFPSAimer;
		if (!aim_assist) return;

		Matrix shoot_space = client_weapon->m_ShootSpace;

		AimbotTarget target = m_PlayerManager.get_closest_crosshair_player();
		if (!IsValidPtr(target.m_Player)) return;

		if (!target.m_HasTarget) return;

		Vector3 temporary_aim = target.m_WorldPosition;
		float zero_theta_offset = m_AimbotPredictor.PredictLocation(local_soldier, target.m_Player->GetSoldier(), temporary_aim, shoot_space);
		target.m_WorldPosition = temporary_aim;

		if (g_settings.aim_max_time_to_target <= 0.f) return;

		if (target.m_Player != m_PreviousTarget.m_Player)
		{
			Vector2 vec_rand = { generate_random_float(g_settings.aim_min_time_to_target, g_settings.aim_max_time_to_target), generate_random_float(g_settings.aim_min_time_to_target, g_settings.aim_max_time_to_target) };
			m_AimbotSmoother.ResetTimes(vec_rand);
		}
	
		m_AimbotSmoother.Update(delta_time);

		Vector3 vDir = target.m_WorldPosition - shoot_space.Translation();
		vDir.Normalize();

		Vector2 BotAngles = {
			-atan2(vDir.x, vDir.z),
			atan2(vDir.y, vDir.Length())
		};

		BotAngles -= aiming_simulation->m_Sway;
		BotAngles.y -= zero_theta_offset;
		m_AimbotSmoother.SmoothAngles(aim_assist->m_AimAngles, BotAngles);
		aim_assist->m_AimAngles = BotAngles;
		m_PreviousTarget = target;
	}

	void draw_fov()
	{
		if (!g_settings.aimbot) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		if (local_player->GetVehicle()) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		if (g_settings.aim_fov_method && g_settings.aim_draw_fov)
			m_drawing->AddCircle(ImVec2(g_globals.g_width / 2.f, g_globals.g_height / 2.f), g_settings.aim_fov, ImColor(255, 255, 255, 255));
	}
}