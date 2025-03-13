#pragma once
#include "../common.h"

namespace big
{
	class gui
	{
	public:
		void dx_init();
		void dx_on_tick();
	public:
		bool m_opened{};
	};

	inline gui g_gui;
}
