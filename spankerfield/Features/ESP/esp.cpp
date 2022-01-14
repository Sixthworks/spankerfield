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

		if (!Utilities::WorldToScreen(Bone1Vec))
			return;

		if (!Utilities::WorldToScreen(Bone2Vec))
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

	std::string FormatVehicle(std::string VehName)
	{
		std::string Pattern = "_VNAME_";
		size_t Index = VehName.find(Pattern);
		std::string PlaceHolder = VehName;
		if (Index >= 0)
			PlaceHolder.erase(PlaceHolder.begin(), PlaceHolder.begin() + (Index + Pattern.length()));

		return PlaceHolder;
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

		/* Radar */
		float RadarX = 20.f;
		float RadarY = 245.f;
		float Width = 320.f;
		float Height = 320.f;
		m_pDrawing->DrawFillArea(RadarX, RadarY, Width, Height, ImColor::Black(160));

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

			TransformAABBStruct Transform = Utilities::GetTransform(Player);
			TransformAABBStruct LocalTransform = Utilities::GetTransform(LocalPlayer);;

			Vector3 LocalPos = (Vector3)LocalTransform.Transform.m[3];
			Vector3 Pos = (Vector3)Transform.Transform.m[3];
			Vector2 boxCords[2];

			const char* PlayerName = IsValidPtr(Player->m_Name) ? Player->m_Name : "Unknown";
			float HealthPlayer = 0.f;
			float MaxHealthPlayer = 0.f;
			float Meters = Distance(Pos, LocalPos);
			RagdollComponent* RagdollComponent = Soldier->m_pRagdollComponent;

			ClientVehicleEntity* Vehicle = Player->GetVehicle();
			char* VehicleName = nullptr;
			float HealthVehicle = 0.f;
			float MaxHealthVehicle = 0.f;

			if (IsValidPtr(Vehicle))
			{
				if (IsValidPtr(Vehicle->m_pHealthComp) && Vehicle->m_pHealthComp->m_VehicleHealth)
					HealthVehicle = Vehicle->m_pHealthComp->m_VehicleHealth;

				if (Vehicle->m_Data)
				{
					auto VehicleData = reinterpret_cast<VehicleEntityData*>(Vehicle->m_Data);
					if (IsValidPtr(VehicleData))
					{
						if (VehicleData->m_FrontHealthZone.m_MaxHealth)
							MaxHealthVehicle = VehicleData->m_FrontHealthZone.m_MaxHealth;

						if (IsValidPtr(VehicleData->m_NameSid))
							VehicleName = VehicleData->m_NameSid;
					}
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

			bool AimingValid = IsValidPtr(LocalSoldier->m_pWeaponComponent) && IsValidPtr(LocalSoldier->m_pWeaponComponent->GetActiveSoldierWeapon()) && IsValidPtr(LocalSoldier->m_pWeaponComponent->GetActiveSoldierWeapon()->m_pAuthoritativeAiming) && LocalSoldier->m_pWeaponComponent->GetActiveSoldierWeapon()->m_pAuthoritativeAiming->m_Yaw;
			if (AimingValid)
			{
				double Angle = -(double)LocalSoldier->m_pWeaponComponent->GetActiveSoldierWeapon()->m_pAuthoritativeAiming->m_Yaw;

				float DifferenceZ = LocalPos.z - Pos.z;
				float DifferenceX = LocalPos.x - Pos.x;
				float X = DifferenceX * (float)cos(Angle) - DifferenceZ * (float)sin(Angle);
				float Y = DifferenceX * (float)sin(Angle) + DifferenceZ * (float)cos(Angle);

				X *= 2;
				X += RadarX + (Width / 2.f);
				Y *= 2;
				Y += RadarY + (Height / 2.f);

				if (X < RadarX) X = RadarX;
				if (Y < RadarY) Y = RadarY;
				if (X > RadarX + Width - 3) X = RadarX + Width - 3;
				if (Y > RadarY + Height - 3) Y = RadarY + Height - 3;

				if (Meters >= 0.f && Meters <= 1000.f)
				{
					if (IsValidPtr(Vehicle)) {
						std::string str = FormatVehicle(VehicleName);

						m_pDrawing->AddText(X, Y - 3.5f, ImColor(39, 95, 239, 255), 15.f, FL_CENTER_X, u8"%s", str.c_str());
					}
					else
						m_pDrawing->DrawFillArea(X, Y, 4.5f, 4.5f, ImColor::Orange());
				}
			}

			if (Utilities::GetBoxCords(Transform, &boxCords[0]))
			{
				float BoxWidth = boxCords[1].x - boxCords[0].x;
				float BoxHeight = boxCords[1].y - boxCords[0].y;
				float Health = Vehicle ? HealthVehicle : HealthPlayer;
				float MaxHealth = Vehicle ? MaxHealthVehicle : MaxHealthPlayer;

				ImColor BoxColor = Soldier->m_Occluded ? ImColor(39, 95, 239, 255) : ImColor(239, 39, 39, 255);
				m_pDrawing->DrawEspBox(2, boxCords[0].x, boxCords[0].y, boxCords[1].x - boxCords[0].x, boxCords[1].y - boxCords[0].y, BoxColor.Value.x, BoxColor.Value.y, BoxColor.Value.z, BoxColor.Value.w);

				float HealthBarWidth = max(BoxWidth, 5.0f);
				float HealthBarHeight = max(BoxWidth / 50.0f, 3.0f);
				float HealthBarWidthOffset = max((HealthBarWidth - BoxWidth) / 2, 0);
				float HealthBarHeightOffset = 5.0f;
				float HealthBarPercWidth = HealthBarWidth * (Health / MaxHealth);
				ImColor HealthBarColor(BYTE(255 - max(Health - MaxHealth / 2, 0) * (255 / (MaxHealth / 2))), BYTE(255 - max(MaxHealth / 2 - Health, 0) * (255 / (MaxHealth / 2))), 0, 255);

				m_pDrawing->DrawBoxOutline(boxCords[0].x - HealthBarWidthOffset, boxCords[1].y + HealthBarHeightOffset, HealthBarWidth, HealthBarHeight, ImColor(0, 0, 0, 255));
				m_pDrawing->DrawFillArea(boxCords[0].x - HealthBarWidthOffset, boxCords[1].y + HealthBarHeightOffset, HealthBarPercWidth, HealthBarHeight, HealthBarColor);

				if (!Vehicle)
				{
					ImColor TextColor = Soldier->m_Occluded ? ImColor(39, 95, 239, 255) : ImColor::Orange();

					float r_Offset = 0.f;
					m_pDrawing->AddText(boxCords[1].x + 3.5f, boxCords[0].y - 3.f, TextColor, 14.f, FL_NONE, u8"%s", PlayerName);
					r_Offset += 8.f;

					float abs_ceil_distance = abs(ceil(Meters));
					std::string str = to_str(abs_ceil_distance) + "m";
					m_pDrawing->AddText(boxCords[1].x + 3.5f, boxCords[0].y + r_Offset, TextColor, 14.f, FL_NONE, u8"%s", str.c_str());
				}

				bool Dots = false;
				if (Meters <= 22.5f)
					Dots = true;

				ImColor SkeletonColor = ImColor(140, 140, 140, 255);
				if (IsValidPtr(RagdollComponent))
				{
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::Head, UpdatePoseResultData::Neck, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::Neck, UpdatePoseResultData::Spine2, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::Spine2, UpdatePoseResultData::Spine1, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::Spine1, UpdatePoseResultData::Spine, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::Neck, UpdatePoseResultData::LeftShoulder, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::LeftShoulder, UpdatePoseResultData::LeftElbowRoll, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::RightShoulder, UpdatePoseResultData::RightElbowRoll, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::LeftElbowRoll, UpdatePoseResultData::LeftHand, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::RightElbowRoll, UpdatePoseResultData::RightHand, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::Spine, UpdatePoseResultData::RightKneeRoll, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::Spine, UpdatePoseResultData::LeftKneeRoll, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::RightKneeRoll, UpdatePoseResultData::RightFoot, Dots);
					DrawBone(SkeletonColor, RagdollComponent, UpdatePoseResultData::LeftKneeRoll, UpdatePoseResultData::LeftFoot, Dots);
				}
			}
		}
	}
}
