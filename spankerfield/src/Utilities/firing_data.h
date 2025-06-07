#include "../SDK/sdk.h"
#include <chrono>
#include <atomic>
#include <mutex>

class WeaponFiringDataRetriever
{
public:
    // Thread-safe singleton-like state tracking for vehicle entry
    static FiringFunctionData* GetSafeFiringData(WeaponFiring* weapon_firing, bool is_in_vehicle)
    {
        // Use atomic and mutex for thread-safe state management
        static std::mutex state_mutex;
        static std::atomic<bool> was_previously_in_vehicle{ false };
        static std::atomic<bool> is_in_initial_delay{ false };
        static std::chrono::steady_clock::time_point entry_time{};

        // Scoped lock for thread-safe state modifications
        std::lock_guard<std::mutex> lock(state_mutex);

        // Not in vehicle - reset state and proceed normally
        if (!is_in_vehicle)
        {
            was_previously_in_vehicle.store(false);
            is_in_initial_delay.store(false);
            return GetFiringData(weapon_firing);
        }

        auto now = std::chrono::steady_clock::now();

        // First time entering vehicle or transitioning to vehicle
        if (!was_previously_in_vehicle.load())
        {
            entry_time = now;
            was_previously_in_vehicle.store(true);
            is_in_initial_delay.store(true);
            return nullptr;
        }

        // Check if in initial delay period
        if (is_in_initial_delay.load())
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - entry_time);

            // Continue delaying for 150ms
            if (elapsed.count() < 150)
                return nullptr;

            // Delay period over, allow firing data retrieval
            is_in_initial_delay.store(false);
        }

        // Retrieve firing data
        return GetFiringData(weapon_firing);
    }

private:
    static FiringFunctionData* GetFiringData(WeaponFiring* weapon_firing)
    {
        // Extensive null and validity checks with improved error handling
        if (!IsValidPtr(weapon_firing))
        {
            // Log or handle invalid weapon_firing pointer
            return nullptr;
        }

        // Check primary fire with multiple validation criteria
        const auto firing = weapon_firing->m_pPrimaryFire;
        if (!IsValidPtr(firing) ||
            (reinterpret_cast<uintptr_t>(firing) == 0x10F00000030))
        {
            // Log or handle invalid firing pointer
            return nullptr;
        }

        // Check firing data with multiple validation criteria
        const auto firing_data = firing->m_FiringData;
        if (!IsValidPtr(firing_data) ||
            (reinterpret_cast<uintptr_t>(firing_data) == 0x3893E06))
        {
            // Log or handle invalid firing_data pointer
            return nullptr;
        }

        return firing_data;
    }
};