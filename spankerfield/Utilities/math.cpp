#include "math.h"

#define PI      3.14159265358979323846
#define PI_2    1.57079632679489661923

namespace big
{
	void normalize_angle(Vector2& angle)
	{
		if (angle.x <= -PI)
			angle.x += 2 * PI;
		if (angle.x > PI)
			angle.x -= 2 * PI;
		if (angle.y <= -PI_2)
			angle.y += PI;
		if (angle.y > PI_2)
			angle.y -= PI;
	}

	void lerp_angle(Vector2& from, Vector2& to, float step)
	{
		Vector2 delta = (to - from);
		normalize_angle(delta);
		to = (from + step * delta);
		normalize_angle(to);
	}

	void cerp_angle(Vector2& from, Vector2& to, float step_x, float step_y)
	{
		float CubicStepX = (1 - cos(step_x * PI)) / 2;
		float CubicStepY = (1 - cos(step_y * PI)) / 2;

		Vector2 Delta = (to - from);
		normalize_angle(Delta);

		to.x = (from.x + CubicStepX * Delta.x);
		to.y = (from.y + CubicStepY * Delta.y);

		normalize_angle(to);
	}

	void get_eye_location(Matrix& matrix)
	{
		const auto game_renderer = GameRenderer::GetInstance();
		if (!game_renderer)
			return;

		const auto render_view = game_renderer->m_pRenderView;
		if (!render_view)
			return;

		matrix = render_view->m_ViewInverse;
	}

	Vector2 get_screen_size()
	{
		const auto dx_renderer = DxRenderer::GetInstance();
		if (!dx_renderer)
			return Vector2(0, 0);

		const auto screen = dx_renderer->m_pScreen;
		if (!screen)
			return Vector2(0, 0);

		return Vector2(screen->m_Width, screen->m_Height);
	}
}