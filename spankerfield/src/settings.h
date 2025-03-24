#include "global.h"
#include "SDK/sdk.h"
#include "Utilities/path.h"
#include <ImGui/imgui.h>

namespace big
{
	class settings
	{
	public:
		bool use_cheap_text; // DebugRenderer2 text instead of ImGui text
		bool use_cheap_boxes; // DebugRenderer2 boxes instead of ImGui

		bool blacklist{ true };
		ImColor blacklist_color{ 255, 0, 0, 255 };
		float blacklist_text_size{ 26.f };

		bool streamer_mode;
		bool rainbow_mode;
		float rainbow_speed{ 0.5f };

		bool spoof_name;
		bool spoof_restore;
		char original_name[16];
		char spoofed_name[16];

		bool aimbot;
		bool aim_support_controller;
		bool aim_must_be_visible{ true };
		bool aim_must_not_reload{ true };
		bool aim_fov_method{ true };
		bool aim_bone_priority;
		bool aim_draw_fov;
		float aim_fov{ 35.f };
		ImColor aim_fov_color{ 255, 255, 255, 200 };
		float aim_min_time_to_target{ 0.25f };
		float aim_max_time_to_target{ 0.7f };
		float aim_vehicle_smooth{ 12.5f };
		int aim_key{ VK_LMENU };
		int aim_bone{ UpdatePoseResultData::BONES::Spine1 };
		int aim_target_selection{ 1 };
		bool aim_ignore_friends{ true };
		bool aim_auto_bone{ true };
		float aim_zeroing_correction{ 0.5f };

		bool no_recoil;
		float recoil_decrease_scale{ 100.0f };
		float recoil_multiplier{ 0.0f };

		bool no_spread;
		float spread_control{ 0.0f };

		bool anti_afk{ true };
		int anti_afk_timer{ 200000 };

		bool unlock_all;
		bool no_hc_restrictions;

		bool overheat_control{ true };
		float overheat_control_critical{ 0.50f };

		bool infantry_alert{ true };
		bool infantry_alert_light_tech{ true };
		bool infantry_alert_indicators{ true };
		float infantry_alert_distance{ 22.5f };
		float infantry_alert_text_size{ 20.f };
		bool infantry_alert_use_default_pos{ true };
		float infantry_alert_x{ 30.f };
		float infantry_alert_y{ 400.f };
		ImColor infantry_alert_color{ 255, 255, 0, 255 };
		ImColor infantry_alert_indicator_color{ 255, 0, 0, 255 };

		bool esp{ true };
		bool esp_draw_friends{ true };
		bool esp_draw_teammates;
		bool esp_draw_vehicles{ true };
		float esp_distance{ 10000.f };
		bool esp_friend_color_to_tag;
		ImColor esp_teammate_color{ 63, 147, 216, 255 };
		ImColor esp_friend_color{ 80, 255, 80, 255 };
		ImColor esp_additional_tags_color{ 228, 213, 106, 255 };

		bool esp_draw_box{ false };
		int esp_box_style{ 4 };
		ImColor esp_box_color_occluded{ 255, 255, 255, 255 };
		ImColor esp_box_color{ 255, 0, 0, 255 };

		bool esp_box_fill;
		ImColor esp_box_fill_color{ 0, 0, 0, 70 };

		bool esp_draw_3d_box{ true };
		ImColor esp_3d_box_color_occluded{ 255, 255, 255, 255 };
		ImColor esp_3d_box_color{ 255, 0, 0, 255 };
		float esp_3d_box_thickness{ 1.0f };

		bool esp_draw_eye_tracer{ false };
		float esp_eye_tracer_distance{ 8.0f };
		float esp_eye_tracer_thickness{ 1.0f };
		ImColor esp_eye_tracer_color_occluded{ 255, 255, 255, 255 };
		ImColor esp_eye_tracer_color{ 255, 0, 0, 255 };

		bool esp_draw_line{ true };
		int esp_draw_line_from{ 1 };
		float esp_line_thickness{ 1.f };
		ImColor esp_line_color_occluded{ 220, 220, 220, 120 };
		ImColor esp_line_color{ 255, 255, 255, 120 };

		bool esp_draw_health{ true };
		int esp_health_location{ 1 };
		bool esp_draw_name{ false };
		bool esp_draw_distance{ false };
		bool esp_draw_vehicle_tag{ true };
		float esp_text_spacing{ 10.8f };
		int esp_text_position{ 0 };
		ImColor text_color_occluded{ 255, 255, 255, 255 };
		ImColor text_color{ 255, 0, 0, 255 };

		bool skeleton{ true };
		bool skeleton_use_dots;
		float skeleton_dots_distance{ 22.5f };
		ImColor skeleton_color_occluded{ 0, 0, 0, 255 };
		ImColor skeleton_color{ 255, 255, 0, 255 };

		bool esp_draw_aim_point;
		float esp_aim_point_size{ 3.5f };
		ImColor esp_aim_point_color{ 220, 220, 220, 255 };

		bool draw_crosshair{ true };
		bool crosshair_in_vehicles{ true };
		int crosshair_type{ 0 };
		bool crosshair_shadow{ true };
		float crosshair_size{ 20.f };
		float crosshair_thickness{ 1.f };
		ImColor crosshair_color{ 0, 0, 0, 255 };

		bool draw_health_bar;
		bool health_bar_soldier{ true };
		bool health_bar_vehicle{ true };
		float health_bar_width{ 285.f };
		float health_bar_height{ 38.f };
		float health_bar_x{ 20.f };
		float health_bar_y{ 20.f };
		bool health_bar_use_center_x{ true };
		float health_bar_spacing{ 15.f };
		bool health_bar_use_default_color{ true };
		ImColor health_bar_color{ 138, 107, 255, 255 };

