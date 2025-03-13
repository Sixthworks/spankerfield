#pragma once
#include "sdk.h"

namespace big
{
	struct class_info_s
	{
		ClassInfo* MissileEntity = nullptr;
		ClassInfo* ExplosionEntity = nullptr;
		ClassInfo* VehicleEntity = nullptr;
		ClassInfo* WarningComponent = nullptr;
	};
	inline struct class_info_s class_info;

	void update_class_info();
}