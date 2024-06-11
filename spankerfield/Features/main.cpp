#include "main.h"
#include "../global.h"
#include "../settings.h"

namespace big
{
	void features::draw()
	{
		plugins::draw_spectators();
		plugins::draw_esp();

		if (g_settings.aimbot)
			plugins::draw_fov();

		plugins::draw_radar();
		plugins::draw_explosives();
		plugins::draw_missiles();
		plugins::draw_blacklisted();
		plugins::infantry_alert();
	}

	void features::pre_frame(float delta_time)
	{
		if (g_settings.aimbot)
		{
			m_PlayerManager.update_players();
			plugins::aimbot(delta_time);
		}

		plugins::overheat_control();

		plugins::spot_minimap();
		plugins::set_jet_speed();
	}
}