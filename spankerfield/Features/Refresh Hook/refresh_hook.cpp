#include "refresh_hook.h"
#include "../../Hooks/hooks.h"
#include "../../Utilities/vtablehook.h"

using namespace big;
namespace plugins
{
	static bool refresh = false;
	void refresh_preframe_hook()
	{
		const auto border_input_node = BorderInputNode::GetInstance();
		if (!border_input_node) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		// Rehooking PreFrameUpdate vtable to prevent crashes when leaving the server (credits Menool)
		if (!IsValidPtr(player_manager->m_pLocalPlayer))
		{
			if (!refresh)
			{
				refresh = true;

				hook_vtable_func(reinterpret_cast<PDWORD64*>(border_input_node->m_Vtable), reinterpret_cast<PBYTE>(PreFrame::oPreFrameUpdate), 3);
			}

			return;
		}
		else if (refresh)
		{
			refresh = false;

			PreFrame::oPreFrameUpdate = reinterpret_cast<PreFrame::PreFrameUpdate_t>(hook_vtable_func(reinterpret_cast<PDWORD64*>(border_input_node->m_Vtable), reinterpret_cast<PBYTE>(&PreFrame::hkPreFrame), 3));
			LOG(INFO) << xorstr_("Refreshed PreFrame.");
		}
	}
}