#pragma once
#include "../common.h"

#define EXTRA_SPACE 3
#define MAX_VMT_SIZE 1024

namespace big
{
	class VMTHook
	{
	public:
		VMTHook()
			: ppClassBase(nullptr), pClassVmt(nullptr), pNewVmt(nullptr), vmtLenght(-1), m_pOriginalVmt(nullptr) {};
		VMTHook(void* pClassInstance)
			: ppClassBase(reinterpret_cast<uintptr_t**>(pClassInstance)), pClassVmt(nullptr),
			pNewVmt(nullptr), vmtLenght(-1), m_pOriginalVmt(nullptr) {};
		~VMTHook() { Release(); };

		void Hook(int index, void* fnPointer);
		bool Release();
		bool Setup(void* pclass = nullptr);
		bool hook_function(void* original_function, void* hook_function, size_t index);
		void unhook();

		template <typename T>
		T GetOriginal(int index) { return reinterpret_cast<T>(pClassVmt[index]); }

	private:
		uintptr_t* SearchFreeDataPage(const char* moduleName, const size_t vmtSize);
		uintptr_t** ppClassBase;
		uintptr_t* pClassVmt;
		uintptr_t* pNewVmt;
		uintptr_t* m_pOriginalVmt;
		std::size_t vmtLenght;
	};
}