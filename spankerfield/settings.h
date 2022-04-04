#include "ImGui/imgui.h"
#include "Utilities/path.h"
#include "SDK/sdk.h"

namespace big
{
	class settings
	{
	public:
		bool blacklist{ true };
		ImColor blacklist_color{ 255, 0, 0, 255 };

		bool aimbot;
		bool aim_fov_method{ true };
		bool aim_draw_fov;
		float aim_fov{ 90.f };
		float aim_min_time_to_target{ 1.5f };
		float aim_max_time_to_target{ 3.0f };
		int aim_key{ VK_RBUTTON };
		int aim_bone{ UpdatePoseResultData::BONES::Head };

		bool overheat_control{ true };
		float overheat_control_critical{ 0.50f };

		bool infantry_alert{ true };
		bool infantry_alert_light_tech{ true };
		float infantry_alert_distance{ 22.5f };
		ImColor infantry_alert_color{ 255, 255, 0, 255 };

		bool esp{ true };
		bool esp_draw_teammates;
		float esp_distance { 10000.f };
		ImColor esp_teammate_color{ 63, 147, 216, 255 };

		bool esp_draw_box{ true };
		int esp_box_style{ 4 };
		ImColor esp_box_color_occluded{ 24, 162, 162, 255 };
		ImColor esp_box_color{ 255, 51, 0, 255 };
		
		bool esp_draw_health{ true };
		bool esp_draw_name{ true };
		bool esp_draw_distance{ true };
		ImColor text_color_occluded{ 24, 162, 162, 255 };
		ImColor text_color{ 255, 153, 51, 255 };

		bool skeleton{ true };
		bool skeleton_use_dots{ true };
		float skeleton_dots_distance{ 22.5f };
		ImColor skeleton_color{ 220, 220, 220, 255 };

		bool radar;
		bool radar_draw_teammates;
		float radar_x{ 20.f };
		float radar_y{ 245.f };
		float radar_width{ 320.f };
		float radar_height{ 320.f };
		float radar_distance{ 1000.f };
		ImColor radar_teammates_color{ 63, 147, 216, 255 };
		ImColor radar_enemies_color{ 215, 31, 30, 255 };
		ImColor radar_teammate_vehicles_color{ 0, 145, 255, 255 };
		ImColor radar_enemy_vehicles_color{ 255, 100, 0, 255 };

		bool explosives{ true };
		ImColor explosives_color{ 255, 0, 255, 255 };

		bool jet_speed{ true };

		bool spectator_list{ true };
		bool spectator_raw_drawing;
		float spectator_x{ 23.5f };
		float spectator_y{ 565.f };
		ImColor spectator_color{ 255, 255, 255, 255 };

		bool minimap{ true };
		bool obs_check{ true };
	};

	inline settings g_settings;

	struct config
	{
	public:
		nlohmann::json default_options;
		nlohmann::json options;

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
			g_settings.blacklist = j["settings"]["blacklist"];
			g_settings.blacklist_color = string_to_color(j["settings"]["blacklist_color"]);

			g_settings.aimbot = j["settings"]["aimbot"];
			g_settings.aim_fov_method = j["settings"]["aim_fov_method"];
			g_settings.aim_draw_fov = j["settings"]["aim_draw_fov"];
			g_settings.aim_fov = j["settings"]["aim_fov"];
			g_settings.aim_min_time_to_target = j["settings"]["aim_min_time_to_target"];
			g_settings.aim_max_time_to_target = j["settings"]["aim_max_time_to_target"];
			g_settings.aim_bone = j["settings"]["aim_bone"];

			g_settings.overheat_control = j["settings"]["overheat_control"];
			g_settings.overheat_control_critical = j["settings"]["overheat_control_critical"];

			g_settings.infantry_alert = j["settings"]["infantry_alert"];
			g_settings.infantry_alert_light_tech = j["settings"]["infantry_alert_light_tech"];
			g_settings.infantry_alert_distance = j["settings"]["infantry_alert_distance"];
			g_settings.infantry_alert_color = string_to_color(j["settings"]["infantry_alert_color"]);

			g_settings.esp = j["settings"]["esp"];
			g_settings.esp_distance = j["settings"]["esp_distance"];
			g_settings.esp_teammate_color = string_to_color(j["settings"]["esp_teammate_color"]);

			g_settings.esp_draw_box = j["settings"]["esp_draw_box"];
			g_settings.esp_box_style = j["settings"]["esp_box_style"];
			g_settings.esp_box_color_occluded = string_to_color(j["settings"]["esp_box_color_occluded"]);
			g_settings.esp_box_color = string_to_color(j["settings"]["esp_box_color"]);

			g_settings.esp_draw_health = j["settings"]["esp_draw_health"];
			g_settings.esp_draw_name = j["settings"]["esp_draw_name"];
			g_settings.esp_draw_distance = j["settings"]["esp_draw_distance"];
			g_settings.text_color_occluded = string_to_color(j["settings"]["text_color_occluded"]);
			g_settings.text_color = string_to_color(j["settings"]["text_color"]);

			g_settings.skeleton = j["settings"]["skeleton"];
			g_settings.skeleton_use_dots = j["settings"]["skeleton_use_dots"];
			g_settings.skeleton_dots_distance = j["settings"]["skeleton_dots_distance"];
			g_settings.skeleton_color = string_to_color(j["settings"]["skeleton_color"]);

			g_settings.radar = j["settings"]["radar"];
			g_settings.radar_x = j["settings"]["radar_x"];
			g_settings.radar_y = j["settings"]["radar_y"];
			g_settings.radar_width = j["settings"]["radar_width"];
			g_settings.radar_height = j["settings"]["radar_height"];
			g_settings.radar_distance = j["settings"]["radar_distance"];
			g_settings.radar_teammates_color = string_to_color(j["settings"]["radar_teammates_color"]);
			g_settings.radar_enemies_color = string_to_color(j["settings"]["radar_enemies_color"]);
			g_settings.radar_teammate_vehicles_color = string_to_color(j["settings"]["radar_teammate_vehicles_color"]);
			g_settings.radar_enemy_vehicles_color = string_to_color(j["settings"]["radar_enemy_vehicles_color"]);

			g_settings.explosives = j["settings"]["explosives"];
			g_settings.explosives_color = string_to_color(j["settings"]["explosives_color"]);

			g_settings.jet_speed = j["settings"]["jet_speed"];

			g_settings.spectator_list = j["settings"]["spectator_list"];
			g_settings.spectator_raw_drawing = j["settings"]["spectator_raw_drawing"];
			g_settings.spectator_x = j["settings"]["spectator_x"];
			g_settings.spectator_y = j["settings"]["spectator_y"];
			g_settings.spectator_color = string_to_color(j["settings"]["spectator_color"]);

			g_settings.minimap = j["settings"]["minimap"];
			g_settings.obs_check = j["settings"]["obs_check"];
		}

		nlohmann::json to_json()
		{
			return nlohmann::json{
				{
					"settings",
					{
					    { "blacklist", g_settings.blacklist },
						{ "blacklist_color", color_to_string(g_settings.blacklist_color) },
						{ "esp", g_settings.esp },
						{ "esp_distance", g_settings.esp_distance },
						{ "esp_teammate_color", color_to_string(g_settings.esp_teammate_color) },
						{ "esp_draw_box", g_settings.esp_draw_box },
						{ "esp_box_style", g_settings.esp_box_style },
						{ "esp_box_color_occluded", color_to_string(g_settings.esp_box_color_occluded) },
						{ "esp_box_color", color_to_string(g_settings.esp_box_color) },
						{ "esp_draw_health", g_settings.esp_draw_health },
						{ "esp_draw_name", g_settings.esp_draw_name },
						{ "esp_draw_distance", g_settings.esp_draw_distance },
						{ "text_color_occluded", color_to_string(g_settings.text_color_occluded) },
						{ "text_color", color_to_string(g_settings.text_color) },
						{ "skeleton", g_settings.skeleton },
						{ "skeleton_use_dots", g_settings.skeleton_use_dots },
						{ "skeleton_dots_distance", g_settings.skeleton_dots_distance },
			            { "skeleton_color", color_to_string(g_settings.skeleton_color) },
						{ "aimbot", g_settings.aimbot },
						{ "aim_fov_method", g_settings.aim_fov_method },
						{ "aim_draw_fov", g_settings.aim_draw_fov },
						{ "aim_fov", g_settings.aim_fov },
						{ "aim_min_time_to_target", g_settings.aim_min_time_to_target },
						{ "aim_max_time_to_target", g_settings.aim_max_time_to_target },
						{ "aim_bone", g_settings.aim_bone },
						{ "overheat_control", g_settings.overheat_control },
						{ "overheat_control_critical", g_settings.overheat_control_critical },
						{ "infantry_alert", g_settings.infantry_alert },
						{ "infantry_alert_light_tech", g_settings.infantry_alert_light_tech },
						{ "infantry_alert_distance", g_settings.infantry_alert_distance },
			            { "infantry_alert_color", color_to_string(g_settings.infantry_alert_color) },
						{ "radar", g_settings.radar },
						{ "radar_x", g_settings.radar_x },
						{ "radar_y", g_settings.radar_y },
						{ "radar_width", g_settings.radar_width },
						{ "radar_height", g_settings.radar_height },
						{ "radar_distance", g_settings.radar_distance },
						{ "radar_teammates_color", color_to_string(g_settings.radar_teammates_color) },
						{ "radar_enemies_color", color_to_string(g_settings.radar_enemies_color) },
						{ "radar_teammate_vehicles_color", color_to_string(g_settings.radar_teammate_vehicles_color) },
						{ "radar_enemy_vehicles_color", color_to_string(g_settings.radar_enemy_vehicles_color) },
						{ "explosives", g_settings.explosives },
			            { "explosives_color", color_to_string(g_settings.explosives_color) },
						{ "jet_speed", g_settings.jet_speed },
						{ "spectator_list", g_settings.spectator_list },
						{ "spectator_raw_drawing", g_settings.spectator_raw_drawing },
						{ "spectator_x", g_settings.spectator_x },
						{ "spectator_y", g_settings.spectator_y },
			            { "spectator_color", color_to_string(g_settings.spectator_color) },
						{ "minimap", g_settings.minimap },
						{ "obs_check", g_settings.obs_check },
					},
				},

			};
		}

		void attempt_save()
		{
			const nlohmann::json& j = to_json();

			if (deep_compare(options, j, true))
				save();
		}

		bool load()
		{
			default_options = to_json();

			auto file_path = get_path();
			std::ifstream file(file_path);

			if (!file.is_open())
			{
				save();

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

				LOG(WARNING) << xorstr_("Detected corrupt settings, writing default config...");

				save();

				return load();
			}

			bool should_save = deep_compare(options, default_options);

			from_json(options);

			if (should_save)
			{
				LOG(INFO) << xorstr_("Updating settings.");
				save();
			}
			
			return true;
		}
	private:
		std::filesystem::path get_path()
		{
			auto file_path = get_appdata_folder();
			file_path /= xorstr_("Config.json");

			return file_path;
		}

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

		bool save()
		{
			auto file_path = get_path();

			std::ofstream file(file_path, std::ios::out | std::ios::trunc);
			file << to_json().dump(4);
			file.close();

			return true;
		}
	};

	inline struct config g_config;
}