#include "minimap.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../SDK/sdk.h"
#include "../../Utilities/xorstr.h"
#include "../../Utilities/other.h"

using namespace big;
namespace plugins
{
	void spot_minimap()
	{
		if (!g_settings.minimap) return;

		if (g_settings.obs_check)
		{
			if (GetTickCount64() - g_globals.g_obscheck > 5000)
			{
#ifdef _WIN64
g_globals.g_obs = is_process_running(L"obs64.exe");
#else
g_globals.g_obs = is_process_running(L"obs32.exe");
#endif

				g_globals.g_obscheck = GetTickCount64();
			}

			if (g_globals.g_obs) return;
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
				vehicle->m_pComponents->GetComponentByClassId<ClientSpottingTargetComponent>(378)->activeSpotType = !g_globals.g_punkbuster && !g_globals.g_fairfight ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
			else
			{
				ClientSoldierEntity* soldier = player->GetSoldier();
				if (!soldier) continue;

				if (!soldier->IsAlive())
					continue;

				soldier->m_pComponents->GetComponentByClassId<ClientSpottingTargetComponent>(378)->activeSpotType = !g_globals.g_punkbuster && !g_globals.g_fairfight ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
		}
	}
}