#include "overheat.h"
#include "../../settings.h"

using namespace big;
namespace plugins
{
	static bool about_to_overheat = false;
	void overheat_control()
	{
		if (!g_settings.overheat_control) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		const auto vehicle = local_player->GetVehicle();
		if (!vehicle) return;

		const auto weapon = WeaponFiring::GetInstance();
		if (!weapon) return;

		if (weapon->m_FirstSlotBullets != -1) return;

		const auto border_input_node = BorderInputNode::GetInstance();
		if (!border_input_node) return;

		const auto mouse = border_input_node->m_pMouse;
		if (!mouse) return;

		const auto device = mouse->m_pDevice;
		if (!device) return;

		if (weapon->m_Overheat >= g_settings.overheat_critical)
		{
			about_to_overheat = true;
			device->m_Buffer.buttons[0] = false;
		}
		else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			border_input_node->m_InputCache->m_Event[ConceptFire] = 1.f;
		}
	}
}