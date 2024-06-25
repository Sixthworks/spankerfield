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
			if (GetTickCount64() - last_check > 2500)
			{
				g_thread_pool->push([&]
				{
				    // There is no way the executable is in any other name, you must be running 64-bit...
                    g_globals.g_obs = is_process_running(xorstr_(L"obs64.exe"));
				});

				last_check = GetTickCount64();
			}
		}

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtrWithVTable(local_player)) return;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0, 1);

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (g_globals.g_obs && dis(gen) < 0.7)
				continue;

			const auto player = player_manager->m_ppPlayers[i];
			if (!IsValidPtrWithVTable(player))
				continue;

			if (player == local_player)
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			const auto vehicle = player->GetVehicle();
			if (IsValidPtrWithVTable(vehicle))
			{
				const auto components = vehicle->m_pComponents;
				if (!IsValidPtr(components)) continue;

				const auto map = components->GetComponentByClassId<ClientSpottingTargetComponent>(378);
				if (IsValidPtrWithVTable(map))
					map->activeSpotType = g_globals.g_should_draw ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
			else
			{
				const auto soldier = player->GetSoldier();
				if (!IsValidPtrWithVTable(soldier)) continue;

				if (!soldier->IsAlive()) continue;

				const auto components = soldier->m_pComponents;
				if (!IsValidPtr(components)) continue;

				const auto map = components->GetComponentByClassId<ClientSpottingTargetComponent>(378);
				if (IsValidPtrWithVTable(map))
					map->activeSpotType = g_globals.g_should_draw ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
		}
	}
}