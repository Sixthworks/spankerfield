#pragma once
#include "../SDK/sdk.h"
#include "../ImGui/imgui.h"

namespace big
{
	ClientPlayer* get_player_by_name(std::string nick);

	bool color_wrapper(const char* label, ImColor* color);

	VehicleData* get_vehicle_data(ClientVehicleEntity* vehicle);
	TransformAABBStruct get_transform(ClientPlayer* player);
	float get_distance(const Vector3& point1, const Vector3& point2);
	std::string format_vehicle(std::string vehicle);

	IDXGISwapChain* get_swapchain();
	bool punkbuster_check();

	int generate_random_int(int min, int max);
	float generate_random_float(float min, float max);

	std::string current_time();
	bool is_process_running(const wchar_t* process_name);
}