		bool radar{ true };
		bool radar_circular;
		bool radar_draw_friends{ true };
		bool radar_draw_teammates{ true };
		bool radar_draw_you{ true };
		bool radar_cross{ true };
		bool radar_outline{ true };
		float radar_x{ 20.f };
		float radar_y{ 245.f };
		float radar_width{ 320.f };
		float radar_height{ 320.f };
		float radar_distance{ 150.f };
		ImColor radar_background_color{ 0, 0, 0, 160 };
		ImColor radar_you_color{ 255, 255, 255, 200 };
		ImColor radar_outline_color{ 0, 0, 0, 255 };
		ImColor radar_cross_color{ 55, 55, 55, 175 };
		ImColor radar_friends_color{ 80, 255, 80, 255 };
		ImColor radar_teammates_color{ 170, 170, 170, 255 };
		ImColor radar_enemies_color{ 215, 133, 30, 255 };
		ImColor radar_friend_vehicles_color{ 65, 255, 65, 255 };
		ImColor radar_teammate_vehicles_color{ 190, 190, 190, 255 };
		ImColor radar_enemy_vehicles_color{ 255, 97, 30, 255 };

		bool explosives{ true };
		ImColor explosives_color{ 255, 77, 77, 255 };

		bool c4_bot;
		bool c4_bot_auto_detonate{ true };
		bool c4_bot_auto_detonate_independently;
		bool c4_bot_always_active;
		int c4_bot_key{ VK_XBUTTON1 };
		bool c4_bot_ignore_friends{ true };
		float c4_bot_radius{ 5.0f };
		float c4_bot_damage_radius{ 4.35f };
		float c4_bot_lethal_radius{ 2.5f };
		float c4_bot_min_damage_to_enemy{ 70.f };
		bool c4_bot_prevent_self_damage{ true };
		bool c4_bot_smart_self_damage{ true };
		float c4_bot_health_buffer{ 35.0f };

		bool missiles_own{ true };
		ImColor missiles_color{ 14, 231, 231, 255 };

		bool jet_speed{ true };

		bool spectator_list{ true };
		bool spectator_raw_drawing;
		bool spectator_disable_esp_aim;
		bool spectator_warn_new{ true };
		float spectator_x{ 23.5f };
		float spectator_y{ 565.f };
		float spectator_warnings_size{ 28.f };
		ImColor spectator_color{ 255, 255, 255, 255 };
		ImColor spectator_warnings_color{ 255, 0, 0, 255 };

		bool kill_sound;
		char kill_sound_path[MAX_PATH];

		bool minimap{ true };
		bool obs_check{ true };

		bool screenshots;
		bool screenshots_warn{ true };
		bool screenshots_pb_temp_disable;
		bool screenshots_pb_clean{ true };
		bool screenshots_pb_use_both;
		bool screenshots_pb_save_to_folder;
		int screenshots_pb_clean_delay{ 20000 };
		int screenhots_pb_delay{ 300 };
		int screenhots_post_pb_delay{ 300 };
		ImColor screenshots_color{ 255, 255, 255, 255 };

		bool disable_watermark;
	};

	inline settings g_settings;

	struct config
	{
	public:
		nlohmann::json default_options;
		nlohmann::json options;
		std::string current_config = xorstr_("default");
		std::vector<std::string> available_configs;

		std::string color_to_string(ImColor color)
		{
			float colors[4];
			colors[0] = color.Value.x;
			colors[1] = color.Value.y;
			colors[2] = color.Value.z;
			colors[3] = color.Value.w;

			std::string format = fmt::format(xorstr_("X({}) Y({}) Z({}) W({})"), colors[0], colors[1], colors[2], colors[3]);
			return format;
		}

		ImVec4 string_to_color(std::string color)
		{
			float x, y, z, w;
			sscanf_s(color.c_str(), xorstr_("X(%f) Y(%f) Z(%f) W(%f)"), &x, &y, &z, &w);

			return ImVec4(x, y, z, w);
		}

