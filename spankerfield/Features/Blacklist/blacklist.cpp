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
	std::filesystem::path get_path()
	{
		auto file_path = get_appdata_folder();
		file_path /= xorstr_("Blacklisted.json");

		return file_path;
	}

	nlohmann::json get_json()
	{
		auto file_path = get_path();

		nlohmann::json players;
		std::ifstream file(file_path);

		if (!file.fail())
			file >> players;

		return players;
	}

	void save_json(nlohmann::json json)
	{
		auto file_path = get_path();

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

		players[name][xorstr_("Time added")] = current_time();

		save_json(players);
		parse_blacklist();
	}

	void delete_from_blacklist(std::string name)
	{
		nlohmann::json players = get_json();
		LOG(INFO) << name;
		players.erase(name);

		save_json(players);

		// For instant changes, apparently parsing again is not enough.
		auto it = std::find(blacklisted.begin(), blacklisted.end(), name);
		if (*it == name)
		{
			__int64 index = it - blacklisted.begin();
			blacklisted.erase(blacklisted.begin() + index);
		}

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
					m_drawing->AddText(g_globals.g_width / 2.f, 95.f + offset, ImColor::Red(), 28.f, FL_CENTER_X, nickname);
					offset += 20.f;
				}
			}
		}
	}
}