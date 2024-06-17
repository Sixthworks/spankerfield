#include "minimap.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Utilities/other.h"
#include "../../Utilities/thread_pool.h"

using namespace big;
namespace plugins
{
	static ULONGLONG last_check = 0;
	void spot_minimap()
	{
		if (!g_settings.minimap) return;

		if (g_settings.obs_check)
		{
			if (GetTickCount64() - last_check > 5000)
			{
				g_thread_pool->push([&]
				{
				    // There is no way the executable is in any other name, you must be running 64-bit...
                    g_globals.g_obs = is_process_running(xorstr_(L"obs64.exe"));
				});

				last_check = GetTickCount64();
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
			const auto player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			const auto vehicle = player->GetVehicle();
			if (IsValidPtr(vehicle))
			{
				const auto components = vehicle->m_pComponents;
				if (!components) continue;

				const auto map = components->GetComponentByClassId<ClientSpottingTargetComponent>(378);
				if (!IsValidPtrWithVTable(map)) continue;

				map->activeSpotType = g_globals.g_should_draw ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
			else
			{
				const auto soldier = player->GetSoldier();
				if (!soldier) continue;

				if (!soldier->IsAlive()) continue;

				const auto components = soldier->m_pComponents;
				if (!components) continue;

				const auto map = components->GetComponentByClassId<ClientSpottingTargetComponent>(378);
				if (!IsValidPtrWithVTable(map)) continue;

				map->activeSpotType = g_globals.g_should_draw ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
		}
	}
}