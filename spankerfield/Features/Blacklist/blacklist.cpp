#include "blacklist.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/xorstr.h"
#include "../../global.h"

std::vector<std::string> Blacklisted
{
	xorstr_("RenKim1234"),
	xorstr_("m_PMKevinRudSan"),
	xorstr_("Sovereign_Skies"),
	xorstr_("skulls"),
	xorstr_("supervillain321"),
	xorstr_("Revolving_DCON"),
	xorstr_("Voltolly"),
	xorstr_("DoggGodd"),
	xorstr_("Cmmd_mx"),
	xorstr_("TheNotoriousDoge"),
	xorstr_("Il-Scavenger-lI"),
	xorstr_("BurtTheBlueberry"),
	xorstr_("Torsten-AUR-"),
	xorstr_("Riotranz"),
	xorstr_("Kersplooshers"),
	xorstr_("KarekCat"),
	xorstr_("FrAnK-CaStLe_Kil"),
	xorstr_("Striker77733"),
	xorstr_("Infinatshadex"),
	xorstr_("Flying_Autist"),
	xorstr_("TheGodDamnPope"),
	xorstr_("Fortunate_Frank"),
	xorstr_("Gompyzone"),
	xorstr_("GoodFood2K16"),
	xorstr_("Angry_Cuban13"),
	xorstr_("Daniel_Recker_95"),
};

namespace Features
{
	void ShowBlacklisted()
	{
		const auto GameContext = ClientGameContext::GetInstance();
		if (!GameContext) return;

		const auto PlayerManager = GameContext->m_pPlayerManager;
		if (!PlayerManager) return;

		const auto LocalPlayer = PlayerManager->m_pLocalPlayer;
		if (!LocalPlayer) return;

		float AdminsOffset = 0.f;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* Player = PlayerManager->m_ppPlayers[i];
			if (!Player)
				continue;

			if (Player == LocalPlayer)
				continue;

			const char* PlayerName = IsValidPtr(Player->m_Name) ? Player->m_Name : xorstr_("Unknown");
			for (const auto& bl : Blacklisted)
			{
				if (PlayerName == bl)
				{
					m_pDrawing->AddText(g_Width / 2, 95.f + AdminsOffset, ImColor::Red(), 28.f, FL_CENTER_X, xorstr_(u8"%s"), PlayerName);
					AdminsOffset += 20;
				}
			}
		}
	}
}
