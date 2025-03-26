#pragma once
#include "../../SDK/sdk.h"

namespace big
{
	// Predictor
	class AimbotPredictor
	{
	public:
		struct PredictionResult
		{
			Vector3 predicted_position;
			bool success;
			
			PredictionResult() : success(false) {}
		};

		PredictionResult PredictTarget(ClientSoldierEntity* local_entity, 
									 ClientControllableEntity* enemy, 
									 const Vector3& aim_point, 
									 const Matrix& shoot_space
		);

	private:
		struct AngularPredictionData_s
		{
			Quaternion orientation;
			Vector3 angular_velocity;
			bool valid = false;
		};

		void PredictLinearMove(const Vector3& linearVelocity, const double predictionTime, const Vector3& curPosition, Vector3* out);
		void PredictRotation(const Vector3& angularVelocity, const Quaternion& orientation, const double predictionTime, Quaternion* out);
		void PredictFinalRotation(const Vector3& linearVel, const Vector3& angularVel, const double predTime, const Quaternion& orientation, const Vector3& curPosition, Quaternion* predOrientationOut, Vector3* predLinearVelOut);

		bool DoPrediction(const Vector3& shoot_space,
						  Vector3& aim_point, 
						  const Vector3& my_velocity, 
						  const Vector3& enemy_velocity, 
						  const Vector3& bullet_speed, 
						  const float gravity,
						  const WeaponZeroingEntry& zero_entry,
			              const AngularPredictionData_s* angular_data);
	};

	// Smoother
	class AimbotSmoother
	{
	public:
		AimbotSmoother();
		void Update(float DeltaTime);
		void ResetTimes(Vector2& Times);
		void SmoothAngles(Vector2& CurrentAngles, Vector2& TargetAngles);

		Vector2 m_TimeToTarget;
		Vector2 m_CurAimTime;
	};

	// Target struct
	struct AimbotTarget
	{
		ClientPlayer* m_Player;
		Vector3 m_WorldPosition;
		bool m_HasTarget;
		AimbotTarget()
		{
			m_Player = nullptr;
			m_HasTarget = false;
		}
	};

	// Manager
	class BotPlayerManager
	{
	public:
		BotPlayerManager();
		void update_players();
		std::vector<ClientPlayer*>& get_player_list();
		AimbotTarget& get_closest_crosshair_player();
	protected:
		void update_all_player_list();
		void update_closest_crosshair_list();
		float get_screen_distance(Vector3& EnemyScreen, Vector2 ScreenSize);
	private:
		std::vector<ClientPlayer*> m_PlayerList;
		AimbotTarget m_ClosestPlayer;
	};

	// Declarations
	inline AimbotPredictor m_AimbotPredictor;
	inline AimbotSmoother m_AimbotSmoother;
	inline struct AimbotTarget m_PreviousTarget;
	inline BotPlayerManager m_PlayerManager;
}

namespace plugins
{
	void aimbot(float delta_time);
	void draw_fov();
}