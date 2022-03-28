#include "other.h"
#include "../global.h"
#include <random>

namespace big
{
	VehicleEntityData* get_vehicle_data(ClientVehicleEntity* vehicle)
	{
		if (vehicle->m_Data)
		{
			auto data = reinterpret_cast<VehicleEntityData*>(vehicle->m_Data);
			if (IsValidPtr(data))
				return data;
		}

		return nullptr;
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

	std::vector<std::string> jets
	{
		xorstr_("ID_P_VNAME_F35"),
		xorstr_("ID_P_VNAME_J20"),
		xorstr_("ID_P_VNAME_PAKFA"),
		xorstr_("ID_P_VNAME_A10"),
		xorstr_("ID_P_VNAME_SU39"),
		xorstr_("ID_P_VNAME_Q5")
	};

	bool is_jet(const char* vehicle)
	{
		for (const auto& jet : jets)
		{
			if (jet == vehicle)
				return true;
		}

		return false;
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