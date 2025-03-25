#include "../SDK/sdk.h"

class WeaponFiringDataRetriever
{
public:
    static FiringFunctionData* GetSafeFiringData(WeaponFiring* weapon_firing, bool is_in_vehicle)
    {
        // Singleton-like state tracking for vehicle entry
        static struct VehicleEntryState
        {
            bool was_previously_in_vehicle = false;
            std::chrono::steady_clock::time_point entry_time{};
            bool is_in_initial_delay = false;
        } vehicle_state;

        // Not in vehicle - reset state and proceed normally
        if (!is_in_vehicle)
        {
            vehicle_state.was_previously_in_vehicle = false;
            vehicle_state.is_in_initial_delay = false;

            return GetFiringData(weapon_firing);
        }

        auto now = std::chrono::steady_clock::now();

        // First time entering vehicle or transitioning to vehicle
        if (!vehicle_state.was_previously_in_vehicle)
        {
            vehicle_state.entry_time = now;
            vehicle_state.was_previously_in_vehicle = true;
            vehicle_state.is_in_initial_delay = true;

            return nullptr;
        }

        // Check if in initial delay period
        if (vehicle_state.is_in_initial_delay)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - vehicle_state.entry_time);

            // Continue delaying for 50ms
            if (elapsed.count() < 50)
                return nullptr;

            // Delay period over, allow firing data retrieval
            vehicle_state.is_in_initial_delay = false;
        }

        // Retrieve firing data
        return GetFiringData(weapon_firing);
    }
private:
    static FiringFunctionData* GetFiringData(WeaponFiring* weapon_firing)
    {
        // Extensive null and validity checks
        if (!IsValidPtr(weapon_firing))
            return nullptr;

        // Check primary fire
        const auto firing = weapon_firing->m_pPrimaryFire;
        if (!IsValidPtr(firing) || (uintptr_t)firing == 0x10F00000030)
            return nullptr;

        // Check firing data
        const auto firing_data = firing->m_FiringData;
        if (!IsValidPtr(firing_data) || (uintptr_t)(firing_data) == 0x3893E06)
            return nullptr;

        return firing_data;
    }
};