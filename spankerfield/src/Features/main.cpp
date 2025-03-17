#include "main.h"
#include "../global.h"
#include "../settings.h"

namespace big
{
    void features::draw()
    {
        plugins::refresh_preframe_hook(); // Unfortunately we have to do this
        plugins::draw_watermark();
        plugins::rainbow_mode();
        plugins::draw_screenshots();
        plugins::draw_crosshair();
        plugins::draw_spectators();
        plugins::draw_health_bar();
        plugins::draw_esp();

        if (g_settings.aimbot)
            plugins::draw_fov();

        plugins::draw_radar();
        plugins::draw_explosives();
        plugins::draw_missiles();
        plugins::update_friends_list();
        plugins::draw_blacklisted();
        plugins::infantry_alert();
    }

    void features::pre_frame(float delta_time)
    {
        plugins::spoof_name();
        plugins::unlock_all();
        plugins::anti_afk();
        plugins::c4_bot();
        plugins::no_hc_restrictions();
        plugins::sway_modify();
        plugins::overheat_control();
        plugins::kill_sound();

        if (g_settings.aimbot)
        {
            m_PlayerManager.update_players();
            plugins::aimbot(delta_time);
        }

        plugins::spot_minimap();
        plugins::set_jet_speed();
    }
}