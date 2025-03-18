#include "name_spoofer.h"
#include "../../settings.h"

using namespace big;
namespace plugins
{
	static bool backup = true;
	void spoof_name()
	{
		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		// Backup original name (one-time)
		if (backup && local_player->m_Name[0] != '\0')
		{
			strcpy(g_settings.original_name, local_player->m_Name);
			backup = false;
		}

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		// Restore name logic
		if (g_settings.spoof_restore)
		{
			// Restore original name only if backed up previously
			strcpy(local_player->m_Name, g_settings.original_name);
			strcpy(reinterpret_cast<char*>(local_player) + 0x0040, g_settings.original_name);
			strcpy(reinterpret_cast<char*>(local_player) + 0x1836, g_settings.original_name);

			g_settings.spoof_name = false;
			g_settings.spoof_restore = false;

			return;
		}

		// Apply spoof if enabled and player is alive
		if (g_settings.spoof_name && local_soldier->IsAlive())
		{
			if (strcmp(local_player->m_Name, g_settings.spoofed_name) != NULL)
			{
				strcpy(local_player->m_Name, g_settings.spoofed_name);
				strcpy(reinterpret_cast<char*>(local_player) + 0x0040, g_settings.spoofed_name);
				strcpy(reinterpret_cast<char*>(local_player) + 0x1836, g_settings.spoofed_name);
			}
		}
	}

}