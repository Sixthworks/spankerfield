#include "other.h"
#include "../SDK/sdk.h"
#include "../global.h"

namespace big
{
	bool punkbuster_check()
	{
		static auto screenshot_module = (uintptr_t*)OFFSET_SSMODULE;
		if (!IsValidPtr(screenshot_module))
			return true; // Don't draw just in case.

		bool status = (*(int*)(*screenshot_module + 0x14) != -1);
		g_globals.g_punkbuster = status;

		return status;
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