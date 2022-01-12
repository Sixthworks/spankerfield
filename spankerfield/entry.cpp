﻿#include <Windows.h>
#include "SDK/sdk.h"
#include "MinHook.h"

#include "Rendering/base.h"
#include "Utilities/xorstr.h"
#include "Utilities/vtablehook.h"
#include "Hooks/hooks.h"

DWORD WINAPI InitThread(LPVOID reserved) noexcept
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE* out{};
	freopen_s(&out, "CONOUT$", "w", stdout);

	if (MH_Initialize() != MH_OK)
		goto clean;

	Hooks::Hook();
	printf(xorstr_("Farewell!\n"));

	return TRUE;
clean:
	FreeConsole();
	FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(reserved), 0);
	return NULL;
}

void Deattach() {
	ImSetup::KillImgui();
	FreeConsole();
	Hooks::UnHook();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hinstDLL);
		CloseHandle(CreateThread(NULL, NULL, InitThread, hinstDLL, NULL, NULL));
	}
	if (fdwReason == DLL_PROCESS_DETACH) {
		Deattach();
	}
	return TRUE;
}