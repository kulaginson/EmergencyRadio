#include <windows.h>
#include <cstdint>

// ============================================================
// EmergencyRadio
//
// Перенос логики рабочего MoonLoader-скрипта в ASI.
//
// Используем те же адреса, что и в твоём Lua:
//
// AUDIO_SETTINGS_BASE = 0x860AF0
// RECORD_SIZE         = 0x24
// RADIO_ID_OFFSET     = 0x1A
// RADIO_TYPE_OFFSET   = 0x1B
//
// GTA SA / адреса должны соответствовать той же версии игры,
// на которой работает твой emergency_radio.lua.
// ============================================================

namespace
{
    constexpr std::uintptr_t AUDIO_SETTINGS_BASE = 0x860AF0;

    constexpr std::uintptr_t RECORD_SIZE       = 0x24;
    constexpr std::uintptr_t RADIO_ID_OFFSET   = 0x1A;
    constexpr std::uintptr_t RADIO_TYPE_OFFSET = 0x1B;

    // F7
    constexpr int TOGGLE_KEY = VK_F7;

    // true  = обычное гражданское радио
    // false = штатная рация спецслужб
    bool civilianRadio = true;

    // Служебные автомобили из твоего Lua.
    constexpr int emergencyModels[] =
    {
        407, // Fire Truck
        416, // Ambulance
        427, // Enforcer
        490, // FBI Rancher
        523, // HPV1000
        528, // FBI Truck
        544, // Fire LA
        596, // Police LS
        597, // Police SF
        598, // Police LV
        599, // Police Ranger
        601  // SWAT
    };

    constexpr std::size_t emergencyModelCount =
        sizeof(emergencyModels) / sizeof(emergencyModels[0]);


    // --------------------------------------------------------
    // Проверка модели
    // --------------------------------------------------------

    bool isEmergencyModel(int modelId)
    {
        for (std::size_t i = 0;
             i < emergencyModelCount;
             ++i)
        {
            if (emergencyModels[i] == modelId)
                return true;
        }

        return false;
    }


    // --------------------------------------------------------
    // Изменение записи аудио конкретной модели
    //
    // Полностью повторяет твой Lua:
    //
    // record =
    //     AUDIO_SETTINGS_BASE +
    //     (modelId - 400) * RECORD_SIZE
    // --------------------------------------------------------

    void setRadio(int modelId)
    {
        if (!isEmergencyModel(modelId))
            return;

        const std::uintptr_t record =
            AUDIO_SETTINGS_BASE +
            static_cast<std::uintptr_t>(
                modelId - 400
            ) * RECORD_SIZE;


        __try
        {
            if (civilianRadio)
            {
                // Обычное гражданское радио.
                *reinterpret_cast<std::uint8_t*>(
                    record + RADIO_ID_OFFSET
                ) = 1;

                *reinterpret_cast<std::uint8_t*>(
                    record + RADIO_TYPE_OFFSET
                ) = 0;
            }
            else
            {
                // Штатная рация спецслужб.
                *reinterpret_cast<std::uint8_t*>(
                    record + RADIO_ID_OFFSET
                ) = 13;

                *reinterpret_cast<std::uint8_t*>(
                    record + RADIO_TYPE_OFFSET
                ) = 3;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            // Если адреса не соответствуют EXE,
            // не даём игре упасть.
        }
    }


    // --------------------------------------------------------
    // Применить настройки ко всем служебным моделям
    // --------------------------------------------------------

    void applyAll()
    {
        for (std::size_t i = 0;
             i < emergencyModelCount;
             ++i)
        {
            setRadio(emergencyModels[i]);
        }
    }


    // --------------------------------------------------------
    // Уведомление на экране
    //
    // Используем GTA SA native-функцию через её адрес.
    //
    // ВАЖНО:
    // Если этот адрес не соответствует твоему EXE,
    // уведомление просто не вызывается.
    // Само изменение radio settings от этого не зависит.
    // --------------------------------------------------------

    using PrintStringNow_t =
        void (__cdecl*)(const char*, unsigned int);

    // Для классического GTA SA 1.0 US.
    constexpr std::uintptr_t PRINT_STRING_NOW = 0x69F1E0;


    void showMessage(const char* text)
    {
        __try
        {
            auto PrintStringNow =
                reinterpret_cast<PrintStringNow_t>(
                    PRINT_STRING_NOW
                );

            PrintStringNow(text, 1500);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            // Ничего не делаем.
        }
    }


    // --------------------------------------------------------
    // Главный поток ASI
    // --------------------------------------------------------

    DWORD WINAPI MainThread(LPVOID)
    {
        // Даём GTA SA закончить загрузку.
        Sleep(3000);

        // При запуске включаем обычное радио.
        applyAll();

        bool previousF7State = false;

        for (;;)
        {
            // ------------------------------------------------
            // F7
            // ------------------------------------------------

            const bool currentF7State =
                (GetAsyncKeyState(TOGGLE_KEY) & 0x8000) != 0;

            // Аналог isKeyJustPressed().
            if (currentF7State && !previousF7State)
            {
                civilianRadio = !civilianRadio;

                applyAll();

                if (civilianRadio)
                {
                    showMessage(
                        "~g~Emergency Radio: ~w~Civilian Radio"
                    );
                }
                else
                {
                    showMessage(
                        "~y~Emergency Radio: ~w~Emergency Radio"
                    );
                }
            }

            previousF7State = currentF7State;


            // ------------------------------------------------
            // Периодически повторяем настройки.
            //
            // Это полезно потому, что игра может восстановить
            // исходные значения после загрузки/смены транспорта.
            // ------------------------------------------------

            static DWORD lastApply = 0;

            const DWORD now = GetTickCount();

            if (now - lastApply >= 1000)
            {
                applyAll();
                lastApply = now;
            }

            Sleep(20);
        }

        return 0;
    }
}


// ============================================================
// ASI / DLL entry point
// ============================================================

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
