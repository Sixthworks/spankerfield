#pragma once
#include "../../common.h"

namespace plugins
{
	inline std::vector<std::string> blacklisted;

	void add_to_blacklist(std::string name);
	void delete_from_blacklist(std::string name);
	void draw_blacklisted();
}