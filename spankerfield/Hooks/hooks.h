#pragma once
#include "../SDK/sdk.h"

namespace big
{
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