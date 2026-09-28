#pragma once
#include "PCH.h"
#include "Events/PapyrusNames.h"

namespace WeatherPlus {
    float GetGameHour() {
        auto* calendar = RE::Calendar::GetSingleton();
        return calendar ? calendar->GetHour() : 0.0f;
    }

    RE::TESWeather* GetExteriorWeather() {
        auto* player = RE::PlayerCharacter::GetSingleton();
        auto* cell = player ? player->GetParentCell() : nullptr;
        if (!cell || cell->IsInteriorCell()) return nullptr;
        auto* sky = RE::Sky::GetSingleton();
        return sky ? sky->currentWeather : nullptr;
    }

    bool IsGameHourBetween(RE::StaticFunctionTag*, float firstHour, float secondHour) {
        float hour = GetGameHour();

        if (firstHour <= secondHour) {
            return hour >= firstHour && hour < secondHour;
        } else {
            return hour >= firstHour || hour < secondHour;
        }
    }

    float GetCurrentGameHour(RE::StaticFunctionTag*) {
        return GetGameHour();
    }

    RE::BSFixedString GetCurrentDayName(RE::StaticFunctionTag*) {
        auto* calendar = RE::Calendar::GetSingleton();
        if (!calendar) return "";
        return RE::BSFixedString(calendar->GetDayName().c_str());
    }

    RE::BSFixedString GetCurrentMonthName(RE::StaticFunctionTag*) {
        auto* calendar = RE::Calendar::GetSingleton();
        if (!calendar) return "";
        return RE::BSFixedString(calendar->GetMonthName().c_str());
    }

    float GetWindAngle(RE::StaticFunctionTag*) {
        auto* weather = GetExteriorWeather();
        if (!weather) return 0.0f;
        return static_cast<std::uint8_t>(weather->data.windDirection) / 256.0f * 360.0f;
    }

    float GetWindSpeed(RE::StaticFunctionTag*) {
        auto* weather = GetExteriorWeather();
        if (!weather) return 0.0f;
        return static_cast<std::uint8_t>(weather->data.windSpeed) / 255.0f;
    }

    int GetMoonPhaseInt(RE::StaticFunctionTag*) {
        auto* calendar = RE::Calendar::GetSingleton();
        if (!calendar) return -1;

        float totalDays = calendar->GetCurrentGameTime();
        int phase = (static_cast<int>(totalDays) % 24) / 3;

        return phase;
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::fWeatherHourBetween, PapyrusNames::PapyrusWeatherScript, IsGameHourBetween);
        a_vm->RegisterFunction(PapyrusNames::fWeatherWindAngle, PapyrusNames::PapyrusWeatherScript, GetWindAngle);
        a_vm->RegisterFunction(PapyrusNames::fWeatherWindSpeed, PapyrusNames::PapyrusWeatherScript, GetWindSpeed);
        a_vm->RegisterFunction(PapyrusNames::fWeatherGameHour, PapyrusNames::PapyrusWeatherScript, GetCurrentGameHour);
        a_vm->RegisterFunction(PapyrusNames::fWeatherDayName, PapyrusNames::PapyrusWeatherScript, GetCurrentDayName);
        a_vm->RegisterFunction(PapyrusNames::fWeatherMonthName, PapyrusNames::PapyrusWeatherScript, GetCurrentMonthName);
        a_vm->RegisterFunction(PapyrusNames::fWeatherMoonPhase, PapyrusNames::PapyrusWeatherScript, GetMoonPhaseInt);
    }
}
