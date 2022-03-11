#include "blacklist.h"
#include "../../settings.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/xorstr.h"
#include "../../global.h"

std::vector<std::string> list
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

namespace features
{
	void draw_blacklisted()
	{
		if (!settings::blacklist) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		float offset = 0.f;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			const char* nickname = IsValidPtr(player->m_Name) ? player->m_Name : xorstr_("Unknown");
			for (const auto& bl : list)
			{
				if (nickname == bl)
				{
					m_pDrawing->AddText(globals::g_Width / 2, 95.f + offset, ImColor::Red(), 28.f, FL_CENTER_X, xorstr_(u8"%s"), nickname);
					offset += 20;
				}
			}
		}
	}
}