		void from_json(const nlohmann::json& j)
		{
			g_settings.use_cheap_text = j[xorstr_("settings")][xorstr_("use_cheap_text")];
			g_settings.use_cheap_boxes = j[xorstr_("settings")][xorstr_("use_cheap_boxes")];

			g_settings.blacklist = j[xorstr_("settings")][xorstr_("blacklist")];
			g_settings.blacklist_color = string_to_color(j[xorstr_("settings")][xorstr_("blacklist_color")]);
			g_settings.blacklist_text_size = j[xorstr_("settings")][xorstr_("blacklist_text_size")];

			g_settings.streamer_mode = j[xorstr_("settings")][xorstr_("streamer_mode")];
			g_settings.rainbow_mode = j[xorstr_("settings")][xorstr_("rainbow_mode")];
			g_settings.rainbow_speed = j[xorstr_("settings")][xorstr_("rainbow_speed")];

			g_settings.aimbot = j[xorstr_("settings")][xorstr_("aimbot")];
			g_settings.aim_support_controller = j[xorstr_("settings")][xorstr_("aim_support_controller")];
			g_settings.aim_must_be_visible = j[xorstr_("settings")][xorstr_("aim_must_be_visible")];
			g_settings.aim_must_not_reload = j[xorstr_("settings")][xorstr_("aim_must_not_reload")];
			g_settings.aim_bone_priority = j[xorstr_("settings")][xorstr_("aim_bone_priority")];
			g_settings.aim_fov_method = j[xorstr_("settings")][xorstr_("aim_fov_method")];
			g_settings.aim_draw_fov = j[xorstr_("settings")][xorstr_("aim_draw_fov")];
			g_settings.aim_fov = j[xorstr_("settings")][xorstr_("aim_fov")];
			g_settings.aim_fov_color = string_to_color(j[xorstr_("settings")][xorstr_("aim_fov_color")]);
			g_settings.aim_min_time_to_target = j[xorstr_("settings")][xorstr_("aim_min_time_to_target")];
			g_settings.aim_max_time_to_target = j[xorstr_("settings")][xorstr_("aim_max_time_to_target")];
			g_settings.aim_vehicle_smooth = j[xorstr_("settings")][xorstr_("aim_vehicle_smooth")];
			g_settings.aim_key = j[xorstr_("settings")][xorstr_("aim_key")];
			g_settings.aim_bone = j[xorstr_("settings")][xorstr_("aim_bone")];
			g_settings.aim_target_selection = j[xorstr_("settings")][xorstr_("aim_target_selection")];
			g_settings.aim_ignore_friends = j[xorstr_("settings")][xorstr_("aim_ignore_friends")];
			g_settings.aim_auto_bone = j[xorstr_("settings")][xorstr_("aim_auto_bone")];
			g_settings.aim_zeroing_correction = j[xorstr_("settings")][xorstr_("aim_zeroing_correction")];	

			g_settings.no_recoil = j[xorstr_("settings")][xorstr_("no_recoil")];
			g_settings.no_spread = j[xorstr_("settings")][xorstr_("no_spread")];

			g_settings.recoil_multiplier = j[xorstr_("settings")][xorstr_("recoil_multiplier")];
			g_settings.recoil_decrease_scale = j[xorstr_("settings")][xorstr_("recoil_decrease_scale")];

			g_settings.spread_control = j[xorstr_("settings")][xorstr_("spread_control")];

			g_settings.anti_afk = j[xorstr_("settings")][xorstr_("anti_afk")];
			g_settings.anti_afk_timer = j[xorstr_("settings")][xorstr_("anti_afk_timer")];

			g_settings.unlock_all = j[xorstr_("settings")][xorstr_("unlock_all")];
			g_settings.no_hc_restrictions = j[xorstr_("settings")][xorstr_("no_hc_restrictions")];

			g_settings.overheat_control = j[xorstr_("settings")][xorstr_("overheat_control")];
			g_settings.overheat_control_critical = j[xorstr_("settings")][xorstr_("overheat_control_critical")];

			g_settings.infantry_alert = j[xorstr_("settings")][xorstr_("infantry_alert")];
			g_settings.infantry_alert_light_tech = j[xorstr_("settings")][xorstr_("infantry_alert_light_tech")];
			g_settings.infantry_alert_indicators = j[xorstr_("settings")][xorstr_("infantry_alert_indicators")];
			g_settings.infantry_alert_distance = j[xorstr_("settings")][xorstr_("infantry_alert_distance")];
			g_settings.infantry_alert_color = string_to_color(j[xorstr_("settings")][xorstr_("infantry_alert_color")]);
			g_settings.infantry_alert_indicator_color = string_to_color(j[xorstr_("settings")][xorstr_("infantry_alert_indicator_color")]);
			g_settings.infantry_alert_text_size = j[xorstr_("settings")][xorstr_("infantry_alert_text_size")];
			g_settings.infantry_alert_use_default_pos = j[xorstr_("settings")][xorstr_("infantry_alert_use_default_pos")];
			g_settings.infantry_alert_x = j[xorstr_("settings")][xorstr_("infantry_alert_x")];
			g_settings.infantry_alert_y = j[xorstr_("settings")][xorstr_("infantry_alert_y")];

			g_settings.esp = j[xorstr_("settings")][xorstr_("esp")];
			g_settings.esp_draw_friends = j[xorstr_("settings")][xorstr_("esp_draw_friends")];
			g_settings.esp_draw_teammates = j[xorstr_("settings")][xorstr_("esp_draw_teammates")];
			g_settings.esp_draw_vehicles = j[xorstr_("settings")][xorstr_("esp_draw_vehicles")];
			g_settings.esp_distance = j[xorstr_("settings")][xorstr_("esp_distance")];
			g_settings.esp_friend_color_to_tag = j[xorstr_("settings")][xorstr_("esp_friend_color_to_tag")];
			g_settings.esp_teammate_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_teammate_color")]);
			g_settings.esp_friend_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_friend_color")]);
			g_settings.esp_additional_tags_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_additional_tags_color")]);

			g_settings.esp_draw_box = j[xorstr_("settings")][xorstr_("esp_draw_box")];
			g_settings.esp_box_style = j[xorstr_("settings")][xorstr_("esp_box_style")];
			g_settings.esp_box_color_occluded = string_to_color(j[xorstr_("settings")][xorstr_("esp_box_color_occluded")]);
			g_settings.esp_box_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_box_color")]);

			g_settings.esp_box_fill = j[xorstr_("settings")][xorstr_("esp_box_fill")];
			g_settings.esp_box_fill_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_box_fill_color")]);

			g_settings.esp_draw_3d_box = j[xorstr_("settings")][xorstr_("esp_draw_3d_box")];
			g_settings.esp_3d_box_color_occluded = string_to_color(j[xorstr_("settings")][xorstr_("esp_3d_box_color_occluded")]);
			g_settings.esp_3d_box_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_3d_box_color")]);
			g_settings.esp_3d_box_thickness = j[xorstr_("settings")][xorstr_("esp_3d_box_thickness")];

			g_settings.esp_draw_eye_tracer = j[xorstr_("settings")][xorstr_("esp_draw_eye_tracer")];
			g_settings.esp_eye_tracer_distance = j[xorstr_("settings")][xorstr_("esp_eye_tracer_distance")];
			g_settings.esp_eye_tracer_thickness = j[xorstr_("settings")][xorstr_("esp_eye_tracer_thickness")];
			g_settings.esp_eye_tracer_color_occluded = string_to_color(j[xorstr_("settings")][xorstr_("esp_eye_tracer_color_occluded")]);
			g_settings.esp_eye_tracer_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_eye_tracer_color")]);

			g_settings.esp_draw_line = j[xorstr_("settings")][xorstr_("esp_draw_line")];
			g_settings.esp_draw_line_from = j[xorstr_("settings")][xorstr_("esp_draw_line_from")];
			g_settings.esp_line_thickness = j[xorstr_("settings")][xorstr_("esp_line_thickness")];
			g_settings.esp_line_color_occluded = string_to_color(j[xorstr_("settings")][xorstr_("esp_line_color_occluded")]);
			g_settings.esp_line_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_line_color")]);

			g_settings.esp_draw_health = j[xorstr_("settings")][xorstr_("esp_draw_health")];
			g_settings.esp_health_location = j[xorstr_("settings")][xorstr_("esp_health_location")];
			g_settings.esp_draw_name = j[xorstr_("settings")][xorstr_("esp_draw_name")];
			g_settings.esp_draw_distance = j[xorstr_("settings")][xorstr_("esp_draw_distance")];
			g_settings.esp_draw_vehicle_tag = j[xorstr_("settings")][xorstr_("esp_draw_vehicle_tag")];
			g_settings.esp_text_spacing = j[xorstr_("settings")][xorstr_("esp_text_spacing")];
			g_settings.esp_text_position = j[xorstr_("settings")][xorstr_("esp_text_position")];

			g_settings.text_color_occluded = string_to_color(j["settings"]["text_color_occluded"]);
			g_settings.text_color = string_to_color(j["settings"]["text_color"]);

			g_settings.skeleton = j[xorstr_("settings")][xorstr_("skeleton")];
			g_settings.skeleton_use_dots = j[xorstr_("settings")][xorstr_("skeleton_use_dots")];
			g_settings.skeleton_dots_distance = j[xorstr_("settings")][xorstr_("skeleton_dots_distance")];
			g_settings.skeleton_color_occluded = string_to_color(j[xorstr_("settings")][xorstr_("skeleton_color_occluded")]);
			g_settings.skeleton_color = string_to_color(j[xorstr_("settings")][xorstr_("skeleton_color")]);

			g_settings.esp_draw_aim_point = j[xorstr_("settings")][xorstr_("esp_draw_aim_point")];
			g_settings.esp_aim_point_size = j[xorstr_("settings")][xorstr_("esp_aim_point_size")];
			g_settings.esp_aim_point_color = string_to_color(j[xorstr_("settings")][xorstr_("esp_aim_point_color")]);

			g_settings.draw_crosshair = j[xorstr_("settings")][xorstr_("draw_crosshair")];
			g_settings.crosshair_in_vehicles = j[xorstr_("settings")][xorstr_("crosshair_in_vehicles")];
			g_settings.crosshair_type = j[xorstr_("settings")][xorstr_("crosshair_type")];
			g_settings.crosshair_shadow = j[xorstr_("settings")][xorstr_("crosshair_shadow")];
			g_settings.crosshair_size = j[xorstr_("settings")][xorstr_("crosshair_size")];
			g_settings.crosshair_thickness = j[xorstr_("settings")][xorstr_("crosshair_thickness")];
			g_settings.crosshair_color = string_to_color(j[xorstr_("settings")][xorstr_("crosshair_color")]);

			g_settings.draw_health_bar = j[xorstr_("settings")][xorstr_("draw_health_bar")];
			g_settings.health_bar_soldier = j[xorstr_("settings")][xorstr_("health_bar_soldier")];
			g_settings.health_bar_vehicle = j[xorstr_("settings")][xorstr_("health_bar_vehicle")];
			g_settings.health_bar_width = j[xorstr_("settings")][xorstr_("health_bar_width")];
			g_settings.health_bar_height = j[xorstr_("settings")][xorstr_("health_bar_height")];
			g_settings.health_bar_x = j[xorstr_("settings")][xorstr_("health_bar_x")];
			g_settings.health_bar_y = j[xorstr_("settings")][xorstr_("health_bar_y")];
			g_settings.health_bar_use_center_x = j[xorstr_("settings")][xorstr_("health_bar_use_center_x")];
			g_settings.health_bar_spacing = j[xorstr_("settings")][xorstr_("health_bar_spacing")];
			g_settings.health_bar_use_default_color = j[xorstr_("settings")][xorstr_("health_bar_use_default_color")];
			g_settings.health_bar_color = string_to_color(j[xorstr_("settings")][xorstr_("health_bar_color")]);

			g_settings.radar = j[xorstr_("settings")][xorstr_("radar")];
			g_settings.radar_circular = j[xorstr_("settings")][xorstr_("radar_circular")];
			g_settings.radar_draw_friends = j[xorstr_("settings")][xorstr_("radar_draw_friends")];
			g_settings.radar_draw_teammates = j[xorstr_("settings")][xorstr_("radar_draw_teammates")];
			g_settings.radar_draw_you = j[xorstr_("settings")][xorstr_("radar_draw_you")];
			g_settings.radar_cross = j[xorstr_("settings")][xorstr_("radar_cross")];
			g_settings.radar_outline = j[xorstr_("settings")][xorstr_("radar_outline")];
			g_settings.radar_x = j[xorstr_("settings")][xorstr_("radar_x")];
			g_settings.radar_y = j[xorstr_("settings")][xorstr_("radar_y")];
			g_settings.radar_width = j[xorstr_("settings")][xorstr_("radar_width")];
			g_settings.radar_height = j[xorstr_("settings")][xorstr_("radar_height")];
			g_settings.radar_distance = j[xorstr_("settings")][xorstr_("radar_distance")];
			g_settings.radar_background_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_background_color")]);
			g_settings.radar_you_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_you_color")]);
			g_settings.radar_outline_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_outline_color")]);
			g_settings.radar_cross_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_cross_color")]);
			g_settings.radar_friends_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_friends_color")]);
			g_settings.radar_teammates_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_teammates_color")]);
			g_settings.radar_enemies_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_enemies_color")]);
			g_settings.radar_friend_vehicles_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_friend_vehicles_color")]);
			g_settings.radar_teammate_vehicles_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_teammate_vehicles_color")]);
			g_settings.radar_enemy_vehicles_color = string_to_color(j[xorstr_("settings")][xorstr_("radar_enemy_vehicles_color")]);

			g_settings.explosives = j[xorstr_("settings")][xorstr_("explosives")];
			g_settings.explosives_color = string_to_color(j[xorstr_("settings")][xorstr_("explosives_color")]);

			g_settings.c4_bot = j[xorstr_("settings")][xorstr_("c4_bot")];
			g_settings.c4_bot_auto_detonate = j[xorstr_("settings")][xorstr_("c4_bot_auto_detonate")];
			g_settings.c4_bot_auto_detonate_independently = j[xorstr_("settings")][xorstr_("c4_bot_auto_detonate_independently")];
			g_settings.c4_bot_always_active = j[xorstr_("settings")][xorstr_("c4_bot_always_active")];
			g_settings.c4_bot_key = j[xorstr_("settings")][xorstr_("c4_bot_key")];
			g_settings.c4_bot_ignore_friends = j[xorstr_("settings")][xorstr_("c4_bot_ignore_friends")];
			g_settings.c4_bot_radius = j[xorstr_("settings")][xorstr_("c4_bot_radius")];
			g_settings.c4_bot_damage_radius = j[xorstr_("settings")][xorstr_("c4_bot_damage_radius")];
			g_settings.c4_bot_lethal_radius = j[xorstr_("settings")][xorstr_("c4_bot_lethal_radius")];
			g_settings.c4_bot_min_damage_to_enemy = j[xorstr_("settings")][xorstr_("c4_bot_min_damage_to_enemy")];
			g_settings.c4_bot_prevent_self_damage = j[xorstr_("settings")][xorstr_("c4_bot_prevent_self_damage")];
			g_settings.c4_bot_smart_self_damage = j[xorstr_("settings")][xorstr_("c4_bot_smart_self_damage")];
			g_settings.c4_bot_health_buffer = j[xorstr_("settings")][xorstr_("c4_bot_health_buffer")];

			g_settings.missiles_own = j[xorstr_("settings")][xorstr_("missiles_own")];
			g_settings.missiles_color = string_to_color(j[xorstr_("settings")][xorstr_("missiles_color")]);

			g_settings.jet_speed = j["settings"]["jet_speed"];

			g_settings.spectator_list = j[xorstr_("settings")][xorstr_("spectator_list")];
			g_settings.spectator_raw_drawing = j[xorstr_("settings")][xorstr_("spectator_raw_drawing")];
			g_settings.spectator_disable_esp_aim = j[xorstr_("settings")][xorstr_("spectator_disable_esp_aim")];
			g_settings.spectator_warn_new = j[xorstr_("settings")][xorstr_("spectator_warn_new")];
			g_settings.spectator_x = j[xorstr_("settings")][xorstr_("spectator_x")];
			g_settings.spectator_y = j[xorstr_("settings")][xorstr_("spectator_y")];
			g_settings.spectator_warnings_size = j[xorstr_("settings")][xorstr_("spectator_warnings_size")];
			g_settings.spectator_color = string_to_color(j[xorstr_("settings")][xorstr_("spectator_color")]);
			g_settings.spectator_warnings_color = string_to_color(j[xorstr_("settings")][xorstr_("spectator_warnings_color")]);

			g_settings.kill_sound = j[xorstr_("settings")][xorstr_("kill_sound")];
			std::string sound_path_str = j["settings"]["kill_sound_path"].get<std::string>();
			strncpy_s(g_settings.kill_sound_path, sizeof(g_settings.kill_sound_path), sound_path_str.c_str(), MAX_PATH - 1);
			g_settings.kill_sound_path[sound_path_str.size()] = '\0';

			g_settings.minimap = j[xorstr_("settings")][xorstr_("minimap")];
			g_settings.obs_check = j[xorstr_("settings")][xorstr_("obs_check")];
			g_settings.screenshots = j[xorstr_("settings")][xorstr_("screenshots")];
			g_settings.screenshots_warn = j[xorstr_("settings")][xorstr_("screenshots_warn")];
			g_settings.screenshots_color = string_to_color(j[xorstr_("settings")][xorstr_("screenshots_color")]);
			g_settings.screenshots_pb_temp_disable = j[xorstr_("settings")][xorstr_("screenshots_pb_temp_disable")];
			g_settings.screenshots_pb_clean = j[xorstr_("settings")][xorstr_("screenshots_pb_clean")];
			g_settings.screenshots_pb_use_both = j[xorstr_("settings")][xorstr_("screenshots_pb_use_both")];
			g_settings.screenshots_pb_save_to_folder = j[xorstr_("settings")][xorstr_("screenshots_pb_save_to_folder")];
			g_settings.screenshots_pb_clean_delay = j[xorstr_("settings")][xorstr_("screenshots_pb_clean_delay")];
			g_settings.screenhots_pb_delay = j[xorstr_("settings")][xorstr_("screenhots_pb_delay")];
			g_settings.screenhots_post_pb_delay = j[xorstr_("settings")][xorstr_("screenhots_post_pb_delay")];

			g_settings.disable_watermark = j[xorstr_("settings")][xorstr_("disable_watermark")];
			g_globals.open_key = j[xorstr_("settings")][xorstr_("open_key")];
		}

