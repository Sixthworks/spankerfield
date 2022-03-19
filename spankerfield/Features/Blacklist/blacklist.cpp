#include "blacklist.h"
#include "../../settings.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../global.h"
#include "../../Utilities/path.h"
#include "../../Utilities/other.h"

using namespace big;
namespace plugins
{
	nlohmann::json get_json()
	{
		auto file_path = get_appdata_folder();
		file_path /= xorstr_("Blacklisted.json");

		nlohmann::json players;
		std::ifstream file(file_path);

		if (!file.fail())
			file >> players;

		return players;
	}

	void save_json(nlohmann::json json)
	{
		auto file_path = get_appdata_folder();
		file_path /= xorstr_("Blacklisted.json");

		std::ofstream rest(file_path, std::ios::out | std::ios::trunc);
		rest << json.dump(4);
		rest.close();
	}

	void parse_blacklist()
	{
		nlohmann::json players = get_json();
		
		std::vector<std::string> temp;
		for (auto& el : players.items())
			temp.push_back(el.key());

		if (temp.size() > 0)
			blacklisted = temp;
	}

	void add_to_blacklist(std::string name)
	{
		nlohmann::json players = get_json();

		if (players.contains(name))
			return;

		players[name][xorstr_("time_added")] = current_time();

		save_json(players);
		parse_blacklist();
	}

	void delete_from_blacklist(std::string name)
	{
		nlohmann::json players = get_json();

		if (players[name].is_null())
			return;

		players.erase(name);

		save_json(players);
		parse_blacklist();
	}

	void draw_blacklisted()
	{
		if (!g_settings.blacklist) return;

		if (GetTickCount64() - g_globals.g_blcheck > 5000)
		{
			parse_blacklist();

			g_globals.g_blcheck = GetTickCount64();
		}

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
			for (const auto& bl : blacklisted)
			{
				if (nickname == bl)
				{
					m_drawing->AddText(g_globals.g_width / 2, 95.f + offset, ImColor::Red(), 28.f, FL_CENTER_X, xorstr_(u8"%s"), nickname);
					offset += 20;
				}
			}
		}
	}
}