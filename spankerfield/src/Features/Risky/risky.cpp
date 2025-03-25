#include "risky.h"
#include "../../settings.h"

using namespace big;
namespace plugins
{
	void sway_modify()
	{
		// No point to execude code further at this point
		if (!g_settings.no_recoil && !g_settings.no_spread) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		// There can be some game crashes if you use this function in vehicles, for example when firing rockets with an AA
		if (local_player->GetVehicle()) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		if (local_soldier->IsAlive())
		{
			const auto weapon = WeaponFiring::GetInstance();
			if (!IsValidPtr(weapon)) return;

			// We must verify if the weapon is actually a gun, otherwise the game will crash if we try to access it's recoil
			auto is_hit_type = [weapon]() -> bool
			{
				switch (weapon->GetWeaponClass())
				{
				case WeaponClass::_12gauge:
				case WeaponClass::_338Magnum:
				case WeaponClass::_357Magnum:
				case WeaponClass::_44Magnum:
				case WeaponClass::_45cal:
				case WeaponClass::_46x30mm:
				case WeaponClass::_50cal:
				case WeaponClass::_545x45mmWP:
				case WeaponClass::_556x45mmNATO:
				case WeaponClass::_57x28mm:
				case WeaponClass::_58x42mm:
				case WeaponClass::_762x39mmWP:
				case WeaponClass::_762x51mmNATO:
				case WeaponClass::_762x54mmR:
				case WeaponClass::_9x19mm:
				case WeaponClass::_9x39mm:
				case WeaponClass::Assault:
				case WeaponClass::Shotgun:
				case WeaponClass::Smg:
				case WeaponClass::Lmg:
				case WeaponClass::Sniper:
					return true;
				default:
					return false;
				}
			};

			if (!is_hit_type()) return;

			const auto sway = weapon->m_Sway;
			if (!IsValidPtr(sway)) return;

			const auto data = sway->m_Data;
			if (!IsValidPtr(data)) return;

			if (g_settings.no_recoil)
			{
				data->m_ShootingRecoilDecreaseScale = g_settings.recoil_decrease_scale; // 100.0f
				data->m_FirstShotRecoilMultiplier = g_settings.recoil_multiplier; // 0.0f
			}

			if (g_settings.no_spread)
			{
				*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(data) + 0x0430) = g_settings.spread_control; // m_DeviationScaleFactorZoom
				*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(data) + 0x0434) = g_settings.spread_control; // m_GameplayDeviationScaleFactorZoom
				*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(data) + 0x0438) = g_settings.spread_control; // m_DeviationScaleFactorNoZoom
				*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(data) + 0x043C) = g_settings.spread_control; // m_GameplayDeviationScaleFactorNoZoom
			}
		}
	}

	void unlock_all()
	{
		if (!g_settings.unlock_all) return;

		const auto settings = SyncedBFSettings::GetInstance();
		if (!settings) return;

		settings->m_AllUnlocksUnlocked = true;
	}

	void no_hc_restrictions()
	{
		if (!g_settings.no_hc_restrictions) return;

		const auto settings = SyncedBFSettings::GetInstance();
		if (!settings) return;

		settings->m_DisableHitIndicators = false;
		settings->m_NoMinimap = false;
		settings->m_NoMinimapSpotting = false;
		settings->m_No3dSpotting = false;
		settings->m_NoHud = false;
		settings->m_NoNameTag = false;
	}
}