		nlohmann::json to_json()
		{
			return nlohmann::json{
				{
					xorstr_("settings"),
					{
			            { xorstr_("use_cheap_text"), g_settings.use_cheap_text },
						{ xorstr_("use_cheap_boxes"), g_settings.use_cheap_boxes },
						{ xorstr_("blacklist"), g_settings.blacklist },
						{ xorstr_("blacklist_color"), color_to_string(g_settings.blacklist_color) },
						{ xorstr_("blacklist_text_size"), g_settings.blacklist_text_size },
						{ xorstr_("streamer_mode"), g_settings.streamer_mode },
						{ xorstr_("rainbow_mode"), g_settings.rainbow_mode },
						{ xorstr_("rainbow_speed"), g_settings.rainbow_speed },
						{ xorstr_("esp"), g_settings.esp },
						{ xorstr_("esp_draw_friends"), g_settings.esp_draw_friends },
						{ xorstr_("esp_draw_teammates"), g_settings.esp_draw_teammates },
						{ xorstr_("esp_draw_vehicles"), g_settings.esp_draw_vehicles },
						{ xorstr_("esp_distance"), g_settings.esp_distance },
						{ xorstr_("esp_friend_color_to_tag"), g_settings.esp_friend_color_to_tag },
						{ xorstr_("esp_teammate_color"), color_to_string(g_settings.esp_teammate_color) },
						{ xorstr_("esp_friend_color"), color_to_string(g_settings.esp_friend_color) },
						{ xorstr_("esp_additional_tags_color"), color_to_string(g_settings.esp_additional_tags_color) },
						{ xorstr_("esp_draw_box"), g_settings.esp_draw_box },
						{ xorstr_("esp_box_style"), g_settings.esp_box_style },
						{ xorstr_("esp_box_color_occluded"), color_to_string(g_settings.esp_box_color_occluded) },
						{ xorstr_("esp_box_color"), color_to_string(g_settings.esp_box_color) },
						{ xorstr_("esp_box_fill"), g_settings.esp_box_fill },
						{ xorstr_("esp_box_fill_color"), color_to_string(g_settings.esp_box_fill_color) },
						{ xorstr_("esp_draw_3d_box"), g_settings.esp_draw_3d_box },
						{ xorstr_("esp_3d_box_color_occluded"), color_to_string(g_settings.esp_3d_box_color_occluded) },
						{ xorstr_("esp_3d_box_color"), color_to_string(g_settings.esp_3d_box_color) },
						{ xorstr_("esp_3d_box_thickness"), g_settings.esp_3d_box_thickness },
						{ xorstr_("esp_draw_eye_tracer"), g_settings.esp_draw_eye_tracer },
						{ xorstr_("esp_eye_tracer_distance"), g_settings.esp_eye_tracer_distance },
						{ xorstr_("esp_eye_tracer_thickness"), g_settings.esp_eye_tracer_thickness },
						{ xorstr_("esp_eye_tracer_color_occluded"), color_to_string(g_settings.esp_eye_tracer_color_occluded) },
						{ xorstr_("esp_eye_tracer_color"), color_to_string(g_settings.esp_eye_tracer_color) },
						{ xorstr_("esp_draw_line"), g_settings.esp_draw_line },
						{ xorstr_("esp_draw_line_from"), g_settings.esp_draw_line_from },
						{ xorstr_("esp_line_thickness"), g_settings.esp_line_thickness },
						{ xorstr_("esp_line_color_occluded"), color_to_string(g_settings.esp_line_color_occluded) },
						{ xorstr_("esp_line_color"), color_to_string(g_settings.esp_line_color) },
						{ xorstr_("esp_draw_health"), g_settings.esp_draw_health },
						{ xorstr_("esp_health_location"), g_settings.esp_health_location },
						{ xorstr_("esp_draw_name"), g_settings.esp_draw_name },
						{ xorstr_("esp_draw_distance"), g_settings.esp_draw_distance },
						{ xorstr_("esp_draw_vehicle_tag"), g_settings.esp_draw_vehicle_tag },
						{ xorstr_("esp_text_spacing"), g_settings.esp_text_spacing },
						{ xorstr_("esp_text_position"), g_settings.esp_text_position },
						{ xorstr_("text_color_occluded"), color_to_string(g_settings.text_color_occluded) },
						{ xorstr_("text_color"), color_to_string(g_settings.text_color) },
						{ xorstr_("skeleton"), g_settings.skeleton },
						{ xorstr_("skeleton_use_dots"), g_settings.skeleton_use_dots },
						{ xorstr_("skeleton_dots_distance"), g_settings.skeleton_dots_distance },
						{ xorstr_("skeleton_color_occluded"), color_to_string(g_settings.skeleton_color_occluded) },
						{ xorstr_("skeleton_color"), color_to_string(g_settings.skeleton_color) },
						{ xorstr_("esp_draw_aim_point"), g_settings.esp_draw_aim_point },
						{ xorstr_("esp_aim_point_size"), g_settings.esp_aim_point_size },
						{ xorstr_("esp_aim_point_color"), color_to_string(g_settings.esp_aim_point_color) },
						{ xorstr_("draw_crosshair"), g_settings.draw_crosshair },
						{ xorstr_("crosshair_in_vehicles"), g_settings.crosshair_in_vehicles },
						{ xorstr_("crosshair_type"), g_settings.crosshair_type },
						{ xorstr_("crosshair_shadow"), g_settings.crosshair_shadow },
						{ xorstr_("crosshair_size"), g_settings.crosshair_size },
						{ xorstr_("crosshair_thickness"), g_settings.crosshair_thickness },
						{ xorstr_("crosshair_color"), color_to_string(g_settings.crosshair_color) },
						{ xorstr_("draw_health_bar"), g_settings.draw_health_bar },
						{ xorstr_("health_bar_soldier"), g_settings.health_bar_soldier },
						{ xorstr_("health_bar_vehicle"), g_settings.health_bar_vehicle },
						{ xorstr_("health_bar_width"), g_settings.health_bar_width },
						{ xorstr_("health_bar_height"), g_settings.health_bar_height },
						{ xorstr_("health_bar_x"), g_settings.health_bar_x },
						{ xorstr_("health_bar_y"), g_settings.health_bar_y },
						{ xorstr_("health_bar_use_center_x"), g_settings.health_bar_use_center_x },
						{ xorstr_("health_bar_spacing"), g_settings.health_bar_spacing },
						{ xorstr_("health_bar_use_default_color"), g_settings.health_bar_use_default_color },
						{ xorstr_("health_bar_color"), color_to_string(g_settings.health_bar_color) },
						{ xorstr_("aimbot"), g_settings.aimbot },
						{ xorstr_("aim_support_controller"), g_settings.aim_support_controller },
						{ xorstr_("aim_must_be_visible"), g_settings.aim_must_be_visible },
						{ xorstr_("aim_must_not_reload"), g_settings.aim_must_not_reload },
						{ xorstr_("aim_fov_method"), g_settings.aim_fov_method },
						{ xorstr_("aim_bone_priority"), g_settings.aim_bone_priority },
						{ xorstr_("aim_draw_fov"), g_settings.aim_draw_fov },
						{ xorstr_("aim_fov"), g_settings.aim_fov },
						{ xorstr_("aim_fov_color"), color_to_string(g_settings.aim_fov_color) },
						{ xorstr_("aim_min_time_to_target"), g_settings.aim_min_time_to_target },
						{ xorstr_("aim_max_time_to_target"), g_settings.aim_max_time_to_target },
						{ xorstr_("aim_vehicle_smooth"), g_settings.aim_vehicle_smooth },
						{ xorstr_("aim_key"), g_settings.aim_key },
						{ xorstr_("aim_bone"), g_settings.aim_bone },
						{ xorstr_("aim_target_selection"), g_settings.aim_target_selection },
						{ xorstr_("aim_ignore_friends"), g_settings.aim_ignore_friends },
						{ xorstr_("aim_auto_bone"), g_settings.aim_auto_bone },
						{ xorstr_("aim_zeroing_correction"), g_settings.aim_zeroing_correction },
						{ xorstr_("no_recoil"), g_settings.no_recoil },
						{ xorstr_("no_spread"), g_settings.no_spread },
						{ xorstr_("recoil_multiplier"), g_settings.recoil_multiplier },
						{ xorstr_("recoil_decrease_scale"), g_settings.recoil_decrease_scale },
						{ xorstr_("spread_control"), g_settings.spread_control },
						{ xorstr_("anti_afk"), g_settings.anti_afk },
						{ xorstr_("anti_afk_timer"), g_settings.anti_afk_timer },
						{ xorstr_("unlock_all"), g_settings.unlock_all },
						{ xorstr_("no_hc_restrictions"), g_settings.no_hc_restrictions },
						{ xorstr_("overheat_control"), g_settings.overheat_control },
						{ xorstr_("overheat_control_critical"), g_settings.overheat_control_critical },
						{ xorstr_("infantry_alert"), g_settings.infantry_alert },
						{ xorstr_("infantry_alert_light_tech"), g_settings.infantry_alert_light_tech },
						{ xorstr_("infantry_alert_indicators"), g_settings.infantry_alert_indicators },
						{ xorstr_("infantry_alert_distance"), g_settings.infantry_alert_distance },
						{ xorstr_("infantry_alert_color"), color_to_string(g_settings.infantry_alert_color) },
						{ xorstr_("infantry_alert_indicator_color"), color_to_string(g_settings.infantry_alert_indicator_color) },
						{ xorstr_("infantry_alert_text_size"), g_settings.infantry_alert_text_size },
						{ xorstr_("infantry_alert_use_default_pos"), g_settings.infantry_alert_use_default_pos },
						{ xorstr_("infantry_alert_x"), g_settings.infantry_alert_x },
						{ xorstr_("infantry_alert_y"), g_settings.infantry_alert_y },
						{ xorstr_("radar"), g_settings.radar },
						{ xorstr_("radar_circular"), g_settings.radar_circular },
						{ xorstr_("radar_draw_friends"), g_settings.radar_draw_friends },
						{ xorstr_("radar_draw_teammates"), g_settings.radar_draw_teammates },
						{ xorstr_("radar_draw_you"), g_settings.radar_draw_you },
						{ xorstr_("radar_cross"), g_settings.radar_cross },
						{ xorstr_("radar_outline"), g_settings.radar_outline },
						{ xorstr_("radar_x"), g_settings.radar_x },
						{ xorstr_("radar_y"), g_settings.radar_y },
						{ xorstr_("radar_width"), g_settings.radar_width },
						{ xorstr_("radar_height"), g_settings.radar_height },
						{ xorstr_("radar_distance"), g_settings.radar_distance },
						{ xorstr_("radar_background_color"), color_to_string(g_settings.radar_background_color) },
						{ xorstr_("radar_you_color"), color_to_string(g_settings.radar_you_color) },
						{ xorstr_("radar_outline_color"), color_to_string(g_settings.radar_outline_color) },
						{ xorstr_("radar_cross_color"), color_to_string(g_settings.radar_cross_color) },
						{ xorstr_("radar_friends_color"), color_to_string(g_settings.radar_friends_color) },
						{ xorstr_("radar_teammates_color"), color_to_string(g_settings.radar_teammates_color) },
						{ xorstr_("radar_enemies_color"), color_to_string(g_settings.radar_enemies_color) },
						{ xorstr_("radar_friend_vehicles_color"), color_to_string(g_settings.radar_friend_vehicles_color) },
						{ xorstr_("radar_teammate_vehicles_color"), color_to_string(g_settings.radar_teammate_vehicles_color) },
						{ xorstr_("radar_enemy_vehicles_color"), color_to_string(g_settings.radar_enemy_vehicles_color) },
						{ xorstr_("explosives"), g_settings.explosives },
						{ xorstr_("explosives_color"), color_to_string(g_settings.explosives_color) },
						{ xorstr_("c4_bot"), g_settings.c4_bot },
						{ xorstr_("c4_bot_auto_detonate"), g_settings.c4_bot_auto_detonate },
						{ xorstr_("c4_bot_auto_detonate_independently"), g_settings.c4_bot_auto_detonate_independently },
						{ xorstr_("c4_bot_always_active"), g_settings.c4_bot_always_active },
						{ xorstr_("c4_bot_key"), g_settings.c4_bot_key },
						{ xorstr_("c4_bot_ignore_friends"), g_settings.c4_bot_ignore_friends },
						{ xorstr_("c4_bot_radius"), g_settings.c4_bot_radius },
						{ xorstr_("c4_bot_damage_radius"), g_settings.c4_bot_damage_radius },
						{ xorstr_("c4_bot_lethal_radius"), g_settings.c4_bot_lethal_radius },
						{ xorstr_("c4_bot_min_damage_to_enemy"), g_settings.c4_bot_min_damage_to_enemy },
						{ xorstr_("c4_bot_prevent_self_damage"), g_settings.c4_bot_prevent_self_damage },
						{ xorstr_("c4_bot_smart_self_damage"), g_settings.c4_bot_smart_self_damage },
						{ xorstr_("c4_bot_health_buffer"), g_settings.c4_bot_health_buffer },
						{ xorstr_("missiles_own"), g_settings.missiles_own },
						{ xorstr_("missiles_color"), color_to_string(g_settings.missiles_color) },
						{ xorstr_("jet_speed"), g_settings.jet_speed },
						{ xorstr_("spectator_list"), g_settings.spectator_list },
						{ xorstr_("spectator_raw_drawing"), g_settings.spectator_raw_drawing },
						{ xorstr_("spectator_disable_esp_aim"), g_settings.spectator_disable_esp_aim },
						{ xorstr_("spectator_warn_new"), g_settings.spectator_warn_new },
						{ xorstr_("spectator_x"), g_settings.spectator_x },
						{ xorstr_("spectator_y"), g_settings.spectator_y },
						{ xorstr_("spectator_warnings_size"), g_settings.spectator_warnings_size },
						{ xorstr_("spectator_color"), color_to_string(g_settings.spectator_color) },
						{ xorstr_("spectator_warnings_color"), color_to_string(g_settings.spectator_warnings_color) },
						{ xorstr_("kill_sound"), g_settings.kill_sound },
						{ xorstr_("kill_sound_path"), g_settings.kill_sound_path },
						{ xorstr_("minimap"), g_settings.minimap },
						{ xorstr_("obs_check"), g_settings.obs_check },
						{ xorstr_("screenshots"), g_settings.screenshots },
						{ xorstr_("screenshots_warn"), g_settings.screenshots_warn },
						{ xorstr_("screenshots_color"), color_to_string(g_settings.screenshots_color) },
						{ xorstr_("screenshots_pb_temp_disable"), g_settings.screenshots_pb_temp_disable },
						{ xorstr_("screenshots_pb_clean"), g_settings.screenshots_pb_clean },
						{ xorstr_("screenshots_pb_use_both"), g_settings.screenshots_pb_use_both },
						{ xorstr_("screenshots_pb_save_to_folder"), g_settings.screenshots_pb_save_to_folder },
						{ xorstr_("screenshots_pb_clean_delay"), g_settings.screenshots_pb_clean_delay },
						{ xorstr_("screenhots_pb_delay"), g_settings.screenhots_pb_delay },
						{ xorstr_("screenhots_post_pb_delay"), g_settings.screenhots_post_pb_delay },
						{ xorstr_("disable_watermark"), g_settings.disable_watermark },
						{ xorstr_("open_key"), g_globals.open_key },
					},
				},

			};
		}

