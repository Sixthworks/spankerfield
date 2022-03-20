#include "ImGui/imgui.h"

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
		bool spectator_list_debug;
		float spectator_x{ 23.5f };
		float spectator_y{ 565.f };
		ImColor spectator_color{ 255, 255, 255, 255 };

		bool minimap{ true };
		bool obs_check{ true };
	};

	inline settings g_settings;
}