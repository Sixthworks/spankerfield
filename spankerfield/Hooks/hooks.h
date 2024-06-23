#pragma once
#include "../SDK/sdk.h"

namespace big
{
	// For the refresh hook plugin
	namespace PreFrame
	{
		using PreFrameUpdate_t = void(*)(float dt);
		extern PreFrameUpdate_t oPreFrameUpdate;
		void hkPreFrame(float delta_time);
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