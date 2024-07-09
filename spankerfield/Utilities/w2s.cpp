#include "w2s.h"
#include "other.h"
#include "../global.h"

namespace big
{
	bool world_to_screen(const Vector3& pos, Vector2& out)
	{
		float w = g_globals.g_viewproj.m[0][3] * pos.x + g_globals.g_viewproj.m[1][3] * pos.y + g_globals.g_viewproj.m[2][3] * pos.z + g_globals.g_viewproj.m[3][3];
		if (w < 0.19)
			return false;

		float x = g_globals.g_viewproj.m[0][0] * pos.x + g_globals.g_viewproj.m[1][0] * pos.y + g_globals.g_viewproj.m[2][0] * pos.z + g_globals.g_viewproj.m[3][0];
		float y = g_globals.g_viewproj.m[0][1] * pos.x + g_globals.g_viewproj.m[1][1] * pos.y + g_globals.g_viewproj.m[2][1] * pos.z + g_globals.g_viewproj.m[3][1];

		auto hWidth = static_cast<float>(g_globals.g_width) / 2.0f;
		auto hHeight = static_cast<float>(g_globals.g_height) / 2.0f;

		auto out_x = hWidth + hWidth * x / w;
		auto out_y = hHeight - hHeight * y / w;

		if (out_x >= 0.0f && out_x < g_globals.g_width && out_y >= 0.0f && out_y < g_globals.g_height) {

			out.x = out_x;
			out.y = out_y;
			//out->z = w;

			return true;
		}
		return false;
	}


	bool world_to_screen(const Vector3& origin, Vector3& screen)
	{
		float mX = static_cast<float>(g_globals.g_width * 0.5f);
		float mY = static_cast<float>(g_globals.g_height * 0.5f);

		float w = g_globals.g_viewproj(0, 3) * origin.x + g_globals.g_viewproj(1, 3) * origin.y + g_globals.g_viewproj(2, 3) * origin.z + g_globals.g_viewproj(3, 3);

		if (w < 0.0001f)
		{
			screen.z = w;
			return false;
		}

		float x = g_globals.g_viewproj(0, 0) * origin.x + g_globals.g_viewproj(1, 0) * origin.y + g_globals.g_viewproj(2, 0) * origin.z + g_globals.g_viewproj(3, 0);
		float y = g_globals.g_viewproj(0, 1) * origin.x + g_globals.g_viewproj(1, 1) * origin.y + g_globals.g_viewproj(2, 1) * origin.z + g_globals.g_viewproj(3, 1);

		screen.x = mX + mX * x / w;
		screen.y = mY - mY * y / w;
		screen.z = w;

		return true;
	}

	bool world_to_screen(Vector3& pos)
	{
		return world_to_screen(pos, reinterpret_cast<Vector2&>(pos));
	}

	Vector3 multiply_mat(const Vector3& vec, const Matrix* mat)
	{
		return Vector3(mat->_11 * vec.x + mat->_21 * vec.y + mat->_31 * vec.z,
			mat->_12 * vec.x + mat->_22 * vec.y + mat->_32 * vec.z,
			mat->_13 * vec.x + mat->_23 * vec.y + mat->_33 * vec.z);
	}

	bool get_box_coords(const TransformAABBStruct& TransAABB, Vector2* cords)
	{
		Vector3 corners[8];
		Vector3 pos = (Vector3)TransAABB.Transform.m[3];
		Vector3 min = Vector3(TransAABB.AABB.m_Min.x, TransAABB.AABB.m_Min.y, TransAABB.AABB.m_Min.z);
		Vector3 max = Vector3(TransAABB.AABB.m_Max.x, TransAABB.AABB.m_Max.y, TransAABB.AABB.m_Max.z);
		corners[2] = pos + multiply_mat(Vector3(max.x, min.y, min.z), &TransAABB.Transform);
		corners[3] = pos + multiply_mat(Vector3(max.x, min.y, max.z), &TransAABB.Transform);
		corners[4] = pos + multiply_mat(Vector3(min.x, min.y, max.z), &TransAABB.Transform);
		corners[5] = pos + multiply_mat(Vector3(min.x, max.y, max.z), &TransAABB.Transform);
		corners[6] = pos + multiply_mat(Vector3(min.x, max.y, min.z), &TransAABB.Transform);
		corners[7] = pos + multiply_mat(Vector3(max.x, max.y, min.z), &TransAABB.Transform);
		min = pos + multiply_mat(min, &TransAABB.Transform);
		max = pos + multiply_mat(max, &TransAABB.Transform);
		corners[0] = min;
		corners[1] = max;

		for (auto& v3 : corners)
		{
			if (!world_to_screen(v3))
				return false;
		}

		cords[0].x = (float)g_globals.g_width;
		cords[0].y = (float)g_globals.g_height;
		cords[1].x = 0.f;
		cords[1].y = 0.f;

		for (auto& v3 : corners) {
			if (v3.x < cords[0].x)
				cords[0].x = v3.x;
			if (v3.y < cords[0].y)
				cords[0].y = v3.y;
			if (v3.x > cords[1].x)
				cords[1].x = v3.x;
			if (v3.y > cords[1].y)
				cords[1].y = v3.y;
		}

		return true;
	}
}