#pragma once
#include "../common.h"

namespace big
{
	bool punkbuster_check();
	int generate_random_int(int min, int max);
	float generate_random_float(float min, float max);
	std::string current_time();
	bool is_process_running(const wchar_t* process_name);
}