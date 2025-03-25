#include "missiles.h"
#include "../../settings.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/w2s.h"
#include "../../SDK/class_info.h"

using namespace big;
namespace plugins
{
    void draw_missiles()
    {
        if (!g_settings.missiles_own) return;

        const auto game_context = ClientGameContext::GetInstance();
        if (!game_context) return;

        const auto player_manager = game_context->m_pPlayerManager;
        if (!player_manager) return;

        const auto local_player = player_manager->m_pLocalPlayer;
        if (!IsValidPtr(local_player)) return;

        const auto local_soldier = local_player->GetSoldier();
        if (!IsValidPtr(local_soldier) || !local_soldier->IsAlive()) return;

        if (!class_info.MissileEntity)
        {
            update_class_info();
            return;
        }

        const auto level = game_context->m_pLevel;
        if (!level) return;

        const auto game_world = level->m_pGameWorld;
        if (!game_world) return;

        EntityIterator<VeniceClientMissileEntity> missiles(game_world, class_info.MissileEntity);
        if (!missiles.front()) return;

        VeniceClientMissileEntity* local_missile = nullptr;
        do
        {
            auto* missile = missiles.front()->getObject();
            if (!IsValidPtr(missile)) continue;

            if (missile->m_pOwner.GetData() == local_player)
            {
                local_missile = missile;
                break;
            }
        } while (missiles.next());

        if (!IsValidPtr(local_missile)) return;

        const auto data = local_missile->m_pMissileEntityData;
        if (!IsValidPtr(data)) return;

        if (data->IsLockable()) return;

        if (!data->IsLaserGuided()) return;

        ClientControllableEntity* missile_controllable = (ClientControllableEntity*)local_missile;
        if (!IsValidPtr(missile_controllable)) return;

        TransformAABBStruct transform;
        missile_controllable->GetAABB(&transform);

        Vector2 box_coords[2];
        if (!get_box_coords(transform, &box_coords[0])) return;

        float box_width = box_coords[1].x - box_coords[0].x;
        float box_height = box_coords[1].y - box_coords[0].y;

        m_drawing->DrawEspBox(2, box_coords[0].x, box_coords[0].y, box_width, box_height, g_settings.missiles_color.Value.x, g_settings.missiles_color.Value.y, g_settings.missiles_color.Value.z, g_settings.missiles_color.Value.w);
    }
}