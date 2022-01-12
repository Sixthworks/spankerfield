#include "spectators.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"

namespace Features
{
	void DrawSpectators()
	{
		const auto GameContext = ClientGameContext::GetInstance();
		if (!GameContext) return;

		const auto PlayerManager = GameContext->m_pPlayerManager;
		if (!PlayerManager) return;

		const auto LocalPlayer = PlayerManager->m_pLocalPlayer;
		if (!LocalPlayer) return;

		float SpectatorOffset = 0.f;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* Player = PlayerManager->m_ppPlayers[i];
			if (!Player)
				continue;

			if (Player == LocalPlayer)
				continue;

			if (Player->m_IsSpectator)
			{
				const char* PlayerName = IsValidPtr(Player->m_Name) ? Player->m_Name : "Unknown";
				m_pDrawing->AddText(23.5f, 450.f + SpectatorOffset, ImColor::White(), 26.f, FL_NONE, u8"%s", PlayerName);

				SpectatorOffset += 20;
			}
		}
	}
}
