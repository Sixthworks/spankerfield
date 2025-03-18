#include "rainbow_mode.h"
#include "../../settings.h"
#include "../../Rendering/draw-list.h"

using namespace big;

// Class
class RainbowColorManager
{
private:
    float hue = 0.0f;
    ImColor current_color;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_update_time;
    std::vector<std::pair<std::reference_wrapper<ImColor>, ImColor>> managed_colors;
    bool is_active = false;

    void update_rainbow_color(float delta_time)
    {
        hue += g_settings.rainbow_speed * delta_time;
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

    void update_original_colors()
    {
        for (auto& [color_ref, original_color] : managed_colors)
            original_color = color_ref.get();
    }

    void restore_original_colors()
    {
        for (auto& [color_ref, original_color] : managed_colors)
            color_ref.get() = original_color;
    }
public:
    RainbowColorManager() : last_update_time(std::chrono::high_resolution_clock::now()) {}

    void add_color(ImColor& color)
    {
        managed_colors.emplace_back(color, color); // Store reference and original color
    }

    void update()
    {
        if (!is_active) return;

        auto current_time = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float>(current_time - last_update_time).count();
        last_update_time = current_time;

        update_rainbow_color(delta_time);
        for (auto& [color_ref, original_color] : managed_colors)
            color_ref.get() = current_color;
    }

    void activate()
    {
        update_original_colors();
        is_active = true;
    }

    void deactivate()
    {
        is_active = false;
        restore_original_colors();
    }
};

// Code
RainbowColorManager g_rainbow_manager;

namespace plugins
{
    static bool colors_initialized = false;
    void init_colors()
    {
        // These are the only ones that need to be changed in my opinion
        g_rainbow_manager.add_color(g_settings.aim_fov_color);
        g_rainbow_manager.add_color(g_settings.esp_box_color);
        g_rainbow_manager.add_color(g_settings.esp_box_color_occluded);
        g_rainbow_manager.add_color(g_settings.esp_3d_box_color);
        g_rainbow_manager.add_color(g_settings.esp_3d_box_color_occluded);
        g_rainbow_manager.add_color(g_settings.esp_eye_tracer_color);
        g_rainbow_manager.add_color(g_settings.esp_eye_tracer_color_occluded);
        g_rainbow_manager.add_color(g_settings.esp_line_color);
        g_rainbow_manager.add_color(g_settings.esp_line_color_occluded);
        g_rainbow_manager.add_color(g_settings.esp_teammate_color);
        g_rainbow_manager.add_color(g_settings.text_color_occluded);
        g_rainbow_manager.add_color(g_settings.text_color);
        g_rainbow_manager.add_color(g_settings.esp_additional_tags_color);
        g_rainbow_manager.add_color(g_settings.skeleton_color);
        g_rainbow_manager.add_color(g_settings.skeleton_color_occluded);
        g_rainbow_manager.add_color(g_settings.crosshair_color);
        g_rainbow_manager.add_color(g_settings.radar_outline_color);
        g_rainbow_manager.add_color(g_settings.radar_cross_color);
        g_rainbow_manager.add_color(g_settings.radar_you_color);
        g_rainbow_manager.add_color(g_settings.radar_teammates_color);
        g_rainbow_manager.add_color(g_settings.radar_teammate_vehicles_color);
        g_rainbow_manager.add_color(g_settings.radar_enemies_color);
        g_rainbow_manager.add_color(g_settings.radar_enemy_vehicles_color);
        g_rainbow_manager.add_color(g_settings.blacklist_color);
        g_rainbow_manager.add_color(g_settings.infantry_alert_color);
        g_rainbow_manager.add_color(g_settings.health_bar_color);
        g_rainbow_manager.add_color(g_settings.explosives_color);
        g_rainbow_manager.add_color(g_settings.missiles_color);
        g_rainbow_manager.add_color(g_settings.spectator_color);
        g_rainbow_manager.add_color(g_settings.spectator_warnings_color);
        g_rainbow_manager.add_color(g_settings.screenshots_color);
    }

    static bool was_active = false;
    void rainbow_mode()
    {
        if (g_settings.rainbow_mode)
        {
            if (!colors_initialized)
            {
                init_colors();
                colors_initialized = true;
            }

            if (!was_active)
            {
                g_rainbow_manager.activate();
                was_active = true;
            }

            g_rainbow_manager.update();
        }
        else if (was_active)
        {
            g_rainbow_manager.deactivate();
            was_active = false;
        }
    }
}