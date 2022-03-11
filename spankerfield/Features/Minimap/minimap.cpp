#include "minimap.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../SDK/sdk.h"
#include "../../Utilities/xorstr.h"
#include "../../Utilities/other.h"

namespace features
{
	void spot_minimap(bool FFPB)
	{
		if (!settings::minimap) return;

		if (settings::obs_check)
		{
			if (GetTickCount64() - globals::g_Last_OBS_Check > 5000)
			{
				globals::g_OBS = utils::IsProcessRunning(L"obs64.exe");
				globals::g_Last_OBS_Check = GetTickCount64();
			}

			if (globals::g_OBS) return;
		}

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			ClientVehicleEntity* vehicle = player->GetVehicle();
			if (IsValidPtr(vehicle))
			{
				vehicle->m_pComponents->GetComponentByClassId<ClientSpottingTargetComponent>(378)->activeSpotType = FFPB ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
			else
			{
				ClientSoldierEntity* soldier = player->GetSoldier();
				if (!soldier) continue;

				if (!soldier->IsAlive())
					continue;

				soldier->m_pComponents->GetComponentByClassId<ClientSpottingTargetComponent>(378)->activeSpotType = FFPB ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
		}
	}
}