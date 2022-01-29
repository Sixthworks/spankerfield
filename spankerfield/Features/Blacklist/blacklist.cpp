#include "blacklist.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/xorstr.h"
#include "../../global.h"

static const char* Blacklisted[]
{
	"RenKim1234",
	"m_PMKevinRudSan",
	"Sovereign_Skies",
	"skulls",
	"supervillain321",
	"Revolving_DCON",
	"Voltolly",
	"DoggGodd",
	"Cmmd_mx",
	"TheNotoriousDoge",
	"Il-Scavenger-lI",
	"BurtTheBlueberry",
	"Torsten-AUR-",
	"Riotranz",
	"Kersplooshers",
	"KarekCat",
	"FrAnK-CaStLe_Kil",
	"Striker77733",
	"Infinatshadex",
	"Flying_Autist",
	"TheGodDamnPope",
	"Fortunate_Frank",
	"Gompyzone",
	"GoodFood2K16",
	"Angry_Cuban13",
	"Daniel_Recker_95"
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

			const char* PlayerName = IsValidPtr(Player->m_Name) ? Player->m_Name : "Unknown";
			for (int i = 0; i < sizeof(Blacklisted) / sizeof(const char*); i++)
			{
				if (Blacklisted[i] == PlayerName)
				{
					m_pDrawing->AddText(g_Width / 2, 95.f + AdminsOffset, ImColor::Red(), 28.f, FL_CENTER_X, u8"%s", PlayerName);
					AdminsOffset += 20;
				}
			}
		}
	}
}
