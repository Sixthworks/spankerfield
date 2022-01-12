#include "explosives.h"
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

namespace Features
{
	void DrawExplosives()
	{
		const auto GameContext = ClientGameContext::GetInstance();
		if (!GameContext) return;

		const auto Level = GameContext->m_pLevel;
		if (!Level) return;

		const auto GameWorld = Level->m_pGameWorld;
		if (!GameWorld) return;

		const auto PlayerManager = GameContext->m_pPlayerManager;
		if (!PlayerManager) return;

		const auto LocalPlayer = PlayerManager->m_pLocalPlayer;
		if (!LocalPlayer) return;

		const auto LocalSoldier = LocalPlayer->GetSoldier();
		if (!LocalSoldier) return;

		if (!LocalSoldier->IsAlive()) return;

		if (ClassInfos.ExplosionEntity)
		{
			EntityIterator<ClientExplosionEntity> explosives(GameWorld, ClassInfos.ExplosionEntity);

			if (explosives.front())
			{
				do
				{
					ClientExplosionEntity* pExplosive = explosives.front()->getObject();
					if (IsValidPtr(pExplosive))
					{
						TransformAABBStruct Transform;
						ClientControllableEntity* ExplosiveControllable = (ClientControllableEntity*)pExplosive;
						ExplosiveControllable->GetAABB(&Transform);

						Vector2 boxCords[2];
						if (GetBoxCords(Transform, &boxCords[0]))
						{
							float BoxWidth = boxCords[1].x - boxCords[0].x;
							float BoxHeight = boxCords[1].y - boxCords[0].y;
							m_pDrawing->AddText(boxCords[0].x + (BoxWidth / 2), boxCords[0].y + (BoxHeight / 2), ImColor::Pink(), 0.f, FL_CENTER_X, u8"%s", xorstr_("[#]"));
						}
					}
				} while (explosives.next());
			}
		}
		else
			UpdateClassInfos();
	}
}