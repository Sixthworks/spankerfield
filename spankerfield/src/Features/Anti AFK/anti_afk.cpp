#include "anti_afk.h"
#include "../../settings.h"

using namespace big;
namespace plugins
{
	static ULONGLONG last_check = 0;
	void anti_afk()
	{
		if (!g_settings.anti_afk) return;

		// Most basic Anti-AFK ever
		if (GetTickCount64() - last_check > g_settings.anti_afk_timer)
		{
			const auto game_context = ClientGameContext::GetInstance();
			if (!game_context) return;

			const auto player_manager = game_context->m_pPlayerManager;
			if (!player_manager) return;

			const auto local_player = player_manager->m_pLocalPlayer;
			if (!IsValidPtrWithVTable(local_player)) return;

			const auto border_input_node = BorderInputNode::GetInstance();
			if (!border_input_node) return;

			const auto input_cache = border_input_node->m_InputCache;
			if (!input_cache) return;

			const auto input = input_cache->m_Event;
			if (!input) return;

			// Picked the ones that don't conflict with player movement
			input[ConceptMoveFB] = 1.0f;
			input[ConceptMoveLR] = 1.0f;
			input[ConceptFreeCameraMoveFB] = -1.0f;
			input[ConceptFreeCameraRotateX] = 1.0f;
			input[ConceptFreeCameraRotateY] = 1.0f;

			last_check = GetTickCount64();
		}
	}
}