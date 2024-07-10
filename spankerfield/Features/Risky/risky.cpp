#include "risky.h"
#include "../../settings.h"

using namespace big;
namespace plugins
{
	void no_recoil()
	{
		if (!g_settings.no_recoil) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtrWithVTable(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtrWithVTable(local_soldier)) return;

		if (local_soldier->IsAlive())
		{
			const auto weapon = WeaponFiring::GetInstance();
			if (!IsValidPtr(weapon)) return;

			const auto sway = weapon->m_Sway;
			if (!IsValidPtr(sway)) return;

			const auto data = sway->m_Data;
			if (!IsValidPtrWithVTable(data)) return;

			data->m_ShootingRecoilDecreaseScale = 100.0f;
			data->m_FirstShotRecoilMultiplier = 0.0f;
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