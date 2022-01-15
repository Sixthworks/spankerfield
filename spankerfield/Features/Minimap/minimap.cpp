#include "minimap.h"
#include "../../SDK/sdk.h"
#include "../../Utilities/xorstr.h"
#include "../../Utilities/other.h"

namespace Features
{
	ULONGLONG LastOBSCheck;
	static bool OBS = false;
	void Minimap(bool FFPB)
	{
		ULONGLONG now = GetTickCount64();
		if (now - LastOBSCheck > 5000)
		{
			OBS = Utilities::IsProcessRunning(L"obs64.exe");
			LastOBSCheck = now;
		}
		if (OBS) return;

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