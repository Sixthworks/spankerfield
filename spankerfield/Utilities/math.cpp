#include "math.h"

#define PI_RAD	57.295779513082
#define PI      3.14159265358979323846
#define PI_2    1.57079632679489661923
#define DEG2RAD(x)	((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)	((float)(x) * (float)(180.0f / (float)(PI)))

namespace big
{
	float degrees_to_radians(float degrees)
	{
		return degrees * ((float)(PI) / 180.0f);
	}

	float get_fov_radius(float fov_degrees, float screen_width, float screen_height)
	{
		float aspect_ratio = screen_width / screen_height;
		float fov_radians = degrees_to_radians(fov_degrees);
		return tan(fov_radians / 2) * (screen_width / 2) / aspect_ratio;
	}

	void normalize_angle(Vector2& angle)
	{
		if (angle.x <= (float)(-PI))
			angle.x += 2.0f * (float)(PI);
		if (angle.x > (float)(PI))
			angle.x -= 2.0f * (float)(PI);
		if (angle.y <= (float)(-PI_2))
			angle.y += (float)(PI);
		if (angle.y > (float)(PI_2))
			angle.y -= (float)(PI);
	}

	void cerp_angle(Vector2& from, Vector2& to, float step_x, float step_y)
	{
		float CubicStepX = (1 - cos(step_x * (float)(PI))) / 2;
		float CubicStepY = (1 - cos(step_y * (float)(PI))) / 2;

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

		return Vector2((float)screen->m_Width, (float)screen->m_Height);
	}
}