#include "esp.h"
#include "../../settings.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/w2s.h"
#include <sstream>

namespace features
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

		if (!utils::WorldToScreen(Bone1Vec))
			return;

		if (!utils::WorldToScreen(Bone2Vec))
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
		std::string Pattern = xorstr_("_VNAME_");
		size_t Index = VehName.find(Pattern);
		std::string PlaceHolder = VehName;
		if (Index >= 0)
			PlaceHolder.erase(PlaceHolder.begin(), PlaceHolder.begin() + (Index + Pattern.length()));

		return PlaceHolder;
	}

	void draw_esp()
	{
		if (!settings::esp) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!local_player) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!local_soldier) return;

		if (!local_soldier->IsAlive()) return;

		if (settings::radar)
		{
			m_pDrawing->DrawFillArea(settings::radar_x, settings::radar_y, settings::radar_width, settings::radar_height, ImColor::Black(160));
		}

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			ClientPlayer* player = player_manager->m_ppPlayers[i];
			if (!player)
				continue;

			if (player == local_player)
				continue;

			if (player->m_TeamId == local_player->m_TeamId)
				continue;

			ClientSoldierEntity* soldier = player->GetSoldier();
			if (!soldier) continue;

			if (!soldier->IsAlive())
				continue;

			TransformAABBStruct transform = utils::GetTransform(player);
			TransformAABBStruct local_transform = utils::GetTransform(local_player);;

			Vector3 local_pos = (Vector3)local_transform.Transform.m[3];
			Vector3 pos = (Vector3)transform.Transform.m[3];
			Vector2 box_coords[2];

			const char* nickname = IsValidPtr(player->m_Name) ? player->m_Name : xorstr_("Unknown");
			float health_player = 0.f;
			float max_health_player = 0.f;
			float distance = Distance(pos, local_pos);
			RagdollComponent* ragdoll_component = soldier->m_pRagdollComponent;

			ClientVehicleEntity* vehicle = player->GetVehicle();
			char* vehicle_name = nullptr;
			float health_vehicle = 0.f;
			float max_health_vehicle = 0.f;

			if (IsValidPtr(vehicle))
			{
				if (IsValidPtr(vehicle->m_pHealthComp) && vehicle->m_pHealthComp->m_VehicleHealth)
					health_vehicle = vehicle->m_pHealthComp->m_VehicleHealth;

				if (vehicle->m_Data)
				{
					auto vehicle_data = reinterpret_cast<VehicleEntityData*>(vehicle->m_Data);
					if (IsValidPtr(vehicle_data))
					{
						if (vehicle_data->m_FrontHealthZone.m_MaxHealth)
							max_health_vehicle = vehicle_data->m_FrontHealthZone.m_MaxHealth;

						if (IsValidPtr(vehicle_data->m_NameSid))
							vehicle_name = vehicle_data->m_NameSid;
					}
				}
			}
			else
			{
				if (IsValidPtr(soldier->m_pHealthComp))
				{
					health_player = soldier->m_pHealthComp->m_Health;
					max_health_player = soldier->m_pHealthComp->m_MaxHealth;
				}
			}

			if (settings::radar)
			{
				bool valid_aiming = IsValidPtr(local_soldier->m_pWeaponComponent) && IsValidPtr(local_soldier->m_pWeaponComponent->GetActiveSoldierWeapon()) && IsValidPtr(local_soldier->m_pWeaponComponent->GetActiveSoldierWeapon()->m_pAuthoritativeAiming) && local_soldier->m_pWeaponComponent->GetActiveSoldierWeapon()->m_pAuthoritativeAiming->m_Yaw;
				if (valid_aiming)
				{
					double angle = -(double)local_soldier->m_pWeaponComponent->GetActiveSoldierWeapon()->m_pAuthoritativeAiming->m_Yaw;

					float difference_z = local_pos.z - pos.z;
					float difference_x = local_pos.x - pos.x;
					float x = difference_x * (float)cos(angle) - difference_z * (float)sin(angle);
					float y = difference_x * (float)sin(angle) + difference_z * (float)cos(angle);

					x *= 2;
					x += settings::radar_x + (settings::radar_width / 2.f);
					y *= 2;
					y += settings::radar_y + (settings::radar_height / 2.f);

					if (x < settings::radar_x) x = settings::radar_x;
					if (y < settings::radar_y) y = settings::radar_y;
					if (x > settings::radar_x + settings::radar_width - 3) x = settings::radar_x + settings::radar_width - 3;
					if (y > settings::radar_y + settings::radar_height - 3) y = settings::radar_y + settings::radar_height - 3;

					if (distance >= 0.f && distance <= 1000.f)
					{
						if (IsValidPtr(vehicle)) {
							std::string str = FormatVehicle(vehicle_name);

							m_pDrawing->AddText(x, y - 3.5f, ImColor(39, 95, 239, 255), 15.f, FL_CENTER_X, xorstr_(u8"%s"), str.c_str());
						}
						else
							m_pDrawing->DrawFillArea(x, y, 4.5f, 4.5f, ImColor::Orange());
					}
				}
			}

			if (utils::GetBoxCords(transform, &box_coords[0]))
			{
				float box_width = box_coords[1].x - box_coords[0].x;
				float box_height = box_coords[1].y - box_coords[0].y;
				float health = vehicle ? health_vehicle : health_player;
				float max_health = vehicle ? max_health_vehicle : max_health_player;

				if (settings::draw_box)
				{
					ImColor box_color = soldier->m_Occluded ? settings::box_color_occluded : settings::box_color;
					m_pDrawing->DrawEspBox(settings::box_style, box_coords[0].x, box_coords[0].y, box_coords[1].x - box_coords[0].x, box_coords[1].y - box_coords[0].y, box_color.Value.x, box_color.Value.y, box_color.Value.z, box_color.Value.w);
				}

				if (settings::draw_health)
				{
					ImColor hb_color(BYTE(255 - max(health - max_health / 2, 0) * (255 / (max_health / 2))), BYTE(255 - max(max_health / 2 - health, 0) * (255 / (max_health / 2))), 0, 255);
					float hb_width = max(box_width, 5.0f);
					float hb_height = max(box_width / 50.0f, 3.0f);
					float hb_width_offset = max((hb_width - box_width) / 2, 0);
					float hb_height_offset = 5.0f;
					float hb_perc_width = hb_width * (health / max_health);

					m_pDrawing->DrawBoxOutline(box_coords[0].x - hb_width_offset, box_coords[1].y + hb_height_offset, hb_width, hb_height, ImColor(0, 0, 0, 255));
					m_pDrawing->DrawFillArea(box_coords[0].x - hb_width_offset, box_coords[1].y + hb_height_offset, hb_perc_width, hb_height, hb_color);
				}

				if (!IsValidPtr(vehicle))
				{
					ImColor text_color = soldier->m_Occluded ? settings::text_color_occluded : settings::text_color;
					float base[2] = { box_coords[1].x + 3.5f, box_coords[0].y - 3.f };

					if (settings::draw_name)
					{
						m_pDrawing->AddText(base[0], base[1], text_color, 14.f, FL_NONE, xorstr_(u8"%s"), nickname);
						base[1] += 9.f;
					}

					if (settings::draw_distance)
					{
						float fmt_distance = abs(ceil(distance));
						std::string str(to_str(fmt_distance) + xorstr_("m"));

						m_pDrawing->AddText(base[0], base[1], text_color, 14.f, FL_NONE, xorstr_(u8"%s"), str.c_str());
						base[1] += 9.f;
					}
				}

				if (settings::draw_skeleton)
				{
					bool dots = false;
					if (distance <= 22.5f)
						dots = true;

					if (IsValidPtr(ragdoll_component))
					{
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::Head, UpdatePoseResultData::Neck, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::Neck, UpdatePoseResultData::Spine2, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::Spine2, UpdatePoseResultData::Spine1, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::Spine1, UpdatePoseResultData::Spine, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::Neck, UpdatePoseResultData::LeftShoulder, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::LeftShoulder, UpdatePoseResultData::LeftElbowRoll, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::RightShoulder, UpdatePoseResultData::RightElbowRoll, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::LeftElbowRoll, UpdatePoseResultData::LeftHand, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::RightElbowRoll, UpdatePoseResultData::RightHand, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::Spine, UpdatePoseResultData::RightKneeRoll, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::Spine, UpdatePoseResultData::LeftKneeRoll, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::RightKneeRoll, UpdatePoseResultData::RightFoot, dots);
						DrawBone(settings::skeleton_color, ragdoll_component, UpdatePoseResultData::LeftKneeRoll, UpdatePoseResultData::LeftFoot, dots);
					}
				}
			}
		}
	}
}
