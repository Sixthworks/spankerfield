#include "w2s.h"
#include "../global.h"

namespace utils
{
	TransformAABBStruct GetTransform(ClientPlayer* Player)
	{
		TransformAABBStruct Transform;

		const auto Soldier = Player->GetSoldier();
		if (!Soldier) 
			return Transform;

		ClientVehicleEntity* Vehicle = Player->GetVehicle();
		Vehicle ? Vehicle->GetAABB(&Transform) : Soldier->GetAABB(&Transform);

		return Transform;
	}

	inline bool WorldToScreen(const Vector3& pos, Vector2& out)
	{
		float w = globals::g_ViewProj.m[0][3] * pos.x + globals::g_ViewProj.m[1][3] * pos.y + globals::g_ViewProj.m[2][3] * pos.z + globals::g_ViewProj.m[3][3];
		if (w < 0.19)
			return false;

		float x = globals::g_ViewProj.m[0][0] * pos.x + globals::g_ViewProj.m[1][0] * pos.y + globals::g_ViewProj.m[2][0] * pos.z + globals::g_ViewProj.m[3][0];
		float y = globals::g_ViewProj.m[0][1] * pos.x + globals::g_ViewProj.m[1][1] * pos.y + globals::g_ViewProj.m[2][1] * pos.z + globals::g_ViewProj.m[3][1];

		auto hWidth = static_cast<float>(globals::g_Width) / 2.0f;
		auto hHeight = static_cast<float>(globals::g_Height) / 2.0f;

		auto out_x = hWidth + hWidth * x / w;
		auto out_y = hHeight - hHeight * y / w;

		if (out_x >= 0.0f && out_x < globals::g_Width && out_y >= 0.0f && out_y < globals::g_Height) {

			out.x = out_x;
			out.y = out_y;
			//out->z = w;

			return true;
		}
		return false;
	}


	inline bool WorldToScreen(const Vector3& origin, Vector3& screen)
	{
		float mX = static_cast<float>(globals::g_Width * 0.5f);
		float mY = static_cast<float>(globals::g_Height * 0.5f);

		float w = globals::g_ViewProj(0, 3) * origin.x + globals::g_ViewProj(1, 3) * origin.y + globals::g_ViewProj(2, 3) * origin.z + globals::g_ViewProj(3, 3);

		if (w < 0.0001f)
		{
			screen.z = w;
			return false;
		}

		float x = globals::g_ViewProj(0, 0) * origin.x + globals::g_ViewProj(1, 0) * origin.y + globals::g_ViewProj(2, 0) * origin.z + globals::g_ViewProj(3, 0);
		float y = globals::g_ViewProj(0, 1) * origin.x + globals::g_ViewProj(1, 1) * origin.y + globals::g_ViewProj(2, 1) * origin.z + globals::g_ViewProj(3, 1);

		screen.x = mX + mX * x / w;
		screen.y = mY - mY * y / w;
		screen.z = w;

		return true;
	}

	inline bool WorldToScreen(Vector3& pos)
	{
		return WorldToScreen(pos, reinterpret_cast<Vector2&>(pos));
	}

	auto MultiplyMat(const Vector3& vec, const Matrix* mat)
	{
		return Vector3(mat->_11 * vec.x + mat->_21 * vec.y + mat->_31 * vec.z,
			mat->_12 * vec.x + mat->_22 * vec.y + mat->_32 * vec.z,
			mat->_13 * vec.x + mat->_23 * vec.y + mat->_33 * vec.z);
	}

	bool GetBoxCords(const TransformAABBStruct& TransAABB, Vector2* cords) {
		Vector3 corners[8];
		Vector3 pos = (Vector3)TransAABB.Transform.m[3];
		Vector3 min = Vector3(TransAABB.AABB.m_Min.x, TransAABB.AABB.m_Min.y, TransAABB.AABB.m_Min.z);
		Vector3 max = Vector3(TransAABB.AABB.m_Max.x, TransAABB.AABB.m_Max.y, TransAABB.AABB.m_Max.z);
		corners[2] = pos + MultiplyMat(Vector3(max.x, min.y, min.z), &TransAABB.Transform);
		corners[3] = pos + MultiplyMat(Vector3(max.x, min.y, max.z), &TransAABB.Transform);
		corners[4] = pos + MultiplyMat(Vector3(min.x, min.y, max.z), &TransAABB.Transform);
		corners[5] = pos + MultiplyMat(Vector3(min.x, max.y, max.z), &TransAABB.Transform);
		corners[6] = pos + MultiplyMat(Vector3(min.x, max.y, min.z), &TransAABB.Transform);
		corners[7] = pos + MultiplyMat(Vector3(max.x, max.y, min.z), &TransAABB.Transform);
		min = pos + MultiplyMat(min, &TransAABB.Transform);
		max = pos + MultiplyMat(max, &TransAABB.Transform);
		corners[0] = min;
		corners[1] = max;

		for (auto& v3 : corners) {
			if (!WorldToScreen(v3))
				return false;
		}

		cords[0].x = globals::g_Width;
		cords[0].y = globals::g_Height;
		cords[1].x = 0;
		cords[1].y = 0;

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