#pragma once
#include "../../common.h"

namespace plugins
{
	struct friend_s
	{
		std::string name{};
		uint64_t persona_id{};
	};
	inline std::vector<friend_s> friends_list;

	inline int selected_friend;

	void add_to_friends(std::string name);
	void delete_from_friends(std::string name);
	void update_friends_list();
	bool is_friend(uint64_t persona_id);
	bool is_friend_by_name(const std::string& name);
}