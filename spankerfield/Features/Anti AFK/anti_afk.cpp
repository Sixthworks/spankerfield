#include "anti_afk.h"
#include "../../settings.h"
#include "../../global.h"

using namespace big;
namespace plugins
{
	void anti_afk()
	{
		if (!g_settings.anti_afk) return;

		// Most basic Anti-AFK ever
		if (GetTickCount64() - g_globals.g_afkcheck > g_settings.anti_afk_timer)
		{
			const auto game_context = ClientGameContext::GetInstance();
			if (!game_context) return;

			const auto player_manager = game_context->m_pPlayerManager;
			if (!player_manager) return;

			const auto local_player = player_manager->m_pLocalPlayer;
			if (!local_player) return;

			const auto border_input_node = BorderInputNode::GetInstance();
			if (!border_input_node) return;

			const auto input_cache = border_input_node->m_InputCache;
			if (!input_cache) return;

			const auto input = input_cache->m_Event;
			if (!input) return;

			// Shouldn't interrupt anything
			input[ConceptJump] = 1.f;

			g_globals.g_afkcheck = GetTickCount64();
		}
	}
}