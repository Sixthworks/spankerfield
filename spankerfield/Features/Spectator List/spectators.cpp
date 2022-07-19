#include "spectators.h"
#include "../../common.h"
#include "../../settings.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Rendering/gui.h"

using namespace big;
namespace plugins
{
	static bool set = false;
	void draw_spectators()
	{
		if (!g_settings.spectator_list) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		// Vector for storing spectator names
		std::vector<std::string> spectators{};

		// Filling the vector with spectators
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			if (player->m_IsSpectator)
			{
				// Also make an additional check here if the player is actually spectating YOU.
				// ClientPlayer::ClientPlayerView* pView = pPlayer->m_PlayerView;
				// IsValidPtr(pView) && pView->m_Owner == pLocalPlayer)
				// I don't have this in my SDK, but you can add it if you wish.
				// https://github.com/headmax/Cplus/blob/master/Projet4/esp.h#L406=
				// https://github.com/headmax/Cplus/blob/master/Projet4/bfsdk.h#L6576=

				const char* nickname = IsValidPtr(player->m_Name) ? player->m_Name : xorstr_("Unknown");

				spectators.push_back(nickname);
			}
		}

		if (g_settings.spectator_raw_drawing)
		{
			float offset = 0.f;
			for (const auto& rs : spectators)
			{
				m_drawing->AddText(g_settings.spectator_x, g_settings.spectator_y + offset, ImColor(255, 255, 255, 255), 26.f, FL_NONE, rs.c_str());
				offset += 20.f;
			}
		}
		else
		{
			ImGuiWindowFlags flags = g_gui.m_opened ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
			if (!set)
			{
				ImGui::SetNextWindowSize(ImVec2(250, 125.f), ImGuiCond_FirstUseEver);
				ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 260.f, 8.f), ImGuiCond_FirstUseEver);
				
				set = true;
			}

			if (ImGui::Begin(xorstr_("Spectator list"), nullptr, flags))
			{
				for (const auto& rs : spectators)
				{
					ImGui::Text(rs.c_str());
				}
			}
			ImGui::End();
		}
	}
}
