#include "draw-list.h"
#include "../SDK/sdk.h"
#include "../settings.h"

using namespace big;

std::unique_ptr<CDrawing>m_drawing = std::make_unique<CDrawing>();

// Conversion from ImColor to Color32
static Color32 FromImColor(const ImColor& color)
{
    ImVec4 c = color;
    return Color32(
        static_cast<BYTE>(c.x * 255.0f),
        static_cast<BYTE>(c.y * 255.0f),
        static_cast<BYTE>(c.z * 255.0f),
        static_cast<BYTE>(c.w * 255.0f)
    );
}

void CDrawing::AddImage(const ImVec2& position, const ImVec2& size, const ImTextureID pTexture, const ImColor& color)
{
	ImRect bb(position, ImVec2(position.x + size.x, position.y + size.y));

	ImGui::GetBackgroundDrawList()->AddImage(pTexture, bb.Min, bb.Max, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImGui::ColorConvertFloat4ToU32(color));
}

void CDrawing::AddCircleFilled(const ImVec2& position, float radius, const ImColor& color, int segments)
{
    if (g_settings.use_cheap_boxes)
    {
        // This is a simplified approach as DebugRenderer2 doesn't have direct circle support
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        Color32 col = FromImColor(color);

        // Approximate a filled circle with multiple rectangles or triangles
        // This is a simplistic approach; a better implementation would use more segments
        const float step = 2.0f * 3.14159f / segments;
        for (int i = 0; i < segments; i++)
        {
            float angle1 = i * step;
            float angle2 = (i + 1) * step;
            float x1 = position.x + cosf(angle1) * radius;
            float y1 = position.y + sinf(angle1) * radius;
            float x2 = position.x + cosf(angle2) * radius;
            float y2 = position.y + sinf(angle2) * radius;

            renderer->drawLine2d(position.x, position.y, x1, y1, col);
            renderer->drawLine2d(position.x, position.y, x2, y2, col);
            renderer->drawLine2d(x1, y1, x2, y2, col);
        }
    }
    else
        ImGui::GetBackgroundDrawList()->AddCircleFilled(position, radius, ImGui::ColorConvertFloat4ToU32(color), segments);
}

void CDrawing::AddCircle(const ImVec2& position, float radius, const ImColor& color, int segments)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        Color32 col = FromImColor(color);

        // Draw a circle outline using lines
        const float step = 2.0f * 3.14159f / segments;
        for (int i = 0; i < segments; i++)
        {
            float angle1 = i * step;
            float angle2 = (i + 1) * step;
            float x1 = position.x + cosf(angle1) * radius;
            float y1 = position.y + sinf(angle1) * radius;
            float x2 = position.x + cosf(angle2) * radius;
            float y2 = position.y + sinf(angle2) * radius;

            renderer->drawLine2d(x1, y1, x2, y2, col);
        }
    }
    else
        ImGui::GetBackgroundDrawList()->AddCircle(position, radius, ImGui::ColorConvertFloat4ToU32(color), segments);
}

// Rectangle functions
void CDrawing::AddRectFilled(const ImVec2& position, const ImVec2& size, const ImColor& color, float rounding)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        renderer->drawRect2d(position.x, position.y, size.x, size.y, FromImColor(color));
    }
    else
        ImGui::GetBackgroundDrawList()->AddRectFilled(position, size, ImGui::ColorConvertFloat4ToU32(color), rounding);
}

void CDrawing::AddRectFilledGradient(const ImVec2& position, const ImVec2& size, const ImColor& leftTop, const ImColor& rightTop, const ImColor& leftBot, const ImColor& rightBot)
{
    if (g_settings.use_cheap_boxes)
    {
        // DebugRenderer2 doesn't support gradient fills directly
        // Fallback to using the average color
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        ImVec4 avgColor = ImVec4(
            (leftTop.Value.x + rightTop.Value.x + leftBot.Value.x + rightBot.Value.x) / 4.0f,
            (leftTop.Value.y + rightTop.Value.y + leftBot.Value.y + rightBot.Value.y) / 4.0f,
            (leftTop.Value.z + rightTop.Value.z + leftBot.Value.z + rightBot.Value.z) / 4.0f,
            (leftTop.Value.w + rightTop.Value.w + leftBot.Value.w + rightBot.Value.w) / 4.0f
        );
        ImColor avgImColor = ImColor(avgColor);
        renderer->drawRect2d(position.x, position.y, size.x, size.y, FromImColor(avgImColor));
    }
    else
        ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(position, size, ImGui::ColorConvertFloat4ToU32(leftTop), ImGui::ColorConvertFloat4ToU32(rightTop), ImGui::ColorConvertFloat4ToU32(rightBot), ImGui::ColorConvertFloat4ToU32(leftBot));
}

void CDrawing::AddRect(const ImVec2& position, const ImVec2& size, const ImColor& color, float rounding)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        float lineWidth = 1.0f; // Default line width
        renderer->drawBox2d(position.x, position.y, size.x, size.y, lineWidth, FromImColor(color));
    }
    else
        ImGui::GetBackgroundDrawList()->AddRect(position, size, ImGui::ColorConvertFloat4ToU32(color), rounding);
}

// Convenience wrappers
void CDrawing::DrawFillArea(float x, float y, float w, float h, const ImColor& color, float rounding)
{
    AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
}

void CDrawing::DrawFillAreaGradient(float x, float y, float w, float h, const ImColor& leftTop, const ImColor& rightTop, const ImColor& leftBot, const ImColor& rightBot)
{
    AddRectFilledGradient(ImVec2(x, y), ImVec2(x + w, y + h), leftTop, rightTop, rightBot, leftBot);
}

// Triangle functions
void CDrawing::AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& color)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        Color32 col = FromImColor(color);
        renderer->drawLine2d(a.x, a.y, b.x, b.y, col);
        renderer->drawLine2d(b.x, b.y, c.x, c.y, col);
        renderer->drawLine2d(c.x, c.y, a.x, a.y, col);
    }
    else
        ImGui::GetBackgroundDrawList()->AddTriangle(a, b, c, ImGui::ColorConvertFloat4ToU32(color));
}

void CDrawing::AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& color)
{
    if (g_settings.use_cheap_boxes)
    {
        // Approximate filled triangle with lines (not ideal but works as fallback)
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        Color32 col = FromImColor(color);

        // Triangle outline
        renderer->drawLine2d(a.x, a.y, b.x, b.y, col);
        renderer->drawLine2d(b.x, b.y, c.x, c.y, col);
        renderer->drawLine2d(c.x, c.y, a.x, a.y, col);

        // Fill with some inner lines (basic approximation)
        ImVec2 center = ImVec2((a.x + b.x + c.x) / 3.0f, (a.y + b.y + c.y) / 3.0f);
        renderer->drawLine2d(center.x, center.y, a.x, a.y, col);
        renderer->drawLine2d(center.x, center.y, b.x, b.y, col);
        renderer->drawLine2d(center.x, center.y, c.x, c.y, col);
    }
    else
        ImGui::GetBackgroundDrawList()->AddTriangleFilled(a, b, c, ImGui::ColorConvertFloat4ToU32(color));
}

// Line functions
void CDrawing::AddLine(const ImVec2& from, const ImVec2& to, const ImColor& color, float thickness)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        renderer->drawLine2d(from.x, from.y, to.x, to.y, FromImColor(color));
    }
    else
        ImGui::GetBackgroundDrawList()->AddLine(from, to, ImGui::ColorConvertFloat4ToU32(color), thickness);
}

void CDrawing::DrawLines(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
    AddLine(ImVec2((float)x0, (float)y0), ImVec2((float)x1, (float)y1), ImColor(r, g, b, a));
}

// Text drawing (added function

void CDrawing::AddText(float x, float y, const ImColor& color, float font_size, int flags, const char* format, ...)
{
    if (!format)
        return;

    // Format the text with variable arguments
    char sz_buff[256] = { '\0' };
    va_list vlist = nullptr;
    va_start(vlist, format);
    vsprintf_s(sz_buff, format, vlist);
    va_end(vlist);

    // Check if we should use the DebugRenderer2 text rendering
    if (g_settings.use_cheap_text)
    {
        // Get DebugRenderer2 instance
        DebugRenderer2* debug_renderer = DebugRenderer2::GetInstance();
        if (debug_renderer)
        {
            // Calculate position adjustments for centering if needed
            float text_x = x;
            float text_y = y;

            // Convert ImColor to Color32 for DebugRenderer2
            Color32 debug_color = FromImColor(color);

            if (flags & FL_CENTER_X || flags & FL_CENTER_Y)
            {
                // For centering, we need to calculate text dimensions
                // This is an approximation since we don't have direct access to DebugRenderer's font metrics
                float approx_char_width = font_size * 0.4f; // Approximate width per character
                float approx_text_width = strlen(sz_buff) * approx_char_width;
                float approx_text_height = font_size;

                if (flags & FL_CENTER_X)
                    text_x -= approx_text_width / 2.0f;
                if (flags & FL_CENTER_Y)
                    text_y -= approx_text_height / 2.0f;
            }

            // Calculate a better scale factor to match ImGui text size
            float scale_factor = font_size / 14.0f; // 16.f is similar but the text starts looking super bad

            // Draw the text using DebugRenderer2
            Vector2 screen_pos(text_x, text_y);
            debug_renderer->drawText(screen_pos, debug_color, sz_buff, scale_factor);
            return;
        }
    }

    // Fallback to original ImGui rendering if DebugRenderer isn't available or disabled
    auto& io = ImGui::GetIO();
    auto draw_list = ImGui::GetBackgroundDrawList();
    
    auto font = io.FontDefault;
    draw_list->PushTextureID(io.Fonts->TexID);
    
    float size = font_size == 0.f ? font->FontSize : font_size;
    ImVec2 text_size = font->CalcTextSizeA(size, FLT_MAX, 0.f, sz_buff);

    // Handle centering
    if (flags & FL_CENTER_X)
        x -= text_size.x / 2.f;

    if (flags & FL_CENTER_Y)
        y -= text_size.y / 2.f;

    // Handle shadow
    if (flags & FL_SHADOW)
    {
        ImColor shadow_color = ImColor(0.f, 0.f, 0.f, color.Value.w);
        draw_list->AddText(font, size, ImVec2(x + 1.f, y + 1.f), ImGui::ColorConvertFloat4ToU32(shadow_color), sz_buff);
    }

    // Handle outline
    if (flags & FL_OUTLINE)
    {
        ImColor outline_color = ImColor(0.f, 0.f, 0.f, color.Value.w);
        draw_list->AddText(font, size, ImVec2(x - 1.f, y), ImGui::ColorConvertFloat4ToU32(outline_color), sz_buff);
        draw_list->AddText(font, size, ImVec2(x + 1.f, y), ImGui::ColorConvertFloat4ToU32(outline_color), sz_buff);
        draw_list->AddText(font, size, ImVec2(x, y - 1.f), ImGui::ColorConvertFloat4ToU32(outline_color), sz_buff);
        draw_list->AddText(font, size, ImVec2(x, y + 1.f), ImGui::ColorConvertFloat4ToU32(outline_color), sz_buff);
    }

    // Draw the main text
    draw_list->AddText(font, size, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), sz_buff);
    draw_list->PopTextureID();
}

void CDrawing::DrawBox(float x, float y, float w, float h, const ImColor& color)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();

        // Convert ImColor float values (0-1) to BYTE values (0-255)
        BYTE r = static_cast<BYTE>(color.Value.x * 255.0f);
        BYTE g = static_cast<BYTE>(color.Value.y * 255.0f);
        BYTE b = static_cast<BYTE>(color.Value.z * 255.0f);
        BYTE a = static_cast<BYTE>(color.Value.w * 255.0f);
        Color32 debugColor(r, g, b, a);

        renderer->drawLineRect2d(x, y, x + w, y + h, debugColor);
    }
    else
    {
        AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color);
    }
}

void CDrawing::DrawBoxOutline(float x, float y, float w, float h, const ImColor& color)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();

        // Convert ImColor float values (0-1) to BYTE values (0-255)
        BYTE a = static_cast<BYTE>(color.Value.w * 255.0f);
        BYTE r = static_cast<BYTE>(color.Value.x * 255.0f);
        BYTE g = static_cast<BYTE>(color.Value.y * 255.0f);
        BYTE b = static_cast<BYTE>(color.Value.z * 255.0f);

        Color32 outlineColor(0, 0, 0, a);
        Color32 mainColor(r, g, b, a);

        // Draw black outline
        renderer->drawLineRect2d(x - 1.f, y - 1.f, x + w + 1.f, y + h + 1.f, outlineColor);

        // Draw main box
        renderer->drawLineRect2d(x, y, x + w, y + h, mainColor);
    }
    else
    {
        DrawBox(x + 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawBox(x - 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawBox(x - 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawBox(x + 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawBox(x + 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawBox(x - 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawBox(x, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawBox(x, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawBox(x, y, w, h, color);
    }
}

void CDrawing::DrawRoundBox(float x, float y, float w, float h, const ImColor& color, float rounding)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();

        // Convert ImColor float values (0-1) to BYTE values (0-255)
        BYTE r = static_cast<BYTE>(color.Value.x * 255.0f);
        BYTE g = static_cast<BYTE>(color.Value.y * 255.0f);
        BYTE b = static_cast<BYTE>(color.Value.z * 255.0f);
        BYTE a = static_cast<BYTE>(color.Value.w * 255.0f);
        Color32 debugColor(r, g, b, a);

        renderer->drawLineRect2d(x, y, x + w, y + h, debugColor);
    }
    else
        AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
}

void CDrawing::DrawRoundBoxOutline(float x, float y, float w, float h, const ImColor& color, float rounding)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();

        // Convert ImColor float values (0-1) to BYTE values (0-255)
        BYTE r = static_cast<BYTE>(color.Value.x * 255.0f);
        BYTE g = static_cast<BYTE>(color.Value.y * 255.0f);
        BYTE b = static_cast<BYTE>(color.Value.z * 255.0f);
        BYTE a = static_cast<BYTE>(color.Value.w * 255.0f);

        Color32 outlineColor(0, 0, 0, a);
        Color32 mainColor(r, g, b, a);

        // Draw black outline
        renderer->drawLineRect2d(x - 1.f, y - 1.f, x + w + 1.f, y + h + 1.f, outlineColor);

        // Draw main box
        renderer->drawLineRect2d(x, y, x + w, y + h, mainColor);
    }
    else
    {
        DrawRoundBox(x + 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
        DrawRoundBox(x - 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
        DrawRoundBox(x - 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
        DrawRoundBox(x + 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
        DrawRoundBox(x + 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
        DrawRoundBox(x - 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
        DrawRoundBox(x, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
        DrawRoundBox(x, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
        DrawRoundBox(x, y, w, h, color, rounding);
    }
}

void CDrawing::DrawCornerBox(float x, float y, float w, float h, const ImColor& color)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();

        // Convert ImColor float values (0-1) to BYTE values (0-255)
        BYTE r = static_cast<BYTE>(color.Value.x * 255.0f);
        BYTE g = static_cast<BYTE>(color.Value.y * 255.0f);
        BYTE b = static_cast<BYTE>(color.Value.z * 255.0f);
        BYTE a = static_cast<BYTE>(color.Value.w * 255.0f);
        Color32 debugColor(r, g, b, a);

        // Top-left corner
        renderer->drawLine2d(x, y, x + w / 4.f, y, debugColor);
        renderer->drawLine2d(x, y, x, y + h / 4.f, debugColor);

        // Top-right corner
        renderer->drawLine2d(x + w, y, x + w - w / 4.f, y, debugColor);
        renderer->drawLine2d(x + w, y, x + w, y + h / 4.f, debugColor);

        // Bottom-left corner
        renderer->drawLine2d(x, y + h, x + w / 4.f, y + h, debugColor);
        renderer->drawLine2d(x, y + h, x, y + h - h / 4.f, debugColor);

        // Bottom-right corner
        renderer->drawLine2d(x + w, y + h, x + w - w / 4.f, y + h, debugColor);
        renderer->drawLine2d(x + w, y + h, x + w, y + h - h / 4.f, debugColor);
    }
    else
    {
        AddLine(ImVec2(x, y), ImVec2(x + w / 4.f, y), color);
        AddLine(ImVec2(x, y), ImVec2(x, y + h / 4.f), color);
        AddLine(ImVec2(x + w, y), ImVec2(x + w - w / 4.f, y), color);
        AddLine(ImVec2(x + w, y), ImVec2(x + w, y + h / 4.f), color);
        AddLine(ImVec2(x, y + h), ImVec2(x + w / 4.f, y + h), color);
        AddLine(ImVec2(x, y + h), ImVec2(x, y + h - h / 4.f), color);
        AddLine(ImVec2(x + w, y + h), ImVec2(x + w, y + h - h / 4.f), color);
        AddLine(ImVec2(x + w, y + h), ImVec2(x + w - w / 4.f, y + h), color);
    }
}

void CDrawing::DrawCornerBoxOutline(float x, float y, float w, float h, const ImColor& color)
{
    if (g_settings.use_cheap_boxes)
    {
        DebugRenderer2* renderer = DebugRenderer2::GetInstance();
        // Convert ImColor float values (0-1) to BYTE values (0-255)
        BYTE r = static_cast<BYTE>(color.Value.x * 255.0f);
        BYTE g = static_cast<BYTE>(color.Value.y * 255.0f);
        BYTE b = static_cast<BYTE>(color.Value.z * 255.0f);
        BYTE a = static_cast<BYTE>(color.Value.w * 255.0f);

        Color32 outlineColor(0, 0, 0, a);
        Color32 mainColor(r, g, b, a);

        // Draw black outline corners
        // Top-left corner
        renderer->drawLine2d(x - 1.f, y - 1.f, x + w / 4.f, y - 1.f, outlineColor);
        renderer->drawLine2d(x - 1.f, y - 1.f, x - 1.f, y + h / 4.f, outlineColor);

        // Top-right corner
        renderer->drawLine2d(x + w + 1.f, y - 1.f, x + w - w / 4.f, y - 1.f, outlineColor);
        renderer->drawLine2d(x + w + 1.f, y - 1.f, x + w + 1.f, y + h / 4.f, outlineColor);

        // Bottom-left corner
        renderer->drawLine2d(x - 1.f, y + h + 1.f, x + w / 4.f, y + h + 1.f, outlineColor);
        renderer->drawLine2d(x - 1.f, y + h + 1.f, x - 1.f, y + h - h / 4.f, outlineColor);

        // Bottom-right corner
        renderer->drawLine2d(x + w + 1.f, y + h + 1.f, x + w - w / 4.f, y + h + 1.f, outlineColor);
        renderer->drawLine2d(x + w + 1.f, y + h + 1.f, x + w + 1.f, y + h - h / 4.f, outlineColor);

        // Draw main colored corners
        // Top-left corner
        renderer->drawLine2d(x, y, x + w / 4.f, y, mainColor);
        renderer->drawLine2d(x, y, x, y + h / 4.f, mainColor);

        // Top-right corner
        renderer->drawLine2d(x + w, y, x + w - w / 4.f, y, mainColor);
        renderer->drawLine2d(x + w, y, x + w, y + h / 4.f, mainColor);

        // Bottom-left corner
        renderer->drawLine2d(x, y + h, x + w / 4.f, y + h, mainColor);
        renderer->drawLine2d(x, y + h, x, y + h - h / 4.f, mainColor);

        // Bottom-right corner
        renderer->drawLine2d(x + w, y + h, x + w - w / 4.f, y + h, mainColor);
        renderer->drawLine2d(x + w, y + h, x + w, y + h - h / 4.f, mainColor);
    }
    else
    {
        DrawCornerBox(x - 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawCornerBox(x - 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawCornerBox(x + 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawCornerBox(x + 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawCornerBox(x - 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawCornerBox(x, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawCornerBox(x, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawCornerBox(x + 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
        DrawCornerBox(x, y, w, h, color);
    }
}

void CDrawing::DrawEspBox(int box_type, float x, float y, float w, float h, float r, float g, float b, float a)
{
	switch (box_type)
	{
	case 1:
		DrawBox(x, y, w, h, ImColor(r, g, b, a));
		break;
	case 2:
		DrawBoxOutline(x, y, w, h, ImColor(r, g, b, a));
		break;
	case 3:
		DrawCornerBox(x, y, w, h, ImColor(r, g, b, a));
		break;
	case 4:
		DrawCornerBoxOutline(x, y, w, h, ImColor(r, g, b, a));
		break;
	case 5:
		DrawRoundBox(x, y, w, h, ImColor(r, g, b, a), 3.f);
		break;
	case 6:
		DrawRoundBoxOutline(x, y, w, h, ImColor(r, g, b, a), 3.f);
		break;
	}
}

void CDrawing::DrawDot(int x, int y, const ImColor& color)
{
	float thickness = 4.f;

	DrawFillArea(x - thickness, y - thickness, thickness + 1.f, thickness + 1.f, color);
}