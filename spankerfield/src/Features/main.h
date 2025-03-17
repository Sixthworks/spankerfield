#pragma once
#include "Minimap/minimap.h"
#include "Blacklist/blacklist.h"
#include "Spectator List/spectators.h"
#include "ESP/esp.h"
#include "Crosshair/crosshair.h"
#include "Radar/radar.h"
#include "Explosives/explosives.h"
#include "Missiles/missiles.h"
#include "Jet Speed/jetspeed.h"
#include "Aimbot/aimbot.h"
#include "Risky/risky.h"
#include "Overheat/overheat.h"
#include "Infantry Alert/infantry_alert.h"
#include "Screenshots/screenshots.h"
#include "Health Bar/health_bar.h"
#include "Anti AFK/anti_afk.h"
#include "Name Spoofer/name_spoofer.h"
#include "Kill Sound/kill_sound.h"
#include "Refresh Hook/refresh_hook.h"
#include "Rainbow Mode/rainbow_mode.h"
#include "Watermark/watermark.h"
#include "Friend List/friend_list.h"
#include "C4 Bot/c4_bot.h"

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