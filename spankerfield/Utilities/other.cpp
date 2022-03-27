#include "other.h"
#include "../SDK/sdk.h"
#include "../global.h"
#include <random>

namespace big
{
	bool punkbuster_check()
	{
		static auto screenshot_module = (uintptr_t*)OFFSET_SSMODULE;
		if (!IsValidPtr(screenshot_module))
			return true;

		bool status = (*(int*)(*screenshot_module + 0x14) != -1);
		g_globals.g_punkbuster = status;

		return status;
	}

	int generate_random_int(int min, int max)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);

		return dist6(rng);
	}

	float generate_random_float(float min, float max)
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<> distr(min, max);

		return (float)distr(eng);
	}

	std::string current_time()
	{
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, xorstr_("%d-%m-%Y %H-%M-%S"));
		auto str = oss.str();

		return str;
	};

	bool is_process_running(const wchar_t* process_name)
	{
		bool exists = false;
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry))
			while (Process32Next(snapshot, &entry))
				if (!_wcsicmp(entry.szExeFile, process_name))
					exists = true;

		CloseHandle(snapshot);
		return exists;
	}
}