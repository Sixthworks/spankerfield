#pragma once
#include "../common.h"

#define EXTRA_SPACE 3

namespace big
{
	class VMTHook
	{
	public:
		VMTHook()
			: ppClassBase(nullptr), pClassVmt(nullptr), pNewVmt(nullptr), vmtLenght(-1) {};
		VMTHook(void* pClassInstance)
			: ppClassBase(reinterpret_cast<uintptr_t**>(pClassInstance)), pClassVmt(nullptr),
			pNewVmt(nullptr), vmtLenght(-1) {};
		~VMTHook() { Release(); };

		void Hook(int index, void* fnPointer);
		bool Release();
		bool Setup(void* pclass = nullptr);

		template <typename T>
		T GetOriginal(int index) { return reinterpret_cast<T>(pClassVmt[index]); }
	private:
		std::uintptr_t* SearchFreeDataPage(const char* moduleName, const size_t vmtSize);
		std::uintptr_t** ppClassBase;
		std::uintptr_t* pClassVmt;
		std::uintptr_t* pNewVmt;
		std::size_t			vmtLenght;
	};
}