#pragma once
#include "../common.h"

namespace big
{
	PBYTE hook_vtable_func(PDWORD64* ppVTable, PBYTE pHook, SIZE_T iIndex);
}