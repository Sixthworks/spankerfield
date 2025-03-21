#include "blacklist.h"
#include "../../settings.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../global.h"
#include "../../Utilities/path.h"
#include "../../Utilities/other.h"
#include "../../Utilities/thread_pool.h"

using namespace big;
namespace plugins
{
	static int exclusive = 193738923; // Random persona ID which won't occur in a game by random chance.

	std::filesystem::path get_path()
	{
		return std::filesystem::path(get_appdata_folder()) / xorstr_("Blacklisted.json");
	}

	nlohmann::json get_json()
	{
		auto file_path = get_path();
		nlohmann::json players;

		try
		{
			std::ifstream file(file_path);
			if (file.is_open()) 
				players = nlohmann::json::parse(file);
		}
		catch (const std::exception& e)
		{
			LOG(INFO) << xorstr_("Error reading JSON file: ") << e.what();
		}

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
		const auto players = get_json();

		std::vector<blacklisted_s> temp;
		for (const auto& [name, data] : players.items())
		{
			blacklisted_s element;
			element.name = name;
			element.persona_id = data[xorstr_("Persona ID")];
			temp.push_back(element);
		}

		if (!temp.empty())
			blacklisted = std::move(temp);
	}

	void request_id_change(std::string name)
	{
		nlohmann::json players = get_json();

		if (!players.contains(name))
			return;

		const auto player = get_player_by_name(name);
		if (IsValidPtr(player))
		{
			const auto id = player->m_onlineId.m_personaid;
			if (!id) return;

			// Update json
			players[name][xorstr_("Persona ID")] = id;

			save_json(players);
			parse_blacklist();
		}
	}

	void request_name_change(std::string name)
	{
		nlohmann::json players = get_json();

		if (!players.contains(name))
			return;

		const auto player = get_player_by_name(name);
		if (IsValidPtr(player))
		{
			const auto nickname = player->m_Name;
			if (!nickname) return;

			// Update json
			nlohmann::json temp = players[name];
			players.erase(name);
			players[nickname] = temp;

			save_json(players);
			parse_blacklist();
		}
	}

	void add_to_blacklist(std::string name)
	{
		nlohmann::json players = get_json();

		if (players.contains(name))
			return;

		const auto player = get_player_by_name(name);
		const auto val = IsValidPtr(player) ? player->m_onlineId.m_personaid : exclusive;

		players[name][xorstr_("Persona ID")] = val;
		players[name][xorstr_("Time added")] = current_time();

		save_json(players);
		parse_blacklist();
	}

	void delete_from_blacklist(std::string name)
	{
		nlohmann::json players = get_json();
		players.erase(name);

		save_json(players);

		// For instant changes, apparently parsing again is not enough.
		int i = 0;
		for (const auto& rs : blacklisted)
		{
			if (rs.name == name)
			{
				blacklisted.erase(blacklisted.begin() + i);
				break;
			}

			i++;
		}

		parse_blacklist();
	}

	static ULONGLONG last_check = 0;
	void draw_blacklisted()
	{
		if (!g_settings.blacklist) return;

		if (GetTickCount64() - last_check > 2000)
		{
			g_thread_pool->push([&]
			{
				parse_blacklist();
			});

			last_check = GetTickCount64();
		}

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		float offset = 0.f;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!IsValidPtr(player))
				continue;

			if (player == local_player)
				continue;

			auto nickname = IsValidPtr(player->m_Name) ? player->m_Name : xorstr_("Unknown");
			auto id = player->m_onlineId.m_personaid;

			for (const auto& bl : blacklisted)
			{
				if (id != bl.persona_id)
				{
					if (bl.persona_id == exclusive)
					{
						if (nickname == bl.name)
							request_id_change(nickname);
					}
				}

				if (id == bl.persona_id)
				{
					if (nickname != bl.name)
						request_name_change(bl.name);

					m_drawing->AddText((float)g_globals.g_width / 2.f, 75.f + offset, g_settings.blacklist_color, g_settings.blacklist_text_size, FL_CENTER_X | FL_SHADOW, bl.name.c_str());
					offset += 20.f;
				}
			}
		}
	}
}