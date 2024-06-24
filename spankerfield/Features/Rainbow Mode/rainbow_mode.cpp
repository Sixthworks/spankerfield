#include "rainbow_mode.h"
#include "../../settings.h"
#include "../../Rendering/draw-list.h"

// Class
class RainbowColorManager
{
private:
    float hue = 0.0f;
    const float rainbow_speed = 0.5f;
    ImColor current_color;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_update_time;
    std::vector<std::reference_wrapper<ImColor>> managed_colors;

    void update_rainbow_color(float delta_time)
    {
        hue += rainbow_speed * delta_time;
        if (hue > 1.0f) hue -= 1.0f;

        float r, g, b;
        float h = hue * 6.0f;
        float c = 1.0f;
        float x = c * (1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f));
        float m = 0.0f;

        if (h < 1.0f) { r = c; g = x; b = 0; }
        else if (h < 2.0f) { r = x; g = c; b = 0; }
        else if (h < 3.0f) { r = 0; g = c; b = x; }
        else if (h < 4.0f) { r = 0; g = x; b = c; }
        else if (h < 5.0f) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        r += m; g += m; b += m;

        current_color = ImColor(
            static_cast<int>(r * 255),
            static_cast<int>(g * 255),
            static_cast<int>(b * 255),
            255
        );
    }

public:
    RainbowColorManager() : last_update_time(std::chrono::high_resolution_clock::now()) {}

    void add_color(ImColor& color)
    {
        managed_colors.push_back(color);
    }

    void update()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float>(current_time - last_update_time).count();
        last_update_time = current_time;

        update_rainbow_color(delta_time);
        for (auto& color_ref : managed_colors)
            color_ref.get() = current_color;
    }
};

// Code
RainbowColorManager g_rainbow_manager;

using namespace big;
namespace plugins
{
    static bool colors_initialized = false;
    void init_colors()
    {
        // These are the only ones that need to be changed in my opinion
        g_rainbow_manager.add_color(g_settings.aim_fov_color);
        g_rainbow_manager.add_color(g_settings.esp_box_color);
        g_rainbow_manager.add_color(g_settings.esp_box_color_occluded);
        g_rainbow_manager.add_color(g_settings.esp_line_color);
        g_rainbow_manager.add_color(g_settings.esp_line_color_occluded);
        g_rainbow_manager.add_color(g_settings.esp_teammate_color);
        g_rainbow_manager.add_color(g_settings.text_color_occluded);
        g_rainbow_manager.add_color(g_settings.text_color);
        g_rainbow_manager.add_color(g_settings.skeleton_color);
        g_rainbow_manager.add_color(g_settings.crosshair_color);
        g_rainbow_manager.add_color(g_settings.radar_outline_color);
        g_rainbow_manager.add_color(g_settings.radar_cross_color);
        g_rainbow_manager.add_color(g_settings.radar_enemies_color);
        g_rainbow_manager.add_color(g_settings.radar_enemy_vehicles_color);
        g_rainbow_manager.add_color(g_settings.blacklist_color);
        g_rainbow_manager.add_color(g_settings.infantry_alert_color);
        g_rainbow_manager.add_color(g_settings.health_bar_color);
        g_rainbow_manager.add_color(g_settings.explosives_color);
        g_rainbow_manager.add_color(g_settings.missiles_color);
        g_rainbow_manager.add_color(g_settings.spectator_color);
    }

	void rainbow_mode()
	{
        if (!g_settings.rainbow_mode) return;

        // Initialize them one time only
        if (!colors_initialized)
        {
            init_colors();

            colors_initialized = true;
        }

        // Update
        g_rainbow_manager.update();
	}
}