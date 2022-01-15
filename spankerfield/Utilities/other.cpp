#include "other.h"
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>

namespace Utilities
{
	bool IsProcessRunning(const wchar_t* processName)
	{
		bool exists = false;
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry))
			while (Process32Next(snapshot, &entry))
				if (!_wcsicmp(entry.szExeFile, processName))
					exists = true;

		CloseHandle(snapshot);
		return exists;
	}
}