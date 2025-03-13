#include "class_info.h"

namespace big
{
	void update_class_info()
	{
		class_info.MissileEntity = FindClassInfo(xorstr_("VeniceClientMissileEntity"));
		class_info.ExplosionEntity = FindClassInfo(xorstr_("ClientExplosionPackEntity"));
		class_info.VehicleEntity = FindClassInfo(xorstr_("ClientVehicleEntity"));
		class_info.WarningComponent = FindClassInfo(xorstr_("ClientWarningSystemComponent"));
	}
}