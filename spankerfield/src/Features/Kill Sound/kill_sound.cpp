#include "kill_sound.h"
#include "../../settings.h"
#include "../../Utilities/thread_pool.h"

using namespace big;
namespace plugins
{
	static int previous_kills = 0;
	static bool first_run = true;
	static std::chrono::steady_clock::time_point last_kill_time;
	void kill_sound()
	{
		if (!g_settings.kill_sound) return;

		const auto game_context = ClientGameContext::GetInstance();
		if (!game_context) return;

		const auto player_manager = game_context->m_pPlayerManager;
		if (!player_manager) return;

		const auto local_player = player_manager->m_pLocalPlayer;
		if (!IsValidPtr(local_player)) return;

		const auto local_soldier = local_player->GetSoldier();
		if (!IsValidPtr(local_soldier)) return;

		if (!local_soldier->IsAlive()) return;

		const auto score_class = SomeScoreManagerClass::GetInstance();
		if (!score_class) return;

		const auto score_manager = score_class->m_pScoreManager;
		if (!score_manager) return;

		const auto score = score_manager->getScore(local_player);
		if (!score) return;

		// Our current kills
		int current_kills = score->m_playerScore.m_kills;
		
		// We do this to prevent issues with injecting the cheat and hearing a kill sound, if you already have kills
		if (first_run)
		{
			first_run = false;
			previous_kills = current_kills;
			last_kill_time = std::chrono::steady_clock::now();

			return;
		}

		// Time check to prevent multiple sounds (debouncing)
		auto now = std::chrono::steady_clock::now();
		if (current_kills > previous_kills && std::chrono::duration_cast<std::chrono::milliseconds>(now - last_kill_time).count() > 100)
		{
			std::string file_path(g_settings.kill_sound_path);
			if (std::filesystem::exists(file_path) && !std::filesystem::is_directory(file_path))
			{
				// Wave only so that we don't have any issues
				bool is_wav_file = file_path.size() > 3 && std::equal(file_path.end() - 4, file_path.end(), xorstr_(".wav"), [](char a, char b) { return std::tolower(a) == std::tolower(b); });

				if (is_wav_file)
				{
					g_thread_pool->push([file_path]
					{
						// Convert std::string to std::wstring
						int size_needed = MultiByteToWideChar(CP_UTF8, 0, file_path.c_str(), (int)file_path.size(), NULL, 0);
						std::wstring wstr_to(size_needed, 0);
						MultiByteToWideChar(CP_UTF8, 0, file_path.c_str(), (int)file_path.size(), &wstr_to[0], size_needed);

						PlaySound(wstr_to.c_str(), NULL, SND_FILENAME | SND_ASYNC);
					});
				}
			}

			last_kill_time = now;
		}

		previous_kills = current_kills;
	}
}