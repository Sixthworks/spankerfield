#include "draw-list.h"

std::unique_ptr<CDrawing>m_drawing = std::make_unique<CDrawing>();

void CDrawing::AddImage(const ImVec2& position, const ImVec2& size, const ImTextureID pTexture, const ImColor& color)
{
	ImRect bb(position, ImVec2(position.x + size.x, position.y + size.y));

	ImGui::GetBackgroundDrawList()->AddImage(pTexture, bb.Min, bb.Max, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImGui::ColorConvertFloat4ToU32(color));
}

void CDrawing::AddCircleFilled(const ImVec2& position, float radius, const ImColor& color, int segments)
{
	ImGui::GetBackgroundDrawList()->AddCircleFilled(position, radius, ImGui::ColorConvertFloat4ToU32(color), segments);
}

void CDrawing::AddCircle(const ImVec2& position, float radius, const ImColor& color, int segments)
{
	ImGui::GetBackgroundDrawList()->AddCircle(position, radius, ImGui::ColorConvertFloat4ToU32(color), segments);
}

void CDrawing::AddRectFilled(const ImVec2& position, const ImVec2& size, const ImColor& color, float rounding)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(position, size, ImGui::ColorConvertFloat4ToU32(color), rounding);
}

void CDrawing::AddRectFilledGradient(const ImVec2& position, const ImVec2& size, const ImColor& leftTop, const ImColor& rightTop, const ImColor& leftBot, const ImColor& rightBot)
{
	ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(position, size, (leftTop), (rightTop), (rightBot), (leftBot));
}

void CDrawing::AddRect(const ImVec2& position, const ImVec2& size, const ImColor& color, float rounding)
{
	ImGui::GetBackgroundDrawList()->AddRect(position, size, ImGui::ColorConvertFloat4ToU32(color), rounding);
}

void CDrawing::DrawFillArea(float x, float y, float w, float h, const ImColor& color, float rounding)
{
	AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
}

void CDrawing::DrawFillAreaGradient(float x, float y, float w, float h, const ImColor& leftTop, const ImColor& rightTop, const ImColor& leftBot, const ImColor& rightBot)
{
	AddRectFilledGradient(ImVec2(x, y), ImVec2(x + w, y + h), (leftTop), (rightTop), (rightBot), (leftBot));
}

void CDrawing::AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& color)
{
	ImGui::GetBackgroundDrawList()->AddTriangle(a, b, c, ImGui::ColorConvertFloat4ToU32(color));
}

void CDrawing::AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& color)
{
	ImGui::GetBackgroundDrawList()->AddTriangleFilled(a, b, c, ImGui::ColorConvertFloat4ToU32(color));
}

void CDrawing::AddLine(const ImVec2& from, const ImVec2& to, const ImColor& color, float thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine(from, to, ImGui::ColorConvertFloat4ToU32(color), thickness);
}

void CDrawing::DrawLines(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	AddLine(ImVec2((float)x0, (float)y0), ImVec2((float)x1, (float)y1), ImColor(r, g, b));
}

void CDrawing::AddText(float x, float y, const ImColor& color, float fontSize, int flags, const char* format, ...)
{
	if (!format)
		return;

	auto& io = ImGui::GetIO();
	auto DrawList = ImGui::GetBackgroundDrawList();
	auto Font = io.FontDefault;

	char szBuff[256] = { '\0' };
	va_list vlist = nullptr;
	va_start(vlist, format);
	vsprintf_s(szBuff, format, vlist);
	va_end(vlist);

	DrawList->PushTextureID(io.Fonts->TexID);

	float size_font = fontSize;

	float size = size_font == 0.f ? Font->FontSize : size_font;
	ImVec2 text_size = Font->CalcTextSizeA(size, FLT_MAX, 0.f, szBuff);

	ImColor Color = ImColor(0.f, 0.f, 0.f, color.Value.w);

	if (flags & FL_CENTER_X)
		x -= text_size.x / 2.f;

	if (flags & FL_CENTER_Y)
		y -= text_size.x / 2.f;


	DrawList->AddText(Font, size, ImVec2(x + 1.f, y + 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);

	/*
	DrawList->AddText(Font, size, ImVec2(x, y - 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
	DrawList->AddText(Font, size, ImVec2(x, y + 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
	DrawList->AddText(Font, size, ImVec2(x + 1.f, y), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
	DrawList->AddText(Font, size, ImVec2(x - 1.f, y), ImGui::ColorConvertFloat4ToU32(Color), szBuff);

	DrawList->AddText(Font, size, ImVec2(x - 1.f, y - 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
	DrawList->AddText(Font, size, ImVec2(x + 1.f, y - 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
	DrawList->AddText(Font, size, ImVec2(x - 1.f, y + 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
	DrawList->AddText(Font, size, ImVec2(x + 1.f, y + 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
	*/

	DrawList->AddText(Font, size, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), szBuff);
	DrawList->PopTextureID();
}

void CDrawing::DrawBox(float x, float y, float w, float h, const ImColor& color)
{
	AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color);
}

void CDrawing::DrawBoxOutline(float x, float y, float w, float h, const ImColor& color)
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

void CDrawing::DrawRoundBox(float x, float y, float w, float h, const ImColor& color, float rounding)
{
	AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
}

void CDrawing::DrawRoundBoxOutline(float x, float y, float w, float h, const ImColor& color, float rounding)
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

void CDrawing::DrawCornerBox(float x, float y, float w, float h, const ImColor& color)
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

void CDrawing::DrawCornerBoxOutline(float x, float y, float w, float h, const ImColor& color)
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