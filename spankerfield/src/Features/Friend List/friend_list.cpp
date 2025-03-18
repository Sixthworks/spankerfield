#include "friend_list.h"
#include "../../settings.h"
#include "../../SDK/sdk.h"
#include "../../global.h"
#include "../../Utilities/path.h"
#include "../../Utilities/other.h"
#include "../../Utilities/thread_pool.h"

using namespace big;
namespace plugins
{
	static int exclusive = 835729164; // Random persona ID which won't occur in a game by random chance.

	std::filesystem::path get_friends_path()
	{
		return std::filesystem::path(get_appdata_folder()) / xorstr_("Friends.json");
	}

	nlohmann::json get_friends_json()
	{
		auto file_path = get_friends_path();
		nlohmann::json players;

		try
		{
			std::ifstream file(file_path);
			if (file.is_open())
				players = nlohmann::json::parse(file);
		}
		catch (const std::exception& e)
		{
			LOG(INFO) << xorstr_("Error reading friends JSON file: ") << e.what();
		}

		return players;
	}

	void save_friends_json(nlohmann::json json)
	{
		auto file_path = get_friends_path();

		std::ofstream rest(file_path, std::ios::out | std::ios::trunc);
		rest << json.dump(4);
		rest.close();
	}

	void parse_friends()
	{
		const auto players = get_friends_json();

		std::vector<friend_s> temp;
		for (const auto& [name, data] : players.items())
		{
			friend_s element;
			element.name = name;
			element.persona_id = data[xorstr_("Persona ID")];
			temp.push_back(element);
		}

		if (!temp.empty())
			friends_list = std::move(temp);
	}

	void request_friend_id_change(std::string name)
	{
		nlohmann::json players = get_friends_json();

		if (!players.contains(name))
			return;

		const auto player = get_player_by_name(name);
		if (IsValidPtr(player))
		{
			const auto id = player->m_onlineId.m_personaid;
			if (!id) return;

			// Update json
			players[name][xorstr_("Persona ID")] = id;

			save_friends_json(players);
			parse_friends();
		}
	}

	void request_friend_name_change(std::string name)
	{
		nlohmann::json players = get_friends_json();

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

			save_friends_json(players);
			parse_friends();
		}
	}

	void add_to_friends(std::string name)
	{
		if (name.empty())
			return;

		nlohmann::json players = get_friends_json();

		if (players.contains(name))
			return;

		const auto player = get_player_by_name(name);
		const auto val = IsValidPtr(player) ? player->m_onlineId.m_personaid : exclusive;

		players[name][xorstr_("Persona ID")] = val;
		players[name][xorstr_("Time added")] = current_time();

		save_friends_json(players);
		parse_friends();
	}

	void delete_from_friends(std::string name)
	{
		nlohmann::json players = get_friends_json();
		players.erase(name);

		save_friends_json(players);

		// For instant changes, apparently parsing again is not enough.
		int i = 0;
		for (const auto& rs : friends_list)
		{
			if (rs.name == name)
			{
				friends_list.erase(friends_list.begin() + i);
				break;
			}

			i++;
		}

		parse_friends();
	}

	bool is_friend(uint64_t persona_id)
	{
		for (const auto& fr : friends_list)
		{
			if (fr.persona_id == persona_id)
				return true;
		}
		return false;
	}

	bool is_friend_by_name(const std::string& name)
	{
		for (const auto& fr : friends_list)
		{
			if (fr.name == name)
				return true;
		}
		return false;
	}

	static ULONGLONG last_friend_check = 0;
	void update_friends_list()
	{
		if (GetTickCount64() - last_friend_check > 2000)
		{
			g_thread_pool->push([&]
				{
					parse_friends();
				});

			last_friend_check = GetTickCount64();
		}

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!IsValidPtr(player))
				continue;

			if (player == local_player)
				continue;

			auto nickname = IsValidPtr(player->m_Name) ? player->m_Name : xorstr_("Unknown");
			auto id = player->m_onlineId.m_personaid;

			for (const auto& fl : friends_list)
			{
				if (id != fl.persona_id)
				{
					if (fl.persona_id == exclusive)
					{
						if (nickname == fl.name)
							request_friend_id_change(nickname);
					}
				}

				if (id == fl.persona_id)
				{
					if (nickname != fl.name)
						request_friend_name_change(fl.name);
				}
			}
		}
	}
}