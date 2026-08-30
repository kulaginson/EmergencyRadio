#include <windows.h>
#include <cstdint>

namespace
{
    // GTA SA 1.0 US addresses.
    constexpr std::uintptr_t WORLD_PLAYERS = 0xB7CD98;

    // CPed::m_pVehicle
    constexpr std::uintptr_t PED_VEHICLE_OFFSET = 0x58C;

    // CVehicle::m_vehicleAudio
    constexpr std::uintptr_t VEHICLE_AUDIO_OFFSET = 0x138;

    /*
        The radio type is stored inside the vehicle-audio settings.

        GTA SA values:
          -1 = no radio
           0 = civilian radio
           1 = special
           2 = none/unused
           3 = emergency radio
    */

    // For the classic GTA SA vehicle audio settings layout.
    constexpr std::uintptr_t RADIO_TYPE_OFFSET = 0x0F;

    constexpr std::uint8_t RADIO_CIVILIAN = 0;

    // GTA SA model IDs.
    constexpr int MODEL_POLICE      = 596;
    constexpr int MODEL_POLICE_LS   = 596;
    constexpr int MODEL_POLICE_SF   = 597;
    constexpr int MODEL_POLICE_LV   = 598;
    constexpr int MODEL_RANGER      = 599;
    constexpr int MODEL_FBI_RANCHER = 490;
    constexpr int MODEL_FBIRANCH    = 528;
    constexpr int MODEL_ENFORCER    = 427;
    constexpr int MODEL_AMBULANCE   = 416;
    constexpr int MODEL_FIRETRUCK   = 407;
    constexpr int MODEL_SWAT        = 601;
    constexpr int MODEL_POLICE_RANGER = 599;
    constexpr int MODEL_HP_VAN      = 497;
    constexpr int MODEL_POLICE_MOTORCYCLE = 523;

    // CVehicle::m_nModelIndex is inherited from CEntity.
    constexpr std::uintptr_t VEHICLE_MODEL_OFFSET = 0x22;

    bool IsEmergencyVehicle(std::uintptr_t vehicle)
    {
        if (!vehicle)
            return false;

        const auto model =
            *reinterpret_cast<const std::int16_t*>(
                vehicle + VEHICLE_MODEL_OFFSET
            );

        switch (model)
        {
            case MODEL_POLICE:
            case MODEL_POLICE_SF:
            case MODEL_POLICE_LV:
            case MODEL_RANGER:
            case MODEL_FBI_RANCHER:
            case MODEL_FBIRANCH:
            case MODEL_ENFORCER:
            case MODEL_AMBULANCE:
            case MODEL_FIRETRUCK:
            case MODEL_SWAT:
            case MODEL_HP_VAN:
            case MODEL_POLICE_MOTORCYCLE:
                return true;

            default:
                return false;
        }
    }

    void EnableCivilianRadio()
    {
        /*
            CWorld::Players[0]
                + 0x00 = CPlayerPed*

            CPlayerPed inherits CPed.

            CPed
                + 0x58C = m_pVehicle

            CVehicle
                + 0x138 = m_vehicleAudio
        */

        auto playerPed =
            *reinterpret_cast<std::uintptr_t*>(
                WORLD_PLAYERS
            );

        if (!playerPed)
            return;

        auto vehicle =
            *reinterpret_cast<std::uintptr_t*>(
                playerPed + PED_VEHICLE_OFFSET
            );

        if (!vehicle)
            return;

        if (!IsEmergencyVehicle(vehicle))
            return;

        auto vehicleAudio =
            vehicle + VEHICLE_AUDIO_OFFSET;

        /*
            Change the vehicle's radio type to civilian.

            This is intentionally done continuously while the player
            remains in the emergency vehicle, because GTA can restore
            the emergency radio setting when entering/changing vehicles.
        */
        *reinterpret_cast<std::uint8_t*>(
            vehicleAudio + RADIO_TYPE_OFFSET
        ) = RADIO_CIVILIAN;
    }

    DWORD WINAPI MainThread(LPVOID)
    {
        // Give GTA time to finish initialization.
        Sleep(3000);

        for (;;)
        {
            __try
            {
                EnableCivilianRadio();
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                // Do not crash GTA if the executable layout differs.
            }

            Sleep(250);
        }

        return 0;
    }
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

        HANDLE thread = CreateThread(
            nullptr,
            0,
            MainThread,
            nullptr,
            0,
            nullptr
        );

        if (thread)
            CloseHandle(thread);
    }

    return TRUE;
}
