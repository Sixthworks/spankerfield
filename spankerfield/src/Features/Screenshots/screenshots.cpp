#include "screenshots.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Rendering/draw-list.h"
#include <chrono>

using namespace big;
namespace plugins
{
    // Track when screenshots were taken
    struct
    {
        int last_ff_count = 0;
        int last_pb_count = 0;
        std::chrono::time_point<std::chrono::steady_clock> last_screenshot_time;
        bool show_warning = false;
    } static s_screenshot_state;

    void draw_screenshots()
    {
        if (!g_settings.screenshots && !g_settings.screenshots_warn)
            return;

        // Check if screenshot counts have changed
        if (g_globals.screenshots_ff > s_screenshot_state.last_ff_count || g_globals.screenshots_pb > s_screenshot_state.last_pb_count)
        {
            // Update the counts
            s_screenshot_state.last_ff_count = g_globals.screenshots_ff;
            s_screenshot_state.last_pb_count = g_globals.screenshots_pb;

            // Set the time when screenshot was taken
            s_screenshot_state.last_screenshot_time = std::chrono::steady_clock::now();
            s_screenshot_state.show_warning = true;
        }

        // Show warning for 3 seconds after being screenshotted
        if (s_screenshot_state.show_warning)
        {
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - s_screenshot_state.last_screenshot_time).count();

            if (elapsed < 3000)
            {
                if (g_settings.screenshots_warn)
                {
                    // Alive status
                    bool is_alive = false;
                    
                    // Fetch alive status
                    const auto game_context = ClientGameContext::GetInstance();
                    if (game_context)
                    {
                        if (game_context->m_pPlayerManager && IsValidPtr(game_context->m_pPlayerManager->m_pLocalPlayer))
                        {
                            const auto local_soldier = game_context->m_pPlayerManager->m_pLocalPlayer->GetSoldier();
                            if (IsValidPtr(local_soldier))
                                is_alive = local_soldier->IsAlive();
                        }
                    }

                    float offset = is_alive ? 150.f : 30.f; // Above gadgets when alive, bottom of the screen when not

                    m_drawing->AddText((float)g_globals.g_width / 2.f, (float)g_globals.g_height - offset, ImColor(255, 0, 0, 255), 25.f, FL_CENTER_X | FL_SHADOW, xorstr_("YOU JUST GOT SCREENSHOTTED!"));
                }
            }
            else
                s_screenshot_state.show_warning = false;
        }

        if (g_settings.screenshots)
        {
            // Default Y offset (example, 1080 - 30 = 1050)
            float offset = 30.f;

            // Smart way of not duplicating the code
            auto draw_screenshot_text = [&](const std::string& text, int count)
                {
                    if (count >= 1)
                    {
                        std::string str = text + std::to_string(count);
                        m_drawing->AddText(12.f, (float)g_globals.g_height - offset, g_settings.screenshots_color, 25.f, FL_SHADOW, str.c_str());
                        offset += 22.5f;
                    }
                };

            // Draw FF screenshots
            draw_screenshot_text(xorstr_("FF screenshots: "), g_globals.screenshots_ff);

            // Draw PB screenshots
            draw_screenshot_text(xorstr_("PB screenshots: "), g_globals.screenshots_pb);
        }
    }
}