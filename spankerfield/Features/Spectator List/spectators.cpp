#include "spectators.h"
#include "../../settings.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/xorstr.h"

namespace features
{
	void draw_spectators()
	{
		if (!settings::spectator_list) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		if (settings::spectator_list_debug)
		{
			static int spectator_dummies[4]{};

			float offset = 0.f;
			for (int i = 0; i < sizeof(spectator_dummies) / sizeof(*spectator_dummies); i++)
			{
				m_pDrawing->AddText(settings::spectator_x, settings::spectator_y + offset, ImColor::White(), 26.f, FL_NONE, xorstr_(u8"%s"), xorstr_("Dummy"));
				offset += 20;
			}
		}
		else
		{
			float offset = 0.f;
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				ClientPlayer* player = player_manager->m_ppPlayers[i];
				if (!player)
					continue;

				if (player == local_player)
					continue;

				if (player->m_IsSpectator)
				{
					const char* nickname = IsValidPtr(player->m_Name) ? player->m_Name : xorstr_("Unknown");

					m_pDrawing->AddText(settings::spectator_x, settings::spectator_y + offset, ImColor::White(), 26.f, FL_NONE, xorstr_(u8"%s"), nickname);
					offset += 20;
				}
			}
		}
	}
}
