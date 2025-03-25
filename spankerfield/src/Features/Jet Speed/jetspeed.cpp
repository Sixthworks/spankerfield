#include "jetspeed.h"
#include "../../settings.h"
#include "../../Utilities/other.h"

using namespace big;
namespace plugins
{
	void set_jet_speed()
	{
		if (!g_settings.jet_speed) return;

		const auto border_input_node = BorderInputNode::GetInstance();
		if (!border_input_node) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto level = game_context->m_pLevel;
		if (!level) return;

		const auto game_world = level->m_pGameWorld;
		if (!game_world) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		const auto local_vehicle = local_player->GetVehicle();
		if (!IsValidPtr(local_vehicle)) return;

		const auto vehicle_data = get_vehicle_data(local_vehicle);
		if (!IsValidPtr(vehicle_data)) return;

		if (vehicle_data->IsInJet())
		{
			const auto input_cache = border_input_node->m_InputCache;
			if (!input_cache) return;

			const auto input = input_cache->m_Event;
			if (!input) return;

			// Simpler velocity
			auto velocity = local_vehicle->m_VelocityVec.Length() * 3.6f;

			switch (vehicle_data->GetVehicleType())
			{
			case VehicleData::VehicleType::JET:
				if (velocity < 314.f && velocity > 311.f)
				{
					input[ConceptMoveForward] = 1.0f;
					input[ConceptMoveFB] = 1.0f;
					input[ConceptFreeCameraMoveFB] = 1.0f;
					input[ConceptFreeCameraTurboSpeed] = 0.0f;
					input[ConceptSprint] = 0.0f;
				}
				else if (velocity >= 315.f)
				{
					input[ConceptMoveBackward] = 1.0f;
					input[ConceptMoveFB] = -1.0f;
					input[ConceptFreeCameraMoveFB] = -1.0f;
					input[ConceptFreeCameraTurboSpeed] = 0.0f;
					input[ConceptSprint] = 0.0f;
					input[ConceptBrake] = 1.0f;
					input[ConceptCrawl] = 1.0f;
				}
				else if (velocity <= 311.f)
				{
					input[ConceptMoveFB] = 1.0f;
					input[ConceptFreeCameraMoveFB] = 1.0f;
					input[ConceptFreeCameraTurboSpeed] = 1.0f;
					input[ConceptSprint] = 1.0f;
				}
				return;
			case VehicleData::VehicleType::JETBOMBER:
				if (velocity < 317.f && velocity > 313.f)
				{
					input[ConceptMoveForward] = 1.0f;
					input[ConceptMoveFB] = 1.0f;
					input[ConceptFreeCameraMoveFB] = 1.0f;
					input[ConceptFreeCameraTurboSpeed] = 0.0f;
					input[ConceptSprint] = 0.0f;
				}
				else if (velocity >= 318.f)
				{
					input[ConceptMoveBackward] = 1.0f;
					input[ConceptMoveFB] = -1.0f;
					input[ConceptFreeCameraMoveFB] = -1.0f;
					input[ConceptFreeCameraTurboSpeed] = 0.0f;
					input[ConceptSprint] = 0.0f;
					input[ConceptBrake] = 1.0f;
					input[ConceptCrawl] = 1.0f;
				}
				else if (velocity <= 313.f)
				{
					input[ConceptMoveFB] = 1.0f;
					input[ConceptFreeCameraMoveFB] = 1.0f;
					input[ConceptFreeCameraTurboSpeed] = 1.0f;
					input[ConceptSprint] = 1.0f;
				}
				return;
			default:
				return;
			}
		}
	}
}