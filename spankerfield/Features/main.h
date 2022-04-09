#pragma once
#include "Minimap/minimap.h"
#include "Blacklist/blacklist.h"
#include "Spectator List/spectators.h"
#include "ESP/esp.h"
#include "Radar/radar.h"
#include "Explosives/explosives.h"
#include "Missiles/missiles.h"
#include "Jet Speed/jetspeed.h"
#include "Aimbot/aimbot.h"
#include "Overheat/overheat.h"
#include "Infantry Alert/infantry_alert.h"

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