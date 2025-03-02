#include "watermark.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../Rendering/draw-list.h"

using namespace big;

namespace plugins
{
    // Debug flag for testing watermark
    static bool debug_watermark = false;

    // Random engine for watermark appearance
    static std::mt19937 rng(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count() & 0xFFFFFFFF));
    static std::uniform_int_distribution<int> position_choice(1, 3);
    static std::uniform_int_distribution<int> font_size(18, 22);
    static std::uniform_int_distribution<int> color_var(0, 30); // Color variation

    // Last time the watermark was shown and hidden
    static auto last_watermark_time = std::chrono::steady_clock::now();
    static auto next_watermark_time = std::chrono::steady_clock::now();
    static bool watermark_visible = false;

    // Store current watermark properties to avoid recalculating every frame
    static int current_x_pos = 0;
    static int current_y_pos = 0;
    static float current_size = 0.f;
    static ImColor current_color;
    static ImColor current_link_color;

    // Fixed positions (x and y percentages) to avoid interrupting gameplay
    static const std::pair<int, int> positions[] = {
        {5, 5}, // Top left
        {70, 5}, // Top right
        {70, 85} // Bottom right
    };

    // Helper function to toggle debug mode
    void toggle_watermark_debug(bool enable)
    {
        debug_watermark = enable;

        // Force watermark to be visible immediately when debug is enabled
        if (debug_watermark)
        {
            watermark_visible = true;

            // Generate new position and appearance for debug mode
            int pos_idx = position_choice(rng) - 1;
            int x_pct = positions[pos_idx].first;
            int y_pct = positions[pos_idx].second;

            current_x_pos = (g_globals.g_width * x_pct) / 100;
            current_y_pos = (g_globals.g_height * y_pct) / 100;
            current_size = static_cast<float>(font_size(rng));

            int r = 220 + color_var(rng) - 15;
            int g = 220 + color_var(rng) - 15;
            int b = 220 + color_var(rng) - 15;
            current_color = ImColor(r, g, b, 180);
            current_link_color = ImColor(180, 180, 220, 180);

            // Reset timer to keep it visible in debug mode
            next_watermark_time = std::chrono::steady_clock::now() + std::chrono::hours(24);
        }
        else
        {
            // Return to normal operation
            watermark_visible = false;
            next_watermark_time = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        }
    }

    void draw_watermark()
    {
        if (g_settings.disable_watermark) return;

        // Only process normal watermark timing logic if not in debug mode
        if (!debug_watermark)
        {
            // Check if it's time to change watermark state
            auto current_time = std::chrono::steady_clock::now();

            if (!watermark_visible && current_time > next_watermark_time)
            {
                // Generate new position and appearance when watermark becomes visible
                int pos_idx = position_choice(rng) - 1;
                int x_pct = positions[pos_idx].first;
                int y_pct = positions[pos_idx].second;

                current_x_pos = (g_globals.g_width * x_pct) / 100;
                current_y_pos = (g_globals.g_height * y_pct) / 100;
                current_size = static_cast<float>(font_size(rng));

                int r = 220 + color_var(rng) - 15;
                int g = 220 + color_var(rng) - 15;
                int b = 220 + color_var(rng) - 15;
                current_color = ImColor(r, g, b, 180);
                current_link_color = ImColor(180, 180, 220, 180);

                watermark_visible = true;
                last_watermark_time = current_time;

                // Display for 5-10 seconds
                std::uniform_int_distribution<int> display_time(5, 10);
                next_watermark_time = current_time + std::chrono::seconds(display_time(rng));
            }
            else if (watermark_visible && current_time > next_watermark_time)
            {
                watermark_visible = false;
                // Wait 10-30 minutes before showing again
                std::uniform_int_distribution<int> wait_time(600, 1800); // 600-1800 seconds (10-30 minutes)
                next_watermark_time = current_time + std::chrono::seconds(wait_time(rng));
            }
        }

        // Draw watermark if visible or in debug mode
        if (watermark_visible || debug_watermark)
        {
            m_drawing->AddText((float)current_x_pos, (float)current_y_pos, current_color, current_size, FL_NONE, xorstr_("Spankerfield"));

            m_drawing->AddText((float)current_x_pos, (float)current_y_pos + current_size + 2.f, current_link_color, current_size - 2.f, FL_NONE, xorstr_("nlog.us/donate"));

            if (debug_watermark)
                m_drawing->AddText((float)current_x_pos, (float)current_y_pos + current_size + current_size, ImColor(255, 50, 50, 200), current_size - 4.f, FL_NONE, xorstr_("DEBUG MODE"));
        }
    }
}