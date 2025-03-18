#include "anti_afk.h"
#include "../../settings.h"

using namespace big;

namespace plugins
{
    static ULONGLONG last_check = 0;

    void anti_afk()
    {
        if (!g_settings.anti_afk) return;

        // Only perform anti-AFK actions based on timer
        if (GetTickCount64() - last_check > g_settings.anti_afk_timer)
        {
            const auto game_context = ClientGameContext::GetInstance();
            if (!game_context) return;

            const auto border_input_node = BorderInputNode::GetInstance();
            if (!border_input_node) return;

            const auto input_cache = border_input_node->m_InputCache;
            if (!input_cache) return;

            const auto input = input_cache->m_Event;
            if (!input) return;

            input[ConceptMenuDigitalRight] = 1.0f;
            input[ConceptSquadVoice] = 1.0f;
            input[ConceptSpectatorFreeCameraRotateY] = 0.1f;

            last_check = GetTickCount64();
        }
    }
}