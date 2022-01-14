#pragma once
#include "../Rendering/base.h"

namespace Utilities
{
	TransformAABBStruct GetTransform(ClientPlayer* Player);
	inline bool WorldToScreen(const Vector3& pos, Vector2& out);
	inline bool WorldToScreen(Vector3& pos);
	auto MultiplyMat(const Vector3& vec, const Matrix* mat);
	extern bool GetBoxCords(const TransformAABBStruct& TransAABB, Vector2* cords);
}