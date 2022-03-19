#pragma once
#include "../common.h"

namespace big
{
	bool punkbuster_check();
	std::string current_time();
	bool is_process_running(const wchar_t* process_name);
}