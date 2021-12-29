#pragma once
#include <Windows.h>

namespace Utilities
{
	PBYTE HookVTableFunction(PDWORD64* ppVTable, PBYTE pHook, SIZE_T iIndex);
}