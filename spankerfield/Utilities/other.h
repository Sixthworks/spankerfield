#pragma once
#include "../SDK/sdk.h"

namespace big
{
	VehicleData* get_vehicle_data(ClientVehicleEntity* vehicle);
	TransformAABBStruct get_transform(ClientPlayer* player);
	float get_distance(const Vector3& point1, const Vector3& point2);
	std::string format_vehicle(std::string vehicle);

	bool punkbuster_check();

	int generate_random_int(int min, int max);
	float generate_random_float(float min, float max);

	std::string current_time();
	bool is_process_running(const wchar_t* process_name);
}