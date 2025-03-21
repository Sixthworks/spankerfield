#include "esp.h"
#include "../../settings.h"
#include "../../global.h"
#include "../../SDK/sdk.h"
#include "../../Rendering/draw-list.h"
#include "../../Rendering/Maps/nicknames.h"
#include "../../Utilities/w2s.h"
#include "../../Utilities/other.h"
#include "../../Features/Friend List/friend_list.h"

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
    
    void draw_3d_box(const TransformAABBStruct& transform, const ImColor& color, float thickness = 1.0f)
    {
        // Get position and size of the object
        Vector3 pos = Vector3(transform.Transform.m[3]);
        Vector3 min = Vector3(transform.AABB.m_Min.x, transform.AABB.m_Min.y, transform.AABB.m_Min.z);
        Vector3 max = Vector3(transform.AABB.m_Max.x, transform.AABB.m_Max.y, transform.AABB.m_Max.z);
        
        // Calculate all 8 corners of the 3D box
        Vector3 corners[8];
        corners[0] = pos + multiply_mat(Vector3(min.x, min.y, min.z), &transform.Transform); // Bottom left back
        corners[1] = pos + multiply_mat(Vector3(max.x, min.y, min.z), &transform.Transform); // Bottom right back
        corners[2] = pos + multiply_mat(Vector3(max.x, min.y, max.z), &transform.Transform); // Bottom right front
        corners[3] = pos + multiply_mat(Vector3(min.x, min.y, max.z), &transform.Transform); // Bottom left front
        corners[4] = pos + multiply_mat(Vector3(min.x, max.y, min.z), &transform.Transform); // Top left back
        corners[5] = pos + multiply_mat(Vector3(max.x, max.y, min.z), &transform.Transform); // Top right back
        corners[6] = pos + multiply_mat(Vector3(max.x, max.y, max.z), &transform.Transform); // Top right front
        corners[7] = pos + multiply_mat(Vector3(min.x, max.y, max.z), &transform.Transform); // Top left front
        
        // Convert 3D coordinates to 2D for display on the screen
        Vector2 screen_corners[8];
        for (int i = 0; i < 8; i++)
        {
            if (!world_to_screen(corners[i], screen_corners[i]))
                return; // If at least one corner is not visible, don't draw the box
        }
        
        // Draw bottom face (0-1-2-3)
        m_drawing->AddLine(ImVec2(screen_corners[0].x, screen_corners[0].y), ImVec2(screen_corners[1].x, screen_corners[1].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[1].x, screen_corners[1].y), ImVec2(screen_corners[2].x, screen_corners[2].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[2].x, screen_corners[2].y), ImVec2(screen_corners[3].x, screen_corners[3].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[3].x, screen_corners[3].y), ImVec2(screen_corners[0].x, screen_corners[0].y), color, thickness);
        
        // Draw top face (4-5-6-7)
        m_drawing->AddLine(ImVec2(screen_corners[4].x, screen_corners[4].y), ImVec2(screen_corners[5].x, screen_corners[5].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[5].x, screen_corners[5].y), ImVec2(screen_corners[6].x, screen_corners[6].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[6].x, screen_corners[6].y), ImVec2(screen_corners[7].x, screen_corners[7].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[7].x, screen_corners[7].y), ImVec2(screen_corners[4].x, screen_corners[4].y), color, thickness);
        
        // Draw vertical lines connecting top and bottom faces
        m_drawing->AddLine(ImVec2(screen_corners[0].x, screen_corners[0].y), ImVec2(screen_corners[4].x, screen_corners[4].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[1].x, screen_corners[1].y), ImVec2(screen_corners[5].x, screen_corners[5].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[2].x, screen_corners[2].y), ImVec2(screen_corners[6].x, screen_corners[6].y), color, thickness);
        m_drawing->AddLine(ImVec2(screen_corners[3].x, screen_corners[3].y), ImVec2(screen_corners[7].x, screen_corners[7].y), color, thickness);
    }

    void draw_eye_tracer(ClientSoldierEntity* soldier, const ImColor& color, float distance, float thickness = 1.0f)
    {
        if (!IsValidPtr(soldier))
            return;
            
        // Get position of the head
        Vector3 head_pos;
        if (IsValidPtr(soldier->m_pRagdollComponent))
        {
            if (!soldier->m_pRagdollComponent->GetBone(UpdatePoseResultData::BONES::Head, head_pos))
                return;
        }
        else
            return;
            
        // Get transformation matrix of the soldier
        Matrix transform;
        soldier->GetTransform(&transform);
        
        // Direction of view is the forward vector from the transformation matrix
        Vector3 forward = Vector3(transform.m[2][0], transform.m[2][1], transform.m[2][2]);
        
        // Calculate the end point of the ray
        Vector3 end_pos = head_pos + (forward * distance);
        
        // Convert 3D coordinates to 2D for display on the screen
        Vector2 head_screen, end_screen;
        if (!world_to_screen(head_pos, head_screen) || !world_to_screen(end_pos, end_screen))
            return;
            
        // Draw line from head to the direction of view
        m_drawing->AddLine(ImVec2(head_screen.x, head_screen.y), ImVec2(end_screen.x, end_screen.y), color, thickness);
    }

    // For aim point
    Vector3 last_aim_point = Vector3(0, 0, 0);
    ULONGLONG last_aim_point_change_time = 0;
    bool aim_point_changed = false;

    void draw_esp()
    {
        if (!g_settings.esp) return;

        const auto game_context = ClientGameContext::GetInstance();
        if (!game_context) return;

        const auto player_manager = game_context->m_pPlayerManager;
        if (!player_manager) return;

        const auto local_player = player_manager->m_pLocalPlayer;
        if (!IsValidPtr(local_player)) return;

        const auto local_soldier = local_player->GetSoldier();
        if (!IsValidPtr(local_soldier)) return;

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            const auto player = player_manager->m_ppPlayers[i];
            if (!IsValidPtr(player) || player == local_player)
                continue;

            // Player status
            uint64_t persona_id = player->m_onlineId.m_personaid;
            bool is_friend = plugins::is_friend(persona_id);
            bool teammate = player->m_TeamId == local_player->m_TeamId;

            // Don't question such logic
            if (teammate && !g_settings.esp_draw_teammates && !(is_friend && g_settings.esp_draw_friends))
                continue;

            if (is_friend && !g_settings.esp_draw_friends && !(teammate && g_settings.esp_draw_teammates))
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
                if (persona_id != 0)
                {
                    if (streamer_personas.count(persona_id) <= 0)
                        streamer_personas[persona_id] = get_random_string(weird_nicknames);

                    nickname = streamer_personas[persona_id].empty() ? xorstr_("Unknown") : streamer_personas[persona_id].c_str();
                }
            }

            // Data
            float distance = get_distance(pos, local_pos);

            ClientVehicleEntity* vehicle = player->GetVehicle();
            if (vehicle && !g_settings.esp_draw_vehicles)
                continue;

            // https://www.unknowncheats.me/forum/battlefield-4-a/296540-vehicle-visibility-checks.html
            // You can do vehicle occlusion yourself, I will not be implementing it
            bool occluded = vehicle || (IsValidPtr(soldier) && soldier->m_Occluded);

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
                float box_width = box_coords[1].x - box_coords[0].x;
                float box_height = box_coords[1].y - box_coords[0].y;
                float health = IsValidPtr(vehicle) ? health_vehicle : health_player;
                float max_health = IsValidPtr(vehicle) ? max_health_vehicle : max_health_player;

                if (g_settings.esp_draw_3d_box)
                {
                    ImColor box_color;
                    if (is_friend && !g_settings.esp_friend_color_to_tag)
                        box_color = g_settings.esp_friend_color;
                    else if (teammate)
                        box_color = g_settings.esp_teammate_color;
                    else
                        box_color = occluded ? g_settings.esp_3d_box_color_occluded : g_settings.esp_3d_box_color;
                    
                    draw_3d_box(transform, box_color, g_settings.esp_3d_box_thickness);
                }
                
                if (g_settings.esp_draw_eye_tracer && IsValidPtr(soldier))
                {
                    ImColor tracer_color;
                    if (is_friend && !g_settings.esp_friend_color_to_tag)
                        tracer_color = g_settings.esp_friend_color;
                    else if (teammate)
                        tracer_color = g_settings.esp_teammate_color;
                    else
                        tracer_color = occluded ? g_settings.esp_eye_tracer_color_occluded : g_settings.esp_eye_tracer_color;

                    draw_eye_tracer(soldier, tracer_color, g_settings.esp_eye_tracer_distance, g_settings.esp_eye_tracer_thickness);
                }

                if (g_settings.esp_draw_box)
                {
                    if (g_settings.esp_box_fill)
                        m_drawing->DrawFillArea(box_coords[0].x, box_coords[0].y, box_coords[1].x - box_coords[0].x, box_coords[1].y - box_coords[0].y, g_settings.esp_box_fill_color, 0.0f);

                    ImColor box_color;
                    if (is_friend && !g_settings.esp_friend_color_to_tag)
                        box_color = g_settings.esp_friend_color;
                    else if (teammate)
                        box_color = g_settings.esp_teammate_color;
                    else
                        box_color = occluded ? g_settings.esp_box_color_occluded : g_settings.esp_box_color;

                    m_drawing->DrawEspBox(g_settings.esp_box_style, box_coords[0].x, box_coords[0].y, box_coords[1].x - box_coords[0].x, box_coords[1].y - box_coords[0].y, box_color.Value.x, box_color.Value.y, box_color.Value.z, box_color.Value.w);
                }

                if (g_settings.esp_draw_line)
                {
                    ImColor line_color = occluded ? g_settings.esp_line_color_occluded : g_settings.esp_line_color;
                    ImVec2 box_center = ImVec2(box_coords[0].x + box_width / 2.0f, box_coords[0].y + box_height / 2.0f);
                    ImVec2 drawing_from = ImVec2((float)g_globals.g_width / 2.0f, (float)g_globals.g_height / 2.0f);

                    switch (g_settings.esp_draw_line_from)
                    {
                    case 1:
                        drawing_from = ImVec2((float)g_globals.g_width / 2.0f, (float)g_globals.g_height);
                        break;
                    case 2:
                        drawing_from = ImVec2((float)g_globals.g_width / 2.0f, 0.f);
                        break;
                    case 3:
                        drawing_from = ImVec2(0, (float)g_globals.g_height / 2.0f);
                        break;
                    case 4:
                        drawing_from = ImVec2((float)g_globals.g_width, (float)g_globals.g_height / 2.0f);
                        break;
                    case 5:
                        drawing_from = ImVec2(0.f, 0.f);
                        break;
                    case 6:
                        drawing_from = ImVec2((float)g_globals.g_width, 0.f);
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
                    float hb_width_vertical = max(box_width / 50.0f, 3.0f);
                    float hb_height = max(box_width / 50.0f, 3.0f);
                    float hb_width_offset = max((hb_width - box_width) / 2, 0.f);
                    float hb_height_offset = 5.0f;
                    float hb_perc_width = hb_width * health_ratio;
                    float hb_perc_height = box_height * health_ratio;

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
                    case 2: // Left
                        m_drawing->DrawBoxOutline(box_coords[0].x - hb_height_offset - hb_width_vertical, box_coords[0].y, hb_width_vertical, box_height, ImColor(0, 0, 0, 255));
                        m_drawing->DrawFillArea(box_coords[0].x - hb_height_offset - hb_width_vertical, box_coords[0].y + box_height - hb_perc_height, hb_width_vertical, hb_perc_height, hb_color);
                        
                        break;
                    case 3: // Right
                        m_drawing->DrawBoxOutline(box_coords[1].x + hb_height_offset, box_coords[0].y, hb_width_vertical, box_height, ImColor(0, 0, 0, 255));
                        m_drawing->DrawFillArea(box_coords[1].x + hb_height_offset, box_coords[0].y + box_height - hb_perc_height, hb_width_vertical, hb_perc_height, hb_color);
                        break;
                    default:
                        break;
                    }
                }

                bool allow_text = !IsValidPtr(vehicle) || player->m_AttachedEntryId == 0;
                if (allow_text)
                {
                    ImColor text_color;
                    if (is_friend && !g_settings.esp_friend_color_to_tag)
                        text_color = g_settings.esp_friend_color;
                    else if (teammate)
                        text_color = g_settings.esp_teammate_color;
                    else
                        text_color = occluded ? g_settings.text_color_occluded : g_settings.text_color;

                    // Calculate position for text elements
                    float x, y;
                    int text_flags = FL_SHADOW;

                    // Count total text elements
                    const std::vector<bool> text_elements =
                    {
                        g_settings.esp_draw_name,
                        g_settings.esp_draw_distance,
                        is_friend && g_settings.esp_friend_color_to_tag,
                        g_settings.esp_draw_vehicle_tag && IsValidPtr(vehicle)
                    };

                    __int64 text_count = std::count(text_elements.begin(), text_elements.end(), true);
                    float total_height = text_count * g_settings.esp_text_spacing;

                    // Set position based on configuration (0 = right, 1 = top, 2 = bottom)
                    switch (g_settings.esp_text_position)
                    {
                    case 1: // Top
                        x = box_coords[0].x + (box_coords[1].x - box_coords[0].x) / 2.f;
                        y = box_coords[0].y - total_height - 9.5f;
                        text_flags = FL_CENTER_X | FL_SHADOW;

                        break;
                    case 2: // Bottom
                        x = box_coords[0].x + (box_coords[1].x - box_coords[0].x) / 2.f;
                        y = box_coords[1].y + 2.5f;
                        text_flags = FL_CENTER_X | FL_SHADOW;

                        break;
                    default: // Right (0)
                        x = box_coords[1].x + 3.5f;
                        y = box_coords[0].y - 3.f;

                        if (g_settings.esp_draw_health && g_settings.esp_health_location == 3)
                            x += 9.0f;

                        break;
                    }

                    // Helper function to draw text if condition is met
                    auto draw_text_if = [&](bool condition, const char* text, const ImColor& color)
                    {
                        if (condition)
                        {
                            m_drawing->AddText(x, y, color, 14.f, text_flags, text);
                            y += g_settings.esp_text_spacing;
                        }
                    };

                    // Draw text elements using the helper function
                    draw_text_if(g_settings.esp_draw_name, nickname, text_color);
                    draw_text_if(g_settings.esp_draw_distance, fmt::format(xorstr_("{}m"), abs(ceil(distance))).c_str(), text_color);
                    draw_text_if(is_friend && g_settings.esp_friend_color_to_tag, xorstr_("FRND"), g_settings.esp_friend_color);
                    draw_text_if(g_settings.esp_draw_vehicle_tag && IsValidPtr(vehicle), xorstr_("VEH"), g_settings.esp_additional_tags_color);
                }

                // Aim Point, credit VincentVega
                if (g_settings.esp_draw_aim_point)
                {
                    if (g_globals.g_has_pred_aim_point)
                    {
                        ULONGLONG current_time = GetTickCount64();

                        // Check if aim point has changed
                        if (g_globals.g_pred_aim_point.x != last_aim_point.x ||
                            g_globals.g_pred_aim_point.y != last_aim_point.y ||
                            g_globals.g_pred_aim_point.z != last_aim_point.z)
                        {
                            // Aim point has changed, update last position and time
                            last_aim_point = g_globals.g_pred_aim_point;
                            last_aim_point_change_time = current_time;
                            aim_point_changed = true;
                        }

                        // Only draw if less than 500ms has passed since last change
                        if (aim_point_changed && (current_time - last_aim_point_change_time <= 500))
                        {
                            Vector2 aimpoint_screen_coords;
                            if (world_to_screen(g_globals.g_pred_aim_point, aimpoint_screen_coords))
                            {
                                // Calculate alpha based on time elapsed
                                float alpha_multiplier = 1.0f;

                                // Start fading out
                                if (current_time - last_aim_point_change_time > 300)
                                {
                                    alpha_multiplier = 1.0f - ((current_time - last_aim_point_change_time - 300) / 200.0f);
                                    if (alpha_multiplier < 0.0f) alpha_multiplier = 0.0f;
                                }

                                // Apply alpha to color
                                ImColor aim_point_color = g_settings.esp_aim_point_color;
                                aim_point_color.Value.w *= alpha_multiplier;

                                m_drawing->AddCircleFilled(
                                    ImVec2(aimpoint_screen_coords.x, aimpoint_screen_coords.y),
                                    g_settings.esp_aim_point_size,
                                    aim_point_color);
                            }
                        }
                    }
                    else
                    {
                        // Reset changed flag if we don't have a prediction point
                        aim_point_changed = false;
                    }
                }

                if (g_settings.skeleton)
                {
                    bool dots = g_settings.skeleton_use_dots && distance <= g_settings.skeleton_dots_distance;

                    RagdollComponent* ragdoll_component = soldier->m_pRagdollComponent;
                    if (IsValidPtr(ragdoll_component))
                    {
                        ImColor skeleton_color;
                        if (is_friend && !g_settings.esp_friend_color_to_tag)
                            skeleton_color = g_settings.esp_friend_color;
                        else if (teammate)
                            skeleton_color = g_settings.esp_teammate_color;
                        else
                            skeleton_color = occluded ? g_settings.skeleton_color_occluded : g_settings.skeleton_color;

                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::Head, UpdatePoseResultData::Neck, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::Neck, UpdatePoseResultData::Spine2, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::Spine2, UpdatePoseResultData::Spine1, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::Spine1, UpdatePoseResultData::Spine, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::Neck, UpdatePoseResultData::LeftShoulder, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::LeftShoulder, UpdatePoseResultData::LeftElbowRoll, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::RightShoulder, UpdatePoseResultData::RightElbowRoll, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::LeftElbowRoll, UpdatePoseResultData::LeftHand, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::RightElbowRoll, UpdatePoseResultData::RightHand, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::Spine, UpdatePoseResultData::RightKneeRoll, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::Spine, UpdatePoseResultData::LeftKneeRoll, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::RightKneeRoll, UpdatePoseResultData::RightFoot, dots);
                        draw_bone(skeleton_color, ragdoll_component, UpdatePoseResultData::LeftKneeRoll, UpdatePoseResultData::LeftFoot, dots);
                    }
                }
            }
        }
    }
}