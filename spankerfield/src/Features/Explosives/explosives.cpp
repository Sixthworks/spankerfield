#include "explosives.h"
#include "../../settings.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/w2s.h"
#include "../../SDK/class_info.h"

using namespace big;
namespace plugins
{
    void draw_explosives()
    {
        if (!g_settings.explosives) return;

        const auto game_context = ClientGameContext::GetInstance();
        if (!game_context) return;

        const auto player_manager = game_context->m_pPlayerManager;
        if (!player_manager) return;

        const auto local_player = player_manager->m_pLocalPlayer;
        if (!IsValidPtr(local_player)) return;

        const auto local_soldier = local_player->GetSoldier();
        if (!IsValidPtr(local_soldier) || !local_soldier->IsAlive()) return;

        if (!class_info.ExplosionEntity)
        {
            update_class_info();
            return;
        }

        const auto level = game_context->m_pLevel;
        if (!level) return;

        const auto game_world = level->m_pGameWorld;
        if (!game_world) return;

        EntityIterator<ClientExplosionEntity> explosives(game_world, class_info.ExplosionEntity);
        if (!explosives.front()) return;

        do
        {
            ClientExplosionEntity* explosive = explosives.front()->getObject();
            if (!IsValidPtr(explosive)) continue;

            ClientControllableEntity* explosive_controllable = (ClientControllableEntity*)explosive;
            if (!IsValidPtr(explosive_controllable)) continue;

            TransformAABBStruct transform;
            explosive_controllable->GetAABB(&transform);

            Vector2 box_coords[2];
            if (!get_box_coords(transform, &box_coords[0])) continue;

            float box_width = box_coords[1].x - box_coords[0].x;
            float box_height = box_coords[1].y - box_coords[0].y;

            m_drawing->AddText(box_coords[0].x + (box_width / 2.f), box_coords[0].y + (box_height / 2.f), g_settings.explosives_color, 0.f, FL_CENTER_X | FL_SHADOW, xorstr_("[#]"));
        } while (explosives.next());
    }
}