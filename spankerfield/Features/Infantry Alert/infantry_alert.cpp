#include "infantry_alert.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/other.h"
#include "../../Utilities/math.h"
#include "../../Rendering/draw-list.h"

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
		if (!local_player) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			if (player->GetVehicle())
				continue;

			TransformAABBStruct transform = get_transform(player);
			TransformAABBStruct local_transform = get_transform(local_player);;

			Vector3 local_pos = (Vector3)local_transform.Transform.m[3];
			Vector3 pos = (Vector3)transform.Transform.m[3];

			float distance = get_distance(pos, local_pos);
			if (distance > 22.5f)
				continue;

			/*
			TODO: Aim assist angles are Vec2, figure out a way with Vec3
			Right now it's just a distance check without FOV :(
			
			Vector3 angle = calc_angle(local_pos, pos);
			Vector3 eye_angles = { aim_assist->m_AimAngles.x, aim_assist->m_AimAngles.y, 0.f };

			float fov = calc_fov(eye_angles, angle);
			if (fov < 70.0f)
				continue;
			*/

			if (!drawing)
			{
				m_drawing->AddText(g_globals.g_width / 2, (g_globals.g_height / 2) + 100.f, ImColor::Yellow(200), 20.f, FL_CENTER_X, xorstr_(u8"%s"), xorstr_("Enemy nearby"));
				drawing = true;
			}
		}

		drawing = false;
	}
}