		void refresh_configs()
		{
			available_configs.clear();

			auto config_dir = get_config_folder();

			// Create directory if it doesn't exist
			if (!std::filesystem::exists(config_dir))
				std::filesystem::create_directories(config_dir);

			// List all config files
			for (const auto& entry : std::filesystem::directory_iterator(config_dir))
			{
				if (entry.is_regular_file() && entry.path().extension() == xorstr_(".json"))
					available_configs.push_back(entry.path().stem().string());
			}

			// Add default config if it doesn't exist
			if (std::find(available_configs.begin(), available_configs.end(), "default") == available_configs.end())
			{
				available_configs.push_back(xorstr_("default"));
				save(xorstr_("default")); // Create default config
			}

			// Sort configs alphabetically
			std::sort(available_configs.begin(), available_configs.end());
		}

		std::filesystem::path get_config_folder()
		{
			auto folder_path = get_appdata_folder();
			folder_path /= xorstr_("Configs");
			return folder_path;
		}

		std::filesystem::path get_config_path(const std::string& config_name)
		{
			auto file_path = get_config_folder();
			file_path /= config_name + ".json";
			return file_path;
		}

		bool load(const std::string& config_name = "")
		{
			if (!config_name.empty())
				current_config = config_name;

			default_options = to_json();

			auto file_path = get_config_path(current_config);
			std::ifstream file(file_path);

			if (!file.is_open())
			{
				save(current_config);
				file.open(file_path);
			}

			try
			{
				file >> options;
				file.close();
			}
			catch (const std::exception&)
			{
				file.close();
				LOG(WARNING) << xorstr_("Detected corrupt settings in config '") << current_config << xorstr_("', writing default config...");
				save(current_config);
				return load(current_config);
			}

			bool should_save = deep_compare(options, default_options);
			from_json(options);

			if (should_save)
				save(current_config);

			return true;
		}

		bool save(const std::string& config_name = "")
		{
			if (!config_name.empty())
				current_config = config_name;

			auto config_dir = get_config_folder();

			// Create directory if it doesn't exist
			if (!std::filesystem::exists(config_dir))
				std::filesystem::create_directories(config_dir);

			auto file_path = get_config_path(current_config);

			std::ofstream file(file_path, std::ios::out | std::ios::trunc);
			file << to_json().dump(4);
			file.close();

			// Refresh the list of configs after saving a new one
			refresh_configs();

			return true;
		}

		bool delete_config(const std::string& config_name)
		{
			// Don't allow deleting the default config
			if (config_name == xorstr_("default"))
				return false;

			auto file_path = get_config_path(config_name);

			if (std::filesystem::exists(file_path))
			{
				std::filesystem::remove(file_path);
				refresh_configs();

				// If we deleted the current config, switch to default
				if (current_config == config_name)
				{
					current_config = xorstr_("default");
					load();
				}

				LOG(INFO) << xorstr_("Deleted config: ") << config_name;
				return true;
			}

			return false;
		}
	private:
		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false)
		{
			bool should_save = false;

			for (auto& e : default_settings.items())
			{
				const std::string& key = e.key();

				if (current_settings.count(key) == 0 || (compare_value && current_settings[key] != e.value()))
				{
					current_settings[key] = e.value();

					should_save = true;
				}
				else if (current_settings[key].is_object() && e.value().is_object())
				{
					if (deep_compare(current_settings[key], e.value(), compare_value))
						should_save = true;
				}
				else if (!current_settings[key].is_object() && e.value().is_object()) {
					current_settings[key] = e.value();

					should_save = true;
				}
			}

			return should_save;
		}
	};

	inline struct config g_config;
}