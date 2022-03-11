#pragma once
#include <Windows.h>

namespace utils
{
	PBYTE HookVTableFunction(PDWORD64* ppVTable, PBYTE pHook, SIZE_T iIndex);
}