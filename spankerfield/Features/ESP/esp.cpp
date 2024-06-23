#include "esp.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Utilities/w2s.h"
#include "../../Utilities/other.h"
#include "../../Rendering/nicknames.h"

using namespace big;
namespace plugins
{
    void draw_bone(const ImColor& color, RagdollComponent* ragdoll, UpdatePoseResultData::BONES bone_1, UpdatePoseResultData::BONES bone_2, bool dots)
    {
        if (!IsValidPtr(ragdoll))
            return;

        Vector3 bone_1_vec;
        if (!ragdoll->GetBone(bone_1, bone_1_vec))
            return;

        Vector3 bone_2_vec;
        if (!ragdoll->GetBone(bone_2, bone_2_vec))
            return;

        if (!world_to_screen(bone_1_vec) || !world_to_screen(bone_2_vec))
            return;

        auto pos_1 = Vector2(bone_1_vec.x, bone_1_vec.y);
        auto pos_2 = Vector2(bone_2_vec.x, bone_2_vec.y);

        if (dots)
        {
            m_drawing->DrawFillArea(pos_1.x, pos_1.y, 3.5f, 3.5f, color);
            m_drawing->DrawFillArea(pos_2.x, pos_2.y, 3.5f, 3.5f, color);
        }

        m_drawing->AddLine(ImVec2(pos_1.x, pos_1.y), ImVec2(pos_2.x, pos_2.y), color);
    }

    std::map<uint64_t, std::string> streamer_personas{};
    void draw_esp()
    {
        if (!g_settings.esp) return;

        const auto game_context = ClientGameContext::GetInstance();
        if (!game_context) return;

        const auto player_manager = game_context->m_pPlayerManager;
        if (!player_manager) return;

        const auto local_player = player_manager->m_pLocalPlayer;
        if (!local_player) return;

        const auto local_soldier = local_player->GetSoldier();
        if (!local_soldier || !local_soldier->IsAlive()) return;

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            const auto player = player_manager->m_ppPlayers[i];
            if (!IsValidPtr(player) || player == local_player)
                continue;

            bool teammate = IsValidPtr(player) && player->m_TeamId == local_player->m_TeamId;
            if (teammate && !g_settings.esp_draw_teammates)
                continue;

            const auto soldier = player->GetSoldier();
            if (!IsValidPtr(soldier) || !soldier->IsAlive()) continue;

            TransformAABBStruct transform = get_transform(player);
            TransformAABBStruct local_transform = get_transform(local_player);

            Vector3 local_pos = Vector3(local_transform.Transform.m[3]);
            Vector3 pos = Vector3(transform.Transform.m[3]);
            Vector2 box_coords[2];

            const char* nickname = IsValidPtr(player->m_Name) ? player->m_Name : xorstr_("Unknown");

            // Streamer mode
            if (g_settings.streamer_mode)
            {
                uint64_t persona_id = player->m_onlineId.m_personaid;

                if (persona_id != 0)
                {
                    if (streamer_personas.count(persona_id) <= 0)
                        streamer_personas[persona_id] = get_random_string(weird_nicknames);

                    nickname = streamer_personas[persona_id].empty() ? xorstr_("Unknown") : streamer_personas[persona_id].c_str();
                }
            }

            float distance = get_distance(pos, local_pos);

            ClientVehicleEntity* vehicle = player->GetVehicle();
            if (!g_settings.esp_draw_vehicles && IsValidPtr(vehicle))
                continue;

            float health_player = 0.f, max_health_player = 0.f, health_vehicle = 0.f, max_health_vehicle = 0.f;
            if (IsValidPtr(vehicle))
            {
                if (IsValidPtr(vehicle->m_pHealthComp) && vehicle->m_pHealthComp->m_VehicleHealth)
                    health_vehicle = vehicle->m_pHealthComp->m_VehicleHealth;

                const auto data = get_vehicle_data(vehicle);
                if (IsValidPtrWithVTable(data))
                {
                    if (data->m_MaxHealth > 0.f)
                        max_health_vehicle = data->m_MaxHealth;
                }
            }
            else if (IsValidPtr(soldier->m_pHealthComp))
            {
                health_player = soldier->m_pHealthComp->m_Health;
                max_health_player = soldier->m_pHealthComp->m_MaxHealth;
            }

            if (get_box_coords(transform, &box_coords[0]) && distance <= g_settings.esp_distance)
            {
                float box_x = box_coords[0].x;
                float box_y = box_coords[0].y;
                float box_width = box_coords[1].x - box_coords[0].x;
                float box_height = box_coords[1].y - box_coords[0].y;
                float health = IsValidPtr(vehicle) ? health_vehicle : health_player;
                float max_health = IsValidPtr(vehicle) ? max_health_vehicle : max_health_player;

                if (g_settings.esp_draw_box)
                {
                    ImColor box_color = teammate ? g_settings.esp_teammate_color : (IsValidPtr(soldier) && soldier->m_Occluded) ? g_settings.esp_box_color_occluded : g_settings.esp_box_color;
                    m_drawing->DrawEspBox(g_settings.esp_box_style, box_coords[0].x, box_coords[0].y, box_coords[1].x - box_coords[0].x, box_coords[1].y - box_coords[0].y, box_color.Value.x, box_color.Value.y, box_color.Value.z, box_color.Value.w);
                }

                if (g_settings.esp_draw_line)
                {
                    ImColor line_color = (IsValidPtr(soldier) && soldier->m_Occluded) ? g_settings.esp_line_color_occluded : g_settings.esp_line_color;
                    ImVec2 box_center = ImVec2(box_coords[0].x + box_width / 2.0f, box_coords[0].y + box_height / 2.0f);
                    ImVec2 drawing_from = ImVec2((float)g_globals.g_width / 2.0f, (float)g_globals.g_height / 2.0f);

                    switch (g_settings.esp_draw_line_from)
                    {
                    case 1:
                        drawing_from = ImVec2((float)g_globals.g_width / 2.0f, (float)g_globals.g_height);
                        break;
                    case 2:
                        drawing_from = ImVec2((float)g_globals.g_width / 2.0f, 0);
                        break;
                    case 3:
                        drawing_from = ImVec2(0, (float)g_globals.g_height / 2.0f);
                        break;
                    case 4:
                        drawing_from = ImVec2((float)g_globals.g_width, (float)g_globals.g_height / 2.0f);
                        break;
                    case 5:
                        drawing_from = ImVec2(0, 0);
                        break;
                    case 6:
                        drawing_from = ImVec2((float)g_globals.g_width, 0);
                        break;
                    case 7:
                        drawing_from = ImVec2(0, (float)g_globals.g_height);
                        break;
                    case 8:
                        drawing_from = ImVec2((float)g_globals.g_width, (float)g_globals.g_height);
                        break;
                    }

                    m_drawing->AddLine(drawing_from, box_center, line_color, g_settings.esp_line_thickness);
                }

                if (g_settings.esp_draw_health && max_health > 0.f)
                {
                    float health_ratio = health / max_health;
                    ImColor hb_color(BYTE(255 - max(health_ratio - 0.5f, 0.f) * 510), BYTE(255 - max(0.5f - health_ratio, 0.f) * 510), 0, 255);
                    float hb_width = max(box_width, 5.0f);
                    float hb_height = max(box_width / 50.0f, 3.0f);
                    float hb_width_offset = max((hb_width - box_width) / 2, 0.f);
                    float hb_height_offset = 5.0f;
                    float hb_perc_width = hb_width * health_ratio;

                    switch (g_settings.esp_health_location)
                    {
                    case 0: // Bottom
                        m_drawing->DrawBoxOutline(box_coords[0].x - hb_width_offset, box_coords[1].y + hb_height_offset, hb_width, hb_height, ImColor(0, 0, 0, 255));
                        m_drawing->DrawFillArea(box_coords[0].x - hb_width_offset, box_coords[1].y + hb_height_offset, hb_perc_width, hb_height, hb_color);
                        
                        break;
                    case 1: // Top
                        m_drawing->DrawBoxOutline(box_coords[0].x - hb_width_offset, box_coords[0].y - hb_height_offset, hb_width, hb_height, ImColor(0, 0, 0, 255));
                        m_drawing->DrawFillArea(box_coords[0].x - hb_width_offset, box_coords[0].y - hb_height_offset, hb_perc_width, hb_height, hb_color);
                        
                        break;
                    default:
                        break;
                    }
                }

                bool allow_text = !IsValidPtr(vehicle) || player->m_AttachedEntryId == 0;

                if (allow_text)
                {
                    // Better approach than what spankerfield had before
                    auto draw_esp_text = [&](float& x, float& y, const char* text, ImColor color, bool condition)
                    {
                        if (condition)
                        {
                            m_drawing->AddText(x, y, color, 14.f, FL_NONE, text);
                            y += g_settings.esp_text_spacing;
                        }
                    };

                    // Drawing part
                    ImColor text_color = teammate ? g_settings.esp_teammate_color : (IsValidPtr(soldier) && soldier->m_Occluded) ? g_settings.text_color_occluded : g_settings.text_color;

                    float x = box_coords[1].x + 3.5f;
                    float y = box_coords[0].y - 3.f;

                    draw_esp_text(x, y, nickname, text_color, g_settings.esp_draw_name);
                    draw_esp_text(x, y, fmt::format(xorstr_("{}m"), abs(ceil(distance))).c_str(), text_color, g_settings.esp_draw_distance);
                    draw_esp_text(x, y, xorstr_("VEHICLE"), g_settings.esp_additional_tags_color, g_settings.esp_draw_vehicle_tag && IsValidPtr(vehicle));
                }

                if (g_settings.skeleton)
                {
                    bool dots = g_settings.skeleton_use_dots && distance <= g_settings.skeleton_dots_distance;

                    RagdollComponent* ragdoll_component = soldier->m_pRagdollComponent;
                    if (IsValidPtr(ragdoll_component))
                    {
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::Head, UpdatePoseResultData::Neck, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::Neck, UpdatePoseResultData::Spine2, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::Spine2, UpdatePoseResultData::Spine1, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::Spine1, UpdatePoseResultData::Spine, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::Neck, UpdatePoseResultData::LeftShoulder, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::LeftShoulder, UpdatePoseResultData::LeftElbowRoll, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::RightShoulder, UpdatePoseResultData::RightElbowRoll, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::LeftElbowRoll, UpdatePoseResultData::LeftHand, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::RightElbowRoll, UpdatePoseResultData::RightHand, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::Spine, UpdatePoseResultData::RightKneeRoll, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::Spine, UpdatePoseResultData::LeftKneeRoll, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::RightKneeRoll, UpdatePoseResultData::RightFoot, dots);
                        draw_bone(g_settings.skeleton_color, ragdoll_component, UpdatePoseResultData::LeftKneeRoll, UpdatePoseResultData::LeftFoot, dots);
                    }
                }
            }
        }
    }
}