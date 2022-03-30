#include "math.h"

#define PI_RAD	57.295779513082
#define PI      3.14159265358979323846
#define PI_2    1.57079632679489661923
#define DEG2RAD(x)	((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)	((float)(x) * (float)(180.0f / (float)(PI)))

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

	Vector3 calc_angle(Vector3& from, const Vector3& to)
	{
		Vector3 angles = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 delta = (from - to);
		float f_hyp = sqrt((delta.x * delta.x) + (delta.y * delta.y));

		angles.x = (atanf(delta.z / f_hyp) * PI_RAD);
		angles.y = (atanf(delta.y / delta.x) * PI_RAD);
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}

	void angle_vectors(Vector3& angles, Vector3* forward)
	{
		float sp, sy, cp, cy;

		sp = sin(DEG2RAD(angles.x));
		cp = cos(DEG2RAD(angles.x));

		sy = sin(DEG2RAD(angles.y));
		cy = cos(DEG2RAD(angles.y));

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}
	}

	float calc_fov(Vector3& from, Vector3& to)
	{
		Vector3 v_src = Vector3();
		angle_vectors(from, &v_src);

		Vector3 v_dst = Vector3();
		angle_vectors(to, &v_dst);

		float result = RAD2DEG(acos(v_dst.Dot(v_src) / v_dst.LengthSquared()));

		if (!isfinite(result) || isinf(result) || isnan(result))
			result = 0.0f;

		return result;
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