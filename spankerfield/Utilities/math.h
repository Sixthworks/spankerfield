#pragma once
#include "../SDK/sdk.h"

namespace big
{
	void normalize_angle(Vector2& angle);
	void lerp_angle(Vector2& from, Vector2& to, float step);
	Vector3 calc_angle(Vector3& from, const Vector3& to);
	void angle_vectors(Vector3& angles, Vector3* forward);
	float calc_fov(Vector3& from, Vector3& to);
	void cerp_angle(Vector2& from, Vector2& to, float step_x, float step_y);
	void get_eye_location(Matrix& matrix);
	Vector2 get_screen_size();
}