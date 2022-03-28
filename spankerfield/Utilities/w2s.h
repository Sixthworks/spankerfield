#pragma once
#include "../SDK/sdk.h"

namespace big
{
	extern bool world_to_screen(const Vector3& pos, Vector2& out);
	extern bool world_to_screen(const Vector3& origin, Vector3& screen);
	extern bool world_to_screen(Vector3& pos);
	Vector3 multiply_mat(const Vector3& vec, const Matrix* mat);
	extern bool get_box_coords(const TransformAABBStruct& TransAABB, Vector2* cords);
}