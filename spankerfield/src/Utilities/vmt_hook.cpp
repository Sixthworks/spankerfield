#include "vmt_hook.h"

namespace big
{
    bool VMTHook::Setup(void* pclass)
    {
        ppClassBase = reinterpret_cast<uintptr_t**>(pclass);
        pClassVmt = reinterpret_cast<uintptr_t*>(*ppClassBase);

        // Get vmt length
        vmtLenght = 0;
        while (reinterpret_cast<void*>(pClassVmt[vmtLenght]) != nullptr)
        {
            vmtLenght++;
        }

        // Allocate new vmt
        pNewVmt = new uintptr_t[vmtLenght + EXTRA_SPACE];
        memcpy(pNewVmt, pClassVmt, sizeof(uintptr_t) * vmtLenght);

        DWORD old_protect;
        VirtualProtect(ppClassBase, sizeof(uintptr_t*), PAGE_READWRITE, &old_protect);
        *ppClassBase = pNewVmt;
        VirtualProtect(ppClassBase, sizeof(uintptr_t*), old_protect, &old_protect);

        return true;
    }

    void VMTHook::Hook(int index, void* fnPointer)
    {
        pNewVmt[index] = reinterpret_cast<uintptr_t>(fnPointer);
    }

    bool VMTHook::Release()
    {
        if (ppClassBase && pClassVmt && pNewVmt)
        {
			DWORD old_protect;
			VirtualProtect(*ppClassBase, sizeof(uintptr_t*) * vmtLenght, PAGE_READWRITE, &old_protect);
            *ppClassBase = pClassVmt;
			VirtualProtect(*ppClassBase, sizeof(uintptr_t*) * vmtLenght, old_protect, &old_protect);
            delete[] pNewVmt;
            return true;
        }
        return false;
    }
    bool VMTHook::hook_function(void* original_function, void* hook_function, size_t index)
    {
        DWORD old_protect;
        VirtualProtect(&pClassVmt[index], sizeof(uintptr_t), PAGE_READWRITE, &old_protect);

        m_pOriginalVmt = reinterpret_cast<uintptr_t*>(pClassVmt[index]);
        pClassVmt[index] = reinterpret_cast<uintptr_t>(hook_function);

        VirtualProtect(&pClassVmt[index], sizeof(uintptr_t), old_protect, &old_protect);

        return true;
    }

    void VMTHook::unhook()
    {
        if (m_pOriginalVmt)
        {
            DWORD old_protect;
            VirtualProtect(m_pOriginalVmt, sizeof(uintptr_t) * MAX_VMT_SIZE, PAGE_READWRITE, &old_protect);

            HMODULE module_handle;
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                (LPCSTR)m_pOriginalVmt,
                &module_handle))
            {
                // Handle module
            }

            // Fix type conversion
            *reinterpret_cast<uintptr_t**>(m_pOriginalVmt) = pClassVmt;

            VirtualProtect(m_pOriginalVmt, sizeof(uintptr_t) * MAX_VMT_SIZE, old_protect, &old_protect);
        }
    }

    class VmtHook
    {
    public:
        bool Setup(void** instance)
        {
            m_Instance = instance;

            if (*m_Instance == nullptr)
                return false;

            m_OldTable = reinterpret_cast<uintptr_t*>(*m_Instance);
            size_t tableLength = CalculateTableLength(m_OldTable);
            m_NewTable = new uintptr_t[tableLength];
            memcpy(m_NewTable, m_OldTable, tableLength * sizeof(uintptr_t));
            *m_Instance = reinterpret_cast<void*>(m_NewTable);

            return true;
        }

    private:
        size_t CalculateTableLength(uintptr_t* vmtTable)
        {
            size_t index = 0;
            while (vmtTable[index] != 0)
            {
                index++;
            }
            return index;
        }

        void** m_Instance = nullptr;  // Changed from void** to match Setup parameter
        uintptr_t* m_OldTable = nullptr;
        uintptr_t* m_NewTable = nullptr;
    };
}