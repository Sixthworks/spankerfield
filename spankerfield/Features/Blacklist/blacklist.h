#pragma once
#include "../../common.h"

namespace plugins
{
	struct blacklisted_s
	{
		std::string name{};
		uint64_t persona_id{};
	};

	inline std::vector<blacklisted_s> blacklisted;
	inline int selected;

	void add_to_blacklist(std::string name);
	void delete_from_blacklist(std::string name);
	void draw_blacklisted();
}