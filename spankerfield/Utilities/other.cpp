#include "other.h"
#include "../global.h"

namespace big
{
	ClientPlayer* get_player_by_name(std::string nick)
	{
		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return nullptr;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return nullptr;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return nullptr;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!player) continue;

			const auto name = player->m_Name;
			if (!name) continue;

			if (name == nick)
				return player;
		}

		return nullptr;
	}

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
		IsValidPtrWithVTable(vehicle) ? vehicle->GetAABB(&placeholder) : soldier->GetAABB(&placeholder);

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

	bool punkbuster_capturing()
	{
		static auto screenshot_module = (uintptr_t*)OFFSET_SSMODULE;
		if (!IsValidPtr(screenshot_module))
			return true;

		return (*(int*)(*screenshot_module + 0x14) != -1);
	}

	WeaponFiring* get_weapon_firing()
	{
		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return nullptr;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return nullptr;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return nullptr;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return nullptr;

		if (local_soldier->IsAlive())
		{
			const auto weapon = WeaponFiring::GetInstance();
			if (!weapon) return nullptr;

			return weapon;
		}

		return nullptr;
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

	std::string get_random_string(const std::vector<std::string>& strings)
	{
		if (strings.empty())
			return "";

		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<size_t> distribution(0, strings.size() - 1);

		size_t random_index = distribution(generator);
		return strings[random_index];
	}

	bool is_process_running(const wchar_t* process_name)
	{
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (!Process32First(snapshot, &entry))
		{
			CloseHandle(snapshot);
			return false;
		}

		do
		{
			if (!_tcsicmp(entry.szExeFile, process_name))
			{
				CloseHandle(snapshot);
				return true;
			}
		} while (Process32Next(snapshot, &entry));

		CloseHandle(snapshot);
		return false;
	}
}