#pragma once
#include "../SDK/sdk.h"
#include "../Utilities/vmt_hook.h"

namespace big
{
	// For the refresh hook plugin
	namespace PreFrame
	{
		extern std::unique_ptr<VMTHook> pPreFrameHook;
		typedef int(__fastcall* PreFrameUpdate_t) (void*, void*, float);

		int __fastcall PreFrameUpdate(void* ecx, void* edx, float delta_time);
	}

	// Hooking
	class hooking
	{
	public:
		void initialize();
		void uninitialize();
		void enable();
		void disable();
	};

	inline hooking* g_hooking{};
}