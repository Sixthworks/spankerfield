#include "refresh_hook.h"
#include "../../Hooks/hooks.h"
#include "../../Utilities/vmt_hook.h"

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
				PreFrame::pPreFrameHook->Release();
			}

			return;
		}
		else if (refresh)
		{
			refresh = false;

			PreFrame::pPreFrameHook->Setup(border_input_node->m_Vtable);
			PreFrame::pPreFrameHook->Hook(3, PreFrame::PreFrameUpdate);

			LOG(INFO) << xorstr_("Refreshed PreFrameUpdate.");
		}
	}
}