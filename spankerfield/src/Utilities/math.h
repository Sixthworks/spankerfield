#pragma once
#include "../SDK/sdk.h"

namespace big
{
	float radians_to_degrees(float radians);
	float degrees_to_radians(float degrees);
	float get_fov_radius(float fov_degrees, float screen_width, float screen_height);
	float get_abs_delta_at_given_points(const Vector2& src, const Vector2& dst);
	void normalize_angle(Vector2& angle);
	void cerp_angle(Vector2& from, Vector2& to, float step_x, float step_y);
	Vector2 clamp_magnitude(const Vector2& v, float maxLength);
	float vector_dot(const Vector3& v1, const Vector3& v2);
	Vector2 get_screen_size();
	Quaternion matrix_to_quaternion(const Matrix& matrix);
}