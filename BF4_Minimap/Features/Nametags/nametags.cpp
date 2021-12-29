#include "nametags.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/w2s.h"

namespace Features
{
	void DrawNames()
	{
		const auto GameContext = ClientGameContext::GetInstance();
		if (!GameContext) return;

		const auto PlayerManager = GameContext->m_pPlayerManager;
		if (!PlayerManager) return;

		const auto LocalPlayer = PlayerManager->m_pLocalPlayer;
		if (!LocalPlayer) return;

		const auto LocalSoldier = LocalPlayer->GetSoldier();
		if (!LocalSoldier) return;

		if (!LocalSoldier->IsAlive())
			return;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* Player = PlayerManager->m_ppPlayers[i];
			if (!Player)
				continue;

			if (Player == LocalPlayer)
				continue;

			if (Player->m_TeamId == LocalPlayer->m_TeamId)
				continue;

			ClientSoldierEntity* Soldier = Player->GetSoldier();
			if (!Soldier) continue;

			if (!Soldier->IsAlive())
				continue;

			TransformAABBStruct Transform;
			ClientVehicleEntity* Vehicle = Player->GetVehicle();
			Vehicle ? Vehicle->GetAABB(&Transform) : Soldier->GetAABB(&Transform);

			Vector2 boxCords[2];
			if (GetBoxCords(Transform, &boxCords[0]))
			{
				const char* PlayerName = IsValidPtr(Player->m_Name) ? Player->m_Name : "Unknown";
				float BoxWidth = boxCords[1].x - boxCords[0].x;
				float BoxHeight = boxCords[1].y - boxCords[0].y;
				m_pDrawing->AddText(boxCords[0].x + (BoxWidth / 2), boxCords[0].y + (BoxHeight / 2), ImColor::Yellow(), 0.f, FL_CENTER_X, u8"%s", PlayerName);
			}
		}
	}
}
