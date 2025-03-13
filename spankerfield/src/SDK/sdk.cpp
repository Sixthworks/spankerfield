#include "sdk.h"

VehicleData::VehicleType VehicleData::GetVehicleType()
{
	return *reinterpret_cast<VehicleData::VehicleType*>((uintptr_t)this + 0xE8);
}

VehicleData::VehicleCategory VehicleData::GetVehicleCategory()
{
	if (IsAirVehicle()) return VehicleCategory::AIR;
	if (IsGroundVehicle()) return VehicleCategory::GROUND;
	if (IsWaterVehicle()) return VehicleCategory::WATER;

	return VehicleCategory::UNUSED;
}

bool VehicleData::IsAirVehicle()
{
	switch (GetVehicleType())
	{
	case VehicleType::HELIATTACK:
	case VehicleType::HELISCOUT:
	case VehicleType::HELITRANS:
	case VehicleType::JETBOMBER:
	case VehicleType::JET:
	case VehicleType::MAV:
	case VehicleType::UAV:
		return true;
	default:
		return false;
	}
}

bool VehicleData::IsGroundVehicle()
{
	switch (GetVehicleType())
	{
	case VehicleType::TANK:
	case VehicleType::TANKAA:
	case VehicleType::TANKARTY:
	case VehicleType::TANKAT:
	case VehicleType::TANKIFV:
	case VehicleType::TANKLC:
	case VehicleType::JEEP:
	case VehicleType::CAR:
	case VehicleType::EODBOT:
	case VehicleType::MORTAR:
	case VehicleType::STATICAA:
	case VehicleType::STATICAT:
	case VehicleType::STATIONARY:
	case VehicleType::STATIONARYWEAPON:
		return true;
	default:
		return false;
	}
}

bool VehicleData::IsWaterVehicle()
{
	return (GetVehicleType() == VehicleType::BOAT);
}

bool VehicleData::IsInJet()
{
	const auto& type = GetVehicleType();

	return (type == VehicleType::JET || type == VehicleType::JETBOMBER);
}

bool VehicleData::IsInHeli()
{
	switch (GetVehicleType())
	{
	case VehicleType::HELIATTACK:
	case VehicleType::HELISCOUT:
	case VehicleType::HELITRANS:
		return true;
	default:
		return false;
	}
}

//m_MinTurnAngle > -1 when laser guided
bool MissileEntityData::IsLaserGuided()
{
	if (m_MinTurnAngle > 0.0f) return true;
	return false;
}

bool MissileEntityData::IsLockable()
{
	if (m_TimeToActivateGuidingSystem > 0.0f) return true;
	return false;
}

bool ClientControllableEntity::IsAlive()
{
	if (!IsValidPtr(m_pHealthComp))
		return false;

	return (m_pHealthComp->m_Health > 0.0f);
}

WeaponClass WeaponFiring::GetWeaponClass()
{
	auto data = reinterpret_cast<WeaponEntityData*>(this->m_weaponComponentData);
	if (IsValidPtr(data))
		return data->m_WeaponClass;

	return WeaponClass::None;
}