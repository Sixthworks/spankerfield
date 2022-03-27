#pragma once
#include "Minimap/minimap.h"
#include "Blacklist/blacklist.h"
#include "Spectator List/spectators.h"
#include "ESP/esp.h"
#include "Explosives/explosives.h"
#include "Jet Speed/jetspeed.h"
#include "Aimbot/aimbot.h"

namespace big
{
	class features
	{
	public:
		void draw();
		void pre_frame(float delta_time);
	};

	inline features* g_features{};
}