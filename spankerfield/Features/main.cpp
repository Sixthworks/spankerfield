#include "main.h"
#include "../global.h"

namespace big
{
	void features::draw()
	{
		bool draw = !g_globals.g_punkbuster && !g_globals.g_fairfight;
		if (!draw) return;

		plugins::draw_spectators();
		plugins::draw_esp();
		plugins::draw_radar();
		plugins::draw_explosives();
		plugins::draw_missiles();
		plugins::draw_fov();
		plugins::draw_blacklisted();
		plugins::infantry_alert();
	}

	void features::pre_frame(float delta_time)
	{
		m_PlayerManager.update_players();
		plugins::aimbot(delta_time);
		plugins::overheat_control();

		plugins::spot_minimap();
		plugins::set_jet_speed();
	}
}