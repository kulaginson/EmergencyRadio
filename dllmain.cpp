#include <windows.h>

#include <plugin.h>

#include "CVehicle.h"
#include "CPed.h"
#include "CPlayerPed.h"
#include "CWorld.h"

namespace
{
    constexpr int RADIO_CIVILIAN = 0;

    void EnableEmergencyVehicleRadio()
    {
        CPlayerPed* player = FindPlayerPed(-1);

        if (!player)
            return;

        CVehicle* vehicle = player->m_pVehicle;

        if (!vehicle)
            return;

        const bool emergency =
            vehicle->bIsLawEnforcer ||
            vehicle->bIsAmbulanceOnDuty ||
            vehicle->bIsFireTruckOnDuty;

        if (!emergency)
            return;

        vehicle->m_vehicleAudio.m_settings.m_nRadioType =
            RADIO_CIVILIAN;
    }

    class EmergencyRadio
    {
    public:
        EmergencyRadio()
        {
            plugin::Events::gameProcessEvent += []()
            {
                EnableEmergencyVehicleRadio();
            };
        }
    };

    EmergencyRadio g_emergencyRadio;
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD reason,
    LPVOID
)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
    }

    return TRUE;
}
