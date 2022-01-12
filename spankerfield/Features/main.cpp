#include "main.h"
#include "../Rendering/base.h"

namespace Features
{
	void Draw(bool FFPB)
	{
		if (!FFPB) return;

		ShowBlacklisted();
		DrawSpectators();
		DrawESP();
		DrawExplosives();
	}

	void PreFrame(bool FFPB)
	{
		Minimap(FFPB);
		SetJetSpeed();
	}
}