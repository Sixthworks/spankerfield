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
			ClientPlayer* player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			if (player->m_IsSpectator)
			{
				const char* nickname = IsValidPtr(player->m_Name) ? player->m_Name : xorstr_("Unknown");

				spectators.push_back(nickname);
			}
		}

		if (g_settings.raw_drawing)
		{
			float offset = 0.f;
			for (const auto& rs : spectators)
			{
				m_drawing->AddText(g_settings.spectator_x, g_settings.spectator_y + offset, ImColor::White(), 26.f, FL_NONE, xorstr_(u8"%s"), rs.c_str());
				offset += 20;
			}
		}
		else
		{
			ImGuiWindowFlags flags = g_gui.m_opened ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
			if (!set)
			{
				ImGui::SetNextWindowSize(ImVec2(250, 125.f));
				ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 260.f, 8.f));
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
