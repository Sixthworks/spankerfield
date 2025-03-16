#include "vmt_hook.h"

namespace big
{
	void VMTHook::Hook(int index, void* fnPointer)
	{
		if ((int)index >= 0 && (int)index <= (int)vmtLenght) {
			DWORD oldProtect;
			VirtualProtect(&pNewVmt[EXTRA_SPACE + (int)index + 1], sizeof(std::uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect);
			pNewVmt[EXTRA_SPACE + (int)index + 1] = reinterpret_cast<std::uintptr_t>(fnPointer);
			VirtualProtect(&pNewVmt[EXTRA_SPACE + (int)index + 1], sizeof(std::uintptr_t), oldProtect, &oldProtect);
			FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
		}
	}

	bool VMTHook::Release()
	{
		if (pClassVmt == nullptr) return false;

		// Use VirtualProtect for safe pointer modification
		DWORD oldProtect;
		VirtualProtect(ppClassBase, sizeof(std::uintptr_t*), PAGE_EXECUTE_READWRITE, &oldProtect);
		*(std::uintptr_t**)ppClassBase = pClassVmt;
		VirtualProtect(ppClassBase, sizeof(std::uintptr_t*), oldProtect, &oldProtect);

		// Flush the cache to ensure instruction consistency
		FlushInstructionCache(GetCurrentProcess(), nullptr, 0);

		std::memset(pNewVmt, NULL, vmtLenght * sizeof(std::uintptr_t) + (sizeof(std::uintptr_t) * (EXTRA_SPACE + 1)));

		pClassVmt = nullptr;
		pNewVmt = nullptr;
		ppClassBase = nullptr;
		vmtLenght = NULL;

		return true;
	}

	bool VMTHook::Setup(void* pclass)
	{
		if (pclass == nullptr)
			return false;

		ppClassBase = reinterpret_cast<uintptr_t**>(pclass);
		if (ppClassBase == nullptr)
			return false;

		pClassVmt = *ppClassBase;
		if (pClassVmt == nullptr)
			return false;

		// Safer VMT length detection
		vmtLenght = 0;

		// Safer approach to find VMT length
		// Most VMTs don't exceed 100 entries, and the entries must be valid function pointers
		// We'll check if the pointer is within reasonable executable memory
		const size_t MAX_VMT_SIZE = 150; // Reasonable upper limit

		for (size_t i = 0; i < MAX_VMT_SIZE; i++) {
			MEMORY_BASIC_INFORMATION mbi = { 0 };
			void* funcPtr = reinterpret_cast<void*>(pClassVmt[i]);

			// Check if the pointer value is within a reasonable range
			// This avoids crashes by ensuring the pointer is vaguely valid
			if (reinterpret_cast<uintptr_t>(funcPtr) < 0x10000 ||
				reinterpret_cast<uintptr_t>(funcPtr) > 0x7FFFFFFFFFFFFFFF)
				break;

			// Attempt to query the function pointer's memory info
			if (!VirtualQuery(funcPtr, &mbi, sizeof(mbi)))
				break;

			// Only count if the memory has executable permissions
			if (!(mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))
				break;

			vmtLenght++;
		}

		if (vmtLenght <= 0)
			return false;

		auto vmtSize = vmtLenght * sizeof(std::uintptr_t);

		HMODULE hModule = NULL;
		if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCSTR)pClassVmt, &hModule) == NULL)
			return false;

		char path[MAX_PATH];
		if (GetModuleFileNameA(hModule, path, sizeof(path)) == NULL)
			return false;

		std::string modulePath = path;
		if (modulePath.empty())
			return false;

		std::size_t found = modulePath.find_last_of("/\\");
		modulePath.erase(0, found + 1);

		auto newVmtSizeExt = vmtSize + (sizeof(std::uintptr_t) * (EXTRA_SPACE + 1));
		pNewVmt = this->SearchFreeDataPage(modulePath.c_str(), newVmtSizeExt);
		if (pNewVmt == nullptr)
			return false;

		// Always use VirtualProtect for better stability
		DWORD old;
		VirtualProtect(pNewVmt, newVmtSizeExt, PAGE_READWRITE, &old);

		std::memset(pNewVmt, NULL, newVmtSizeExt);
		std::memcpy(&pNewVmt[EXTRA_SPACE + 1], pClassVmt, vmtSize);

		// Safely copy the RTTI pointer
		try {
			pNewVmt[EXTRA_SPACE] = pClassVmt[-1];
		}
		catch (...) {
			// If RTTI copy fails, it's not critical, we can continue
		}

		VirtualProtect(pNewVmt, newVmtSizeExt, old, &old);

		try {
			// Properly protect memory while modifying the vtable pointer
			DWORD origProtect;
			if (VirtualProtect(ppClassBase, sizeof(uintptr_t*), PAGE_EXECUTE_READWRITE, &origProtect)) {
				*(std::uintptr_t**)ppClassBase = &pNewVmt[EXTRA_SPACE + 1];
				VirtualProtect(ppClassBase, sizeof(uintptr_t*), origProtect, &origProtect);
				FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
			}
			else {
				delete[] pNewVmt;
				return false;
			}
		}
		catch (...)
		{
			delete[] pNewVmt;
			return false;
		}
		return true;
	}

	std::uintptr_t* VMTHook::SearchFreeDataPage(const char* moduleName, const size_t vmtSize)
	{
		auto CheckDataSection = [&](LPCVOID address, const std::size_t vmt_size) -> bool
			{
				const DWORD dataProtection = (PAGE_EXECUTE_READWRITE | PAGE_READWRITE);
				MEMORY_BASIC_INFORMATION mbi = { 0 };

				if (VirtualQuery(address, &mbi, sizeof(mbi)) == sizeof(mbi) && mbi.AllocationBase && mbi.BaseAddress &&
					mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) && mbi.Protect != PAGE_NOACCESS)
				{
					if ((mbi.Protect & dataProtection) && mbi.RegionSize >= vmt_size)
						return ((mbi.Protect & dataProtection) && mbi.RegionSize >= vmt_size) ? true : false;
				}
				return false;
			};

		auto hModule = GetModuleHandleA(moduleName);

		if (hModule == nullptr) return nullptr;

		const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER> (hModule);
		const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS> (reinterpret_cast<std::uint8_t*>(hModule) + dosHeader->e_lfanew);
		const auto moduleEnd = reinterpret_cast<std::uintptr_t>(hModule) + ntHeader->OptionalHeader.SizeOfImage - sizeof(std::uintptr_t);

		for (auto currAddress = moduleEnd; currAddress > (DWORD)hModule; currAddress -= sizeof(std::uintptr_t))
		{
			if (*reinterpret_cast<std::uintptr_t*>(currAddress) == 0 && CheckDataSection(reinterpret_cast<LPCVOID>(currAddress), vmtSize))
			{
				bool isGoodVmt = true;
				auto pageCount = 0u;

				for (; pageCount < vmtSize && isGoodVmt; pageCount += sizeof(std::uintptr_t))
				{
					if (*reinterpret_cast<std::uintptr_t*>(currAddress + pageCount) != 0)
						isGoodVmt = false;
				}

				if (isGoodVmt && pageCount >= vmtSize)
					return (uintptr_t*)currAddress;
			}
		}

		return nullptr;
	}
}