#include "minimap.h"
#include "../../SDK/sdk.h"

namespace Features
{
	void Minimap(bool FFPB)
	{
		const auto GameContext = ClientGameContext::GetInstance();
		if (!GameContext) return;

		const auto PlayerManager = GameContext->m_pPlayerManager;
		if (!PlayerManager) return;

		const auto LocalPlayer = PlayerManager->m_pLocalPlayer;
		if (!LocalPlayer) return;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* Player = PlayerManager->m_ppPlayers[i];
			if (!Player)
				continue;

			if (Player == LocalPlayer)
				continue;

			if (Player->m_TeamId == LocalPlayer->m_TeamId)
				continue;

			ClientVehicleEntity* Vehicle = Player->GetVehicle();
			if (Vehicle)
			{
				Vehicle->m_pComponents->GetComponentByClassId<ClientSpottingTargetComponent>(378)->activeSpotType = FFPB ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
			else
			{
				ClientSoldierEntity* Soldier = Player->GetSoldier();
				if (!Soldier) continue;

				if (!Soldier->IsAlive())
					continue;

				Soldier->m_pComponents->GetComponentByClassId<ClientSpottingTargetComponent>(378)->activeSpotType = FFPB ? ClientSpottingTargetComponent::SpotType_Active : ClientSpottingTargetComponent::SpotType_None;
			}
		}
	}
}