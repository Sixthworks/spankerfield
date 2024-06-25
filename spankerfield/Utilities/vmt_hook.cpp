#include "vmt_hook.h"

namespace big
{
	void VMTHook::Hook(int index, void* fnPointer)
	{
		if ((int)index >= 0 && (int)index <= (int)vmtLenght)
			pNewVmt[EXTRA_SPACE + (int)index + 1] = reinterpret_cast<std::uintptr_t>(fnPointer);
	}

	bool VMTHook::Release()
	{
		if (pClassVmt == nullptr) return false;

		*(std::uintptr_t**)ppClassBase = pClassVmt;

		std::memset(pNewVmt, NULL, vmtLenght * sizeof(std::uintptr_t) + (sizeof(std::uintptr_t) * (EXTRA_SPACE + 1)));

		pClassVmt = nullptr;
		pNewVmt = nullptr;
		ppClassBase = nullptr;
		vmtLenght = NULL;

		return true;
	}

	bool VMTHook::Setup(void* pclass)
	{
		if (pclass != nullptr)
			ppClassBase = reinterpret_cast<uintptr_t**>(pclass);

		pClassVmt = *ppClassBase;

		if (pClassVmt == nullptr) return false;

		MEMORY_BASIC_INFORMATION mem = { NULL };
		do
		{
			vmtLenght++;
			VirtualQuery(reinterpret_cast<LPCVOID>(pClassVmt[vmtLenght]), &mem, sizeof(mem));
		} while (mem.Protect == PAGE_EXECUTE_READ || mem.Protect == PAGE_EXECUTE_READWRITE);

		if (vmtLenght <= 0) return false;

		auto vmtSize = vmtLenght * sizeof(std::uintptr_t);

		HMODULE hModule = NULL;
		if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCSTR)pClassVmt, &hModule) == NULL) return false;

		char path[MAX_PATH];
		if (GetModuleFileNameA(hModule, path, sizeof(path)) == NULL) return false;

		std::string modulePath = path;
		if (modulePath.empty()) return false;

		std::size_t found = modulePath.find_last_of("/\\");
		modulePath.erase(0, found + 1);

		auto newVmtSizeExt = vmtSize + (sizeof(std::uintptr_t) * (EXTRA_SPACE + 1));
		pNewVmt = this->SearchFreeDataPage(modulePath.c_str(), newVmtSizeExt);
		if (pNewVmt == nullptr) return false;

#ifdef USE_VIRTUAL_PROTECT
		PDWORD old;
		VirtualProtect(pNewVmt, newVmtSizeExt, PAGE_READWRITE, (PDWORD)&old);
#endif

		std::memset(pNewVmt, NULL, newVmtSizeExt);
		std::memcpy(&pNewVmt[EXTRA_SPACE + 1], pClassVmt, vmtSize);
		pNewVmt[EXTRA_SPACE] = pClassVmt[-1];

#ifdef USE_VIRTUAL_PROTECT
		VirtualProtect(pNewVmt, newVmtSizeExt, (DWORD)old, (PDWORD)&old);
#endif

		try { *(std::uintptr_t**)ppClassBase = &pNewVmt[EXTRA_SPACE + 1]; }
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