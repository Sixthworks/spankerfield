#include "main.h"
#include "../global.h"

namespace big
{
	void features::draw()
	{
		if (!(!g_globals.g_punkbuster && !g_globals.g_fairfight)) return;

		plugins::draw_blacklisted();
		plugins::draw_spectators();
		plugins::draw_esp();
		plugins::draw_explosives();
	}

	void features::pre_frame()
	{
		plugins::spot_minimap();
		plugins::set_jet_speed();
	}
}