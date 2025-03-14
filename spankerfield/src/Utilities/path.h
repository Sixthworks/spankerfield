#pragma once
#include "../common.h"

namespace big
{
	void validate_path(std::filesystem::path path);
	std::filesystem::path get_appdata_folder();
}