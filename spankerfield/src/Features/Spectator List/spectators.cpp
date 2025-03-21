#include "spectators.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Rendering/gui.h"
#include "../../Features/Blacklist/blacklist.h"

// How much time (sec) the spectator is considered new to be alerted
#define NEW_SPECTATOR_TIMER 10

using namespace big;
namespace plugins
{
	static bool set = false;
	static bool was_spectated = false;
	static bool esp_backup = false;
	static bool radar_backup = false;
	static bool infantry_alert_backup = false;
	static bool aimbot_backup = false;
	static std::unordered_set<std::string> previous_spectators;
	static std::vector<std::pair<std::string, std::chrono::steady_clock::time_point>> new_spectator_alerts;

	void draw_spectators()
	{
		if (!g_settings.spectator_list) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		// Vector for storing spectator names
		std::vector<std::string> spectators{};
		std::unordered_set<std::string> current_spectators;

		// Filling the vector with spectators
		int censoreds = 0;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!IsValidPtr(player))
				continue;

			if (player->m_IsSpectator)
			{
				// Also make an additional check here if the player is actually spectating YOU.
				// ClientPlayer::ClientPlayerView* pView = pPlayer->m_PlayerView;
				// IsValidPtr(pView) && pView->m_Owner == pLocalPlayer)
				// I don't have this in my SDK, but you can add it if you wish.
				// https://github.com/headmax/Cplus/blob/master/Projet4/esp.h#L406=
				// https://github.com/headmax/Cplus/blob/master/Projet4/bfsdk.h#L6576=
				std::string nickname;
				if (g_settings.streamer_mode)
				{
					nickname = player == local_player ? xorstr_("You") : xorstr_("Censored ") + std::to_string(censoreds);
					censoreds++;
				}
				else
					nickname = IsValidPtr(player->m_Name) ? (player == local_player ? xorstr_("(You) ") + std::string(player->m_Name) : std::string(player->m_Name)) : xorstr_("Unknown");

				spectators.push_back(nickname);
				current_spectators.insert(nickname);
			}
		}

		// Check for new spectators (only if warning about new spectators is enabled)
		if (g_settings.spectator_warn_new)
		{
			for (const auto& spec : current_spectators)
			{
				if (previous_spectators.find(spec) == previous_spectators.end() && spec.find(xorstr_("(You)")) == std::string::npos)
				{
					// New spectator found
					new_spectator_alerts.push_back({ spec, std::chrono::steady_clock::now() });
				}
			}
		}

		// Handle ESP and aimbot disabling when spectated
		bool is_being_spectated = !spectators.empty() && spectators.size() > (std::find(spectators.begin(), spectators.end(), xorstr_("(You) ") + std::string(local_player->m_Name)) != spectators.end() ? 1 : 0);

		if (is_being_spectated && g_settings.spectator_disable_esp_aim)
		{
			if (!was_spectated)
			{
				// Backup current settings before disabling
				esp_backup = g_settings.esp;
				radar_backup = g_settings.radar;
				infantry_alert_backup = g_settings.infantry_alert;
				aimbot_backup = g_settings.aimbot;

				// Disable ESP and aimbot
				g_settings.esp = false;
				g_settings.aimbot = false;

				was_spectated = true;
			}
		}
		else if (!is_being_spectated && was_spectated)
		{
			// Re-enable ESP and aimbot if they were enabled before
			g_settings.esp = esp_backup;
			g_settings.radar = radar_backup;
			g_settings.infantry_alert = infantry_alert_backup;
			g_settings.aimbot = aimbot_backup;
			was_spectated = false;
		}

		// Update previous spectators for next check
		previous_spectators = current_spectators;

		// Clean up expired spectator alerts
		auto now = std::chrono::steady_clock::now();
		auto it = new_spectator_alerts.begin();
		while (it != new_spectator_alerts.end())
		{
			if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count() > NEW_SPECTATOR_TIMER)
				it = new_spectator_alerts.erase(it);
			else
				++it;
		}

		// Draw spectator list
		if (g_settings.spectator_raw_drawing)
		{
			float offset = 0.f;
			for (const auto& rs : spectators)
			{
				m_drawing->AddText(g_settings.spectator_x, g_settings.spectator_y + offset, ImColor(255, 255, 255, 255), 26.f, FL_SHADOW, rs.c_str());
				offset += 20.f;
			}
		}
		else
		{
			ImGuiWindowFlags flags = g_gui.m_opened ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
			if (!set)
			{
				ImGui::SetNextWindowSize(ImVec2(250, 125.f), ImGuiCond_FirstUseEver);
				ImGui::SetNextWindowPos(ImVec2((float)g_globals.g_width - 260.f, 8.f), ImGuiCond_FirstUseEver);
				set = true;
			}

			if (ImGui::Begin(xorstr_("Spectator list"), nullptr, flags))
			{
				for (const auto& rs : spectators)
					ImGui::Text(rs.c_str());
			}
			ImGui::End();
		}

		// Position warnings at the bottom center of the screen to avoid conflict with blacklist text
		if ((is_being_spectated && g_settings.spectator_disable_esp_aim) || (!new_spectator_alerts.empty() && g_settings.spectator_warn_new))
		{
			// Start position for warnings - default start position for blacklist, will be changed (look blacklist support later)
			float y_pos = 75.f;

			if (g_settings.blacklist)
			{
				for (const auto& bl : blacklisted)
				{
					// Count only visible blacklisted entries
					for (int i = 0; i < MAX_PLAYERS; i++)
					{
						const auto player = player_manager->m_ppPlayers[i];
						if (IsValidPtr(player) && player != local_player &&
							player->m_onlineId.m_personaid == bl.persona_id)
						{
							// Adjust the offset based on warning size
							y_pos += g_settings.spectator_warnings_size * 0.7f;
							break;
						}
					}
				}
				y_pos += 25.f; // Add padding after blacklist
			}

			if (g_settings.spectator_warn_new && !new_spectator_alerts.empty())
			{
				for (auto it = new_spectator_alerts.rbegin(); it != new_spectator_alerts.rend(); ++it)
				{
					std::string alert_text = xorstr_("NEW SPECTATOR: ") + it->first;
					ImVec2 text_size = ImGui::CalcTextSize(alert_text.c_str());
					m_drawing->AddText((float)g_globals.g_width / 2, y_pos, g_settings.spectator_warnings_color, g_settings.spectator_warnings_size, FL_CENTER_X | FL_SHADOW, alert_text.c_str());

					y_pos += g_settings.spectator_warnings_size * 1.25f;
				}
			}

			if (is_being_spectated && g_settings.spectator_disable_esp_aim)
			{
				const char* text = xorstr_("BEING SPECTATED - VISUALS AND AIM DISABLED");
				ImVec2 text_size = ImGui::CalcTextSize(text);
				m_drawing->AddText((float)g_globals.g_width / 2.f, y_pos, g_settings.spectator_warnings_color, g_settings.spectator_warnings_size, FL_CENTER_X | FL_SHADOW, text);

				y_pos += g_settings.spectator_warnings_size * 1.15f;
			}
		}
	}
}