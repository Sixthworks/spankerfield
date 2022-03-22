#include "ImGui/imgui.h"
#include "Utilities/path.h"

namespace big
{
	class settings
	{
	public:
		bool blacklist{ true };

		bool esp{ true };
		float esp_distance { 10000.f };

		bool draw_box{ true };
		int box_style{ 4 };
		ImColor box_color_occluded{ 24, 162, 162, 255 };
		ImColor box_color{ 255, 51, 0, 255 };
		
		bool draw_health{ true };
		bool draw_name{ true };
		bool draw_distance{ true };
		ImColor text_color_occluded{ 24, 162, 162, 255 };
		ImColor text_color{ 255, 153, 51, 255 };

		bool draw_skeleton{ true };
		bool use_dots{ true };
		float dots_distance{ 22.5f };
		ImColor skeleton_color{ 220, 220, 220, 255 };

		bool radar;
		float radar_x{ 20.f };
		float radar_y{ 245.f };
		float radar_width{ 320.f };
		float radar_height{ 320.f };
		float radar_distance{ 1000.f };

		bool explosives{ true };
		bool jet_speed{ true };

		bool spectator_list{ true };
		bool raw_drawing;
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

		std::string format_color(ImColor color)
		{
			char buffer[64];
			sprintf_s(buffer, sizeof(buffer), "%f %f %f %f", color.Value.x, color.Value.y, color.Value.z, color.Value.w);

			return buffer;
		}

		ImColor get_color(std::string color)
		{
			float x = 0, y = 0, z = 0, w = 0;
			sscanf_s(color.c_str(), "%f %f %f %f", &x, &y, &z, &w);

			return ImColor(x, y, z, w);
		}

		void from_json(const nlohmann::json& j)
		{
			g_settings.blacklist = j["settings"]["blacklist"];

			g_settings.esp = j["settings"]["esp"];
			g_settings.esp_distance = j["settings"]["esp_distance"];

			g_settings.draw_box = j["settings"]["draw_box"];
			g_settings.box_style = j["settings"]["box_style"];
			// g_settings.box_color_occluded = get_color(j["settings"]["box_color_occluded"]);
			// g_settings.box_color = get_color(j["settings"]["box_color"]);

			g_settings.draw_health = j["settings"]["draw_health"];
			g_settings.draw_name = j["settings"]["draw_name"];
			g_settings.draw_distance = j["settings"]["draw_distance"];
			// g_settings.text_color_occluded = get_color(j["settings"]["text_color_occluded"]);
			// g_settings.text_color = get_color(j["settings"]["text_color"]);

			g_settings.draw_skeleton = j["settings"]["draw_skeleton"];
			g_settings.use_dots = j["settings"]["use_dots"];
			g_settings.dots_distance = j["settings"]["dots_distance"];
			// g_settings.skeleton_color = get_color(j["settings"]["skeleton_color"]);

			g_settings.radar = j["settings"]["radar"];
			g_settings.radar_x = j["settings"]["radar_x"];
			g_settings.radar_y = j["settings"]["radar_y"];
			g_settings.radar_width = j["settings"]["radar_width"];
			g_settings.radar_height = j["settings"]["radar_height"];
			g_settings.radar_distance = j["settings"]["radar_distance"];

			g_settings.explosives = j["settings"]["explosives"];
			g_settings.jet_speed = j["settings"]["jet_speed"];

			g_settings.spectator_list = j["settings"]["spectator_list"];
			g_settings.raw_drawing = j["settings"]["raw_drawing"];
			g_settings.spectator_x = j["settings"]["spectator_x"];
			g_settings.spectator_y = j["settings"]["spectator_y"];
			// g_settings.spectator_color = get_color(j["settings"]["spectator_color"]);

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
						{ "esp", g_settings.esp },
						{ "esp_distance", g_settings.esp_distance },
						{ "draw_box", g_settings.draw_box },
						{ "box_style", g_settings.box_style },
						// { "box_color_occluded", format_color(g_settings.box_color) },
						// { "box_color", format_color(g_settings.box_color) },
						{ "draw_health", g_settings.draw_health },
						{ "draw_name", g_settings.draw_name },
						{ "draw_distance", g_settings.draw_distance },
						// { "text_color_occluded", format_color(g_settings.text_color_occluded) },
						// { "text_color", format_color(g_settings.text_color) },
						{ "draw_skeleton", g_settings.draw_skeleton },
						{ "use_dots", g_settings.use_dots },
						{ "dots_distance", g_settings.dots_distance },
						// { "skeleton_color", format_color(g_settings.skeleton_color) },
						{ "radar", g_settings.radar },
						{ "radar_x", g_settings.radar_x },
						{ "radar_y", g_settings.radar_y },
						{ "radar_width", g_settings.radar_width },
						{ "radar_height", g_settings.radar_height },
						{ "radar_distance", g_settings.radar_distance },
						{ "explosives", g_settings.explosives },
						{ "jet_speed", g_settings.jet_speed },
						{ "spectator_list", g_settings.spectator_list },
						{ "raw_drawing", g_settings.raw_drawing },
						{ "spectator_x", g_settings.spectator_x },
						{ "spectator_y", g_settings.spectator_y },
						// { "spectator_color", format_color(g_settings.spectator_color) },
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