#pragma once
#include "../SDK/sdk.h"

namespace Hooks
{
	void Hook(DxRenderer* pDxRenderer, BorderInputNode* pBorderInputNode);
	void UnHook(BorderInputNode* pBorderInputNode);
}