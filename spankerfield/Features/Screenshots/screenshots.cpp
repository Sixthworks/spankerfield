#include "screenshots.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Rendering/draw-list.h"

using namespace big;
namespace plugins
{
	void draw_screenshots()
	{
		if (!g_settings.screenshots) return;

		// So they can stack up
		float offset = 30.f;

		// Smart way of not duplicating the code
		auto draw_screenshot_text = [&](const std::string& text, int count)
		{
			if (count >= 1)
			{
				std::string str = text + std::to_string(count);
				m_drawing->AddText(12.5f, (float)g_globals.g_height - offset, g_settings.screenshots_color, 25.f, FL_NONE, str.c_str());
				offset += 22.5f;
			}
		};

		// Draw FF screenshots
		draw_screenshot_text(xorstr_("FairFight screenshots: "), g_globals.screenshots_ff);

		// Draw PB screenshots
		draw_screenshot_text(xorstr_("PunkBuster screenshots: "), g_globals.screenshots_pb);
	}
}