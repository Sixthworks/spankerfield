#include "main.h"
#include "../Rendering/base.h"

namespace features
{
	void draw(bool FFPB)
	{
		if (!FFPB) return;

		draw_blacklisted();
		draw_spectators();
		draw_esp();
		draw_explosives();
	}

	void pre_frame(bool FFPB)
	{
		spot_minimap(FFPB);
		set_jet_speed();
	}
}