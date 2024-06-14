#pragma once
#include "../SDK/sdk.h"

namespace big
{
	float degrees_to_radians(float degrees);
	float get_fov_radius(float fov_degrees, float screen_width, float screen_height);
	void normalize_angle(Vector2& angle);
	void cerp_angle(Vector2& from, Vector2& to, float step_x, float step_y);
	void get_eye_location(Matrix& matrix);
	Vector2 get_screen_size();
}