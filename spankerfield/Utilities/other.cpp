#include "other.h"
#include "../global.h"

namespace big
{
	bool color_wrapper(const char* label, ImColor* color)
	{
		float colors[4];
		colors[0] = color->Value.x;
		colors[1] = color->Value.y;
		colors[2] = color->Value.z;
		colors[3] = color->Value.w;

		bool result = ImGui::ColorEdit4(label, colors);
		if (result)
			*color = {colors[0], colors[1], colors[2], colors[3]};

		return result;
	}

	VehicleData* get_vehicle_data(ClientVehicleEntity* vehicle)
	{
		return vehicle->m_Data;
	}

	TransformAABBStruct get_transform(ClientPlayer* player)
	{
		TransformAABBStruct placeholder;

		const auto soldier = player->GetSoldier();
		if (!soldier) return placeholder;

		const auto vehicle = player->GetVehicle();
		vehicle ? vehicle->GetAABB(&placeholder) : soldier->GetAABB(&placeholder);

		return placeholder;
	}

	float get_distance(const Vector3& point1, const Vector3& point2)
	{
		float distance = sqrt((point1.x - point2.x) * (point1.x - point2.x) +
			(point1.y - point2.y) * (point1.y - point2.y) +
			(point1.z - point2.z) * (point1.z - point2.z));

		return distance;
	}

	std::string format_vehicle(std::string vehicle)
	{
		std::string placeholder(vehicle);
		std::string pattern = xorstr_("_VNAME_");

		size_t index = vehicle.find(pattern);
		if (index >= 0)
			placeholder.erase(placeholder.begin(), placeholder.begin() + (index + pattern.length()));

		return placeholder;
	}

	IDXGISwapChain* get_swapchain()
	{
		const auto renderer = DxRenderer::GetInstance();
		if (!renderer) return nullptr;

		const auto screen = renderer->m_pScreen;
		if (!screen) return nullptr;

		const auto swapchain = screen->m_pSwapChain;
		if (!swapchain) return nullptr;

		return swapchain;
	}

	bool punkbuster_check()
	{
		static auto screenshot_module = (uintptr_t*)OFFSET_SSMODULE;
		if (!IsValidPtr(screenshot_module))
			return true;

		bool status = (*(int*)(*screenshot_module + 0x14) != -1);
		g_globals.g_punkbuster = status;

		return status;
	}

	int generate_random_int(int min, int max)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);

		return dist6(rng);
	}

	float generate_random_float(float min, float max)
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<> distr(min, max);

		return (float)distr(eng);
	}

	std::string current_time()
	{
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, xorstr_("%d-%m-%Y %H-%M-%S"));
		auto str = oss.str();

		return str;
	};

	bool is_process_running(const wchar_t* process_name)
	{
		bool exists = false;
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry))
			while (Process32Next(snapshot, &entry))
				if (!_wcsicmp(entry.szExeFile, process_name))
					exists = true;

		CloseHandle(snapshot);
		return exists;
	}
}