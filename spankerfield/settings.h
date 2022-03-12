#include "ImGui/imgui.h"

namespace settings
{
	inline bool blacklist{ true };

	inline bool esp{ true };
	inline bool draw_box{ true };
	inline int box_style{ 2 };
	inline ImColor box_color_occluded{ 24, 162, 162, 255 };
	inline ImColor box_color{ 255, 51, 0, 255 };
	inline bool draw_health{ true };
	inline bool draw_name{ true };
	inline bool draw_distance{ true };
	inline ImColor text_color_occluded{ 24, 162, 162, 255 };
	inline ImColor text_color{ 255, 153, 51, 255 };
	inline bool draw_skeleton{ true };
	inline ImColor skeleton_color{ 220, 220, 220, 255 };

	inline bool radar;
	inline float radar_x{ 20.f };
	inline float radar_y{ 245.f };
	inline float radar_width{ 320.f };
	inline float radar_height{ 320.f };

	inline bool explosives{ true };
	inline bool jet_speed{ true };

	inline bool spectator_list{ true };
	inline bool spectator_list_debug;
	inline float spectator_x{ 23.5f };
	inline float spectator_y{ 565.f };

	inline bool minimap{ true };
	inline bool obs_check{ true };
}