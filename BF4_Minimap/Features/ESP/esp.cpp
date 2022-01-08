#include "esp.h"
#include <sstream>
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/w2s.h"

namespace Features
{
	template <typename Type> std::string to_str(const Type& t)
	{
		std::ostringstream os;
		os << t;
		return os.str();
	}

	float Distance(const Vector3& point1, const Vector3& point2)
	{
		float distance = sqrt((point1.x - point2.x) * (point1.x - point2.x) +
			(point1.y - point2.y) * (point1.y - point2.y) +
			(point1.z - point2.z) * (point1.z - point2.z));

		return distance;
	}

	void DrawBone(const ImColor& color, RagdollComponent* pRag, UpdatePoseResultData::BONES Bone1, UpdatePoseResultData::BONES Bone2, bool Dots)
	{
		if (!IsValidPtr(pRag))
			return;

		Vector3 Bone1Vec;
		if (!pRag->GetBone(Bone1, Bone1Vec))
			return;

		Vector3 Bone2Vec;
		if (!pRag->GetBone(Bone2, Bone2Vec))
			return;

		if (!WorldToScreen(Bone1Vec))
			return;

		if (!WorldToScreen(Bone2Vec))
			return;

		auto pos1 = Vector2(Bone1Vec.x, Bone1Vec.y);
		auto pos2 = Vector2(Bone2Vec.x, Bone2Vec.y);

		if (Dots) 
		{
			m_pDrawing->DrawFillArea(pos1.x, pos1.y, 3.5f, 3.5f, color);
			m_pDrawing->DrawFillArea(pos2.x, pos2.y, 3.5f, 3.5f, color);
		}

		m_pDrawing->AddLine(ImVec2(pos1.x, pos1.y), ImVec2(pos2.x, pos2.y), color);
	}

	void DrawESP()
	{
		const auto GameContext = ClientGameContext::GetInstance();
		if (!GameContext) return;

		const auto PlayerManager = GameContext->m_pPlayerManager;
		if (!PlayerManager) return;

		const auto LocalPlayer = PlayerManager->m_pLocalPlayer;
		if (!LocalPlayer) return;

		const auto LocalSoldier = LocalPlayer->GetSoldier();
		if (!LocalSoldier) return;

		if (!LocalSoldier->IsAlive()) return;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* Player = PlayerManager->m_ppPlayers[i];
			if (!Player)
				continue;

			if (Player == LocalPlayer)
				continue;

			if (Player->m_TeamId == LocalPlayer->m_TeamId)
				continue;

			ClientSoldierEntity* Soldier = Player->GetSoldier();
			if (!Soldier) continue;

			if (!Soldier->IsAlive())
				continue;

			TransformAABBStruct Transform;
			ClientVehicleEntity* Vehicle = Player->GetVehicle();
			Vehicle ? Vehicle->GetAABB(&Transform) : Soldier->GetAABB(&Transform);

			Vector2 boxCords[2];
			if (GetBoxCords(Transform, &boxCords[0]))
			{
				/* Variables */
				float BoxWidth = boxCords[1].x - boxCords[0].x;
				float BoxHeight = boxCords[1].y - boxCords[0].y;

				TransformAABBStruct LocalTransform;
				auto LocalPlayerVehicle = LocalPlayer->GetVehicle();
				LocalPlayerVehicle ? LocalPlayerVehicle->GetAABB(&LocalTransform) : LocalSoldier->GetAABB(&LocalTransform);
				Vector3 LocalPos = (Vector3)LocalTransform.Transform.m[3];
				Vector3 Pos = (Vector3)Transform.Transform.m[3];
				
				float Meters = Distance(Pos, LocalPos);
				const char* PlayerName = IsValidPtr(Player->m_Name) ? Player->m_Name : "Unknown";
				float HealthPlayer = 0.f;
				float MaxHealthPlayer = 0.f;
				float HealthVehicle = 0.f; 
				float MaxHealthVehicle = 0.f;

				if (Vehicle)
				{
					if (IsValidPtr(Vehicle->m_pHealthComp) && Vehicle->m_pHealthComp->m_VehicleHealth)
						HealthVehicle = Vehicle->m_pHealthComp->m_VehicleHealth;

					if (Vehicle->m_Data)
					{
						auto VehicleData = reinterpret_cast<VehicleEntityData*>(Vehicle->m_Data);

						if (IsValidPtr(VehicleData))
							if (VehicleData->m_FrontHealthZone.m_MaxHealth)
								MaxHealthVehicle = VehicleData->m_FrontHealthZone.m_MaxHealth;
					}
				}
				else
				{
					if (IsValidPtr(Soldier->m_pHealthComp))
					{
						HealthPlayer = Soldier->m_pHealthComp->m_Health;
						MaxHealthPlayer = Soldier->m_pHealthComp->m_MaxHealth;
					}
				}

				float Health = Vehicle ? HealthVehicle : HealthPlayer;
				float MaxHealth = Vehicle ? MaxHealthVehicle : MaxHealthPlayer;

				/* Box */
				ImColor Visible = ImColor::Pink();
				ImColor NotVisible = ImColor::Purple();
				ImColor BoxColor = Soldier->m_Occluded ? NotVisible : Visible;
				m_pDrawing->DrawEspBox(2, boxCords[0].x, boxCords[0].y, boxCords[1].x - boxCords[0].x, boxCords[1].y - boxCords[0].y, BoxColor.Value.x, BoxColor.Value.y, BoxColor.Value.z, BoxColor.Value.w);
				
				/* Health Bar */
				float HealthBarWidth = max(BoxWidth, 8.0f);
				float HealthBarHeight = max(BoxWidth / 50.0f, 3.0f);
				float HealthBarWidthOffset = max((HealthBarWidth - BoxWidth) / 2, 0);
				float HealthBarHeightOffset = 5.0f;
				float HealthBarPercWidth = HealthBarWidth * (Health / MaxHealth);

				ImColor HealthBarColor(BYTE(255 - max(Health - MaxHealth / 2, 0) * (255 / (MaxHealth / 2))), BYTE(255 - max(MaxHealth / 2 - Health, 0) * (255 / (MaxHealth / 2))), 0, 255);
				m_pDrawing->DrawFillArea(boxCords[0].x - HealthBarWidthOffset, boxCords[1].y + HealthBarHeightOffset, HealthBarWidth, HealthBarHeight, ImColor(0, 0, 0, 255));
				m_pDrawing->DrawFillArea(boxCords[0].x - HealthBarWidthOffset, boxCords[1].y + HealthBarHeightOffset, HealthBarPercWidth, HealthBarHeight, HealthBarColor);

				/* Info */
				if (!Vehicle)
				{
					float r_Offset = 0.f;
					/* Name */
					m_pDrawing->AddText(boxCords[1].x + 3.5f, boxCords[0].y - 3.f, ImColor::White(), 14.f, FL_NONE, u8"%s", PlayerName);
					r_Offset += 7.f;

					/* Distance */
					float abs_ceil_distance = abs(ceil(Meters));
					std::string str = to_str(abs_ceil_distance) + "m";
					m_pDrawing->AddText(boxCords[1].x + 3.5f, boxCords[0].y + r_Offset, ImColor::White(), 14.f, FL_NONE, u8"%s", str.c_str());
				}

				/* Skeleton */
				RagdollComponent* RagdollComponent = Soldier->m_pRagdollComponent;
				auto SoldierRagdollComponent = IsValidPtr(RagdollComponent) ? RagdollComponent : nullptr;
				bool Dots = false;
				if (Meters <= 22.5f) Dots = true;

				ImColor SkeletonVisible = ImColor::Yellow();
				ImColor SkeletonNotVisible = ImColor::Pink();
				ImColor SkeletonColor = Soldier->m_Occluded ? SkeletonNotVisible : SkeletonVisible;

				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::Head, UpdatePoseResultData::Neck, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::Neck, UpdatePoseResultData::Spine2, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::Spine2, UpdatePoseResultData::Spine1, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::Spine1, UpdatePoseResultData::Spine, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::Neck, UpdatePoseResultData::LeftShoulder, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::LeftShoulder, UpdatePoseResultData::LeftElbowRoll, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::RightShoulder, UpdatePoseResultData::RightElbowRoll, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::LeftElbowRoll, UpdatePoseResultData::LeftHand, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::RightElbowRoll, UpdatePoseResultData::RightHand, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::Spine, UpdatePoseResultData::RightKneeRoll, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::Spine, UpdatePoseResultData::LeftKneeRoll, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::RightKneeRoll, UpdatePoseResultData::RightFoot, Dots);
				DrawBone(SkeletonColor, SoldierRagdollComponent, UpdatePoseResultData::LeftKneeRoll, UpdatePoseResultData::LeftFoot, Dots);
			}
		}
	}
}
