#include "explosives.h"
#include "../../settings.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/w2s.h"

struct ClassInfos_s
{
	ClassInfo* MissileEntity = nullptr;
	ClassInfo* ExplosionEntity = nullptr;
	ClassInfo* VehicleEntity = nullptr;
	ClassInfo* WarningComponent = nullptr;
}ClassInfos;

void UpdateClassInfos()
{
	ClassInfos.MissileEntity = FindClassInfo(xorstr_("VeniceClientMissileEntity"));
	ClassInfos.ExplosionEntity = FindClassInfo(xorstr_("ClientExplosionPackEntity"));
	ClassInfos.VehicleEntity = FindClassInfo(xorstr_("ClientVehicleEntity"));
	ClassInfos.WarningComponent = FindClassInfo(xorstr_("ClientWarningSystemComponent"));
}

namespace features
{
	void draw_explosives()
	{
		if (!settings::explosives) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto level = game_context->m_pLevel;
		if (!level) return;

		const auto game_world = level->m_pGameWorld;
		if (!game_world) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		if (ClassInfos.ExplosionEntity)
		{
			EntityIterator<ClientExplosionEntity> explosives(game_world, ClassInfos.ExplosionEntity);

			if (explosives.front())
			{
				do
				{
					ClientExplosionEntity* explosive = explosives.front()->getObject();
					if (IsValidPtr(explosive))
					{
						TransformAABBStruct transform;
						ClientControllableEntity* explosive_controllable = (ClientControllableEntity*)explosive;
						explosive_controllable->GetAABB(&transform);

						Vector2 box_coords[2];
						if (utils::GetBoxCords(transform, &box_coords[0]))
						{
							float box_width = box_coords[1].x - box_coords[0].x;
							float box_height = box_coords[1].y - box_coords[0].y;

							m_pDrawing->AddText(box_coords[0].x + (box_width / 2), box_coords[0].y + (box_height / 2), ImColor::Pink(), 0.f, FL_CENTER_X, xorstr_(u8"%s"), xorstr_("[#]"));
						}
					}
				} while (explosives.next());
			}
		}
		else
			UpdateClassInfos();
	}
}
