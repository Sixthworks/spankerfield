#include "math.h"

#define PI_RAD	57.295779513082
#define PI      3.14159265358979323846
#define PI_2    1.57079632679489661923
#define DEG2RAD(x)	((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)	((float)(x) * (float)(180.0f / (float)(PI)))

namespace big
{
	float radians_to_degrees(float radians)
	{
		return radians * (180.0f / (float)PI);
	}

	float degrees_to_radians(float degrees)
	{
		return degrees * ((float)(PI) / 180.0f);
	}

	float get_fov_radius(float fov_degrees, float screen_width, float screen_height)
	{
		float aspect_ratio = screen_width / screen_height;
		float fov_radians = degrees_to_radians(fov_degrees);
		return (screen_height / 2) * tan(fov_radians / 2);
	}

	float get_abs_delta_at_given_points(const Vector2& src, const Vector2& dst)
	{
		Vector2 out = { src.x - dst.x, src.y - dst.y };
		return fabsf(out.Length());
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

	Vector2 clamp_magnitude(const Vector2& v, float maxLength)
	{
		float sqrMagnitude = v.x * v.x + v.y * v.y;
		if (sqrMagnitude > maxLength * maxLength)
		{
			float scale = maxLength / sqrt(sqrMagnitude);
			return Vector2(v.x * scale, v.y * scale);
		}
		return v;
	}

	float vector_dot(const Vector3& v1, const Vector3& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
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

	// Function to convert a matrix to a quaternion using SimpleMath
	Quaternion matrix_to_quaternion(const Matrix& matrix)
	{
		Quaternion result;

		// Extract the diagonal elements
		float trace = matrix._11 + matrix._22 + matrix._33;

		if (trace > 0.0f)
		{
			float S = sqrt(trace + 1.0f) * 2.0f;
			result.w = 0.25f * S;
			result.x = (matrix._23 - matrix._32) / S;
			result.y = (matrix._31 - matrix._13) / S;
			result.z = (matrix._12 - matrix._21) / S;
		}
		else if ((matrix._11 > matrix._22) && (matrix._11 > matrix._33))
		{
			float S = sqrt(1.0f + matrix._11 - matrix._22 - matrix._33) * 2.0f;
			result.w = (matrix._23 - matrix._32) / S;
			result.x = 0.25f * S;
			result.y = (matrix._12 + matrix._21) / S;
			result.z = (matrix._31 + matrix._13) / S;
		}
		else if (matrix._22 > matrix._33)
		{
			float S = sqrt(1.0f + matrix._22 - matrix._11 - matrix._33) * 2.0f;
			result.w = (matrix._31 - matrix._13) / S;
			result.x = (matrix._12 + matrix._21) / S;
			result.y = 0.25f * S;
			result.z = (matrix._23 + matrix._32) / S;
		}
		else
		{
			float S = sqrt(1.0f + matrix._33 - matrix._11 - matrix._22) * 2.0f;
			result.w = (matrix._12 - matrix._21) / S;
			result.x = (matrix._31 + matrix._13) / S;
			result.y = (matrix._23 + matrix._32) / S;
			result.z = 0.25f * S;
		}

		return result;
	}
}