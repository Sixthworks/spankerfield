#include "other.h"
#include "key_map.h"
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
		if (!IsValidPtr(local_player)) return nullptr;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const auto player = player_manager->m_ppPlayers[i];
			if (!IsValidPtr(player)) continue;

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

		bool result = ImGui::ColorEdit4(label, colors, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
		if (result)
			*color = {colors[0], colors[1], colors[2], colors[3]};

		return result;
	}

	VehicleData* get_vehicle_data(ClientVehicleEntity* vehicle)
	{
		const auto data = vehicle->m_Data;
		if (!IsValidPtr(data)) return nullptr;

		return data;
	}

	TransformAABBStruct get_transform(ClientPlayer* player)
	{
		TransformAABBStruct placeholder;

		const auto soldier = player->GetSoldier();
		if (!soldier) return placeholder;

		const auto vehicle = player->GetVehicle();
		IsValidPtr(vehicle) ? vehicle->GetAABB(&placeholder) : soldier->GetAABB(&placeholder);

		return placeholder;
	}

	TransformAABBStruct get_transform(ClientControllableEntity* entity)
	{
		TransformAABBStruct placeholder;

		if (IsValidPtr(entity))
			entity->GetAABB(&placeholder);

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

	bool is_localplayer_in_vehicle()
	{
		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return false;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return false;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return false;

		return IsValidPtr(local_player->GetVehicle());
	}

	WeaponFiring* get_weapon_firing()
	{
		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return nullptr;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return nullptr;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return nullptr;

		const auto weapon_firing = WeaponFiring::GetInstance();
		if (!weapon_firing) return nullptr;

		return weapon_firing;
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

	bool is_controller_connected()
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		DWORD dwResult = XInputGetState(0, &state);

		if (dwResult == ERROR_SUCCESS)
			return true;
		else
			return false;
	}

	bool is_left_trigger_pressed(float threshold)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		DWORD result = XInputGetState(0, &state);

		if (result == ERROR_SUCCESS)
		{
			float trigger_value = state.Gamepad.bLeftTrigger / 255.0f;

			return trigger_value > threshold;
		}

		return false;
	}

	bool is_any_recording_software_running()
	{
		const std::vector<std::wstring> recording_software =
		{
			L"obs64.exe",
			L"obs32.exe",
			L"obs.exe",
			L"bandicam.exe",
			L"bdcam.exe",
			L"action.exe", // Mirillis Action
			L"Action_x64.exe", // Mirillis Action 64-bit
			L"fraps.exe", // Fraps
			L"xsplit.core.exe", // XSplit
			L"xsplit.exe",
			L"camtasia.exe", // Camtasia
			L"camtasiastudio.exe",
			L"screencastomatic.exe", // Screencast-O-Matic
			L"dxtory.exe", // Dxtory
			L"wmcap.exe", // Windows Media Encoder
			L"atomicsoftwarestudio.exe", // Debut Video Capture
			L"streamlabs.exe", // Streamlabs OBS
		    L"streamlabsobs.exe"
		};

		for (const auto& process_name : recording_software)
		{
			if (is_process_running(xorstr_(process_name.c_str())))
				return true;
		}

		return false;
	}

	bool is_rivatuner_running()
	{
		const std::vector<std::wstring> rtss_names =
		{
			L"RTSS.exe",
			L"RTSSHooksLoader64.exe",
			L"RTSSHooksLoader.exe",
			L"EncoderServer.exe",
		};

		for (const auto& process_name : rtss_names)
		{
			if (is_process_running(xorstr_(process_name.c_str())))
				return true;
		}

		return false;
	}

	bool has_rivatuner_hooking_zero(const std::filesystem::path& config_path)
	{
		if (!std::filesystem::exists(config_path))
			return false;

		std::ifstream file(config_path);
		if (!file.is_open())
			return false;

		std::string line;
		constexpr std::string_view target = xorstr_("EnableHooking=0");
		while (std::getline(file, line))
		{
			if (line == target)
				return true;
		}

		return false;
	}

	void render_hotkey_selector(const char* title, int* pkey, float width, bool restricted)
	{
		const std::map<int, std::string>& key_source = restricted ? open_key_map : key_map;

		ImGui::Text(xorstr_("%s"), title);

		ImGui::PushItemWidth(width);

		const char* current_item = xorstr_("Select a key");
		auto it = key_source.find(*pkey);
		if (it != key_source.end())
			current_item = it->second.c_str();

		if (ImGui::BeginCombo(fmt::format(xorstr_("Key##{}"), title).c_str(), current_item))
		{
			for (const auto& [key, name] : key_source)
			{
				bool is_selected = (*pkey == key);
				if (ImGui::Selectable(name.c_str(), is_selected))
					*pkey = key;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
	}
}