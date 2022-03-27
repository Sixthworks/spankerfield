#pragma once
#include "../SDK/sdk.h"

namespace big
{
	TransformAABBStruct GetTransform(ClientPlayer* Player);
	extern bool WorldToScreen(const Vector3& pos, Vector2& out);
	extern bool WorldToScreen(const Vector3& origin, Vector3& screen);
	extern bool WorldToScreen(Vector3& pos);
	Vector3 MultiplyMat(const Vector3& vec, const Matrix* mat);
	extern bool GetBoxCords(const TransformAABBStruct& TransAABB, Vector2* cords);
}