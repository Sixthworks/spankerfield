#include "vtablehook.h"

namespace Utilities
{
	PBYTE HookVTableFunction(PDWORD64* ppVTable, PBYTE pHook, SIZE_T iIndex)
	{
		DWORD dwOld = 0;
		VirtualProtect((void*)((*ppVTable) + iIndex), sizeof(PDWORD64), PAGE_EXECUTE_READWRITE, &dwOld);

		PBYTE pOrig = ((PBYTE)(*ppVTable)[iIndex]);
		(*ppVTable)[iIndex] = (DWORD64)pHook;
		VirtualProtect((void*)((*ppVTable) + iIndex), sizeof(PDWORD64), dwOld, &dwOld);

		return pOrig;
	}
}