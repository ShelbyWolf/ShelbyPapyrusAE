#include "OnWeatherChanged.h"
#include "EventRegistry.h"
#include "PapyrusNames.h"

namespace OnWeatherChanged {

    static std::unordered_map<RE::FormID, std::string> weatherEditorIDs;

    struct SetEditorIDHook {
        static bool thunk(RE::TESWeather* a_this, const char* a_id) {
            if (a_id && *a_id) weatherEditorIDs[a_this->GetFormID()] = a_id;
            return func(a_this, a_id);
        }
        static inline REL::Relocation<decltype(thunk)> func;
    };

    RE::BSFixedString GetEditorID(RE::TESWeather* a_weather) {
        if (!a_weather) return "";
        if (auto it = weatherEditorIDs.find(a_weather->GetFormID()); it != weatherEditorIDs.end())
            return it->second.c_str();
        const char* id = a_weather->GetFormEditorID();
        return id ? id : "";
    }

    static Events::EventRegistry<RE::TESWeather*, RE::TESWeather*, RE::BSFixedString, RE::BSFixedString>
        registry{ PapyrusNames::kWeatherChangedEventName, 'WTHR' };

    static RE::TESWeather* lastWeather = nullptr;

    static void Check() {
        auto* sky = RE::Sky::GetSingleton();
        if (!sky || sky->mode.get() != RE::Sky::Mode::kFull) return;
        auto* current = sky->currentWeather;
        if (!current || current == lastWeather) return;

        auto* old = lastWeather;
        lastWeather = current;
        if (!old) return;
        registry.Send(nullptr, old, current, GetEditorID(old), GetEditorID(current));
    }

    struct PlayerUpdateHook {
        static void thunk(RE::PlayerCharacter* a_this, float a_delta) {
            func(a_this, a_delta);
            Check();
        }
        static inline REL::Relocation<decltype(thunk)> func;
    };

    void Reset() { lastWeather = nullptr; }

    void RegisterWeatherChanged(RE::StaticFunctionTag*, RE::TESQuest* a_quest)   { registry.Register(a_quest, nullptr); }
    void UnregisterWeatherChanged(RE::StaticFunctionTag*, RE::TESQuest* a_quest) { registry.Unregister(a_quest, nullptr); }
    RE::BSFixedString GetWeatherEditorID(RE::StaticFunctionTag*, RE::TESWeather* a_weather) { return GetEditorID(a_weather); }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::kRegisterWeatherChangedName,   PapyrusNames::PapyrusQuestScript,  RegisterWeatherChanged);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterWeatherChangedName, PapyrusNames::PapyrusQuestScript,  UnregisterWeatherChanged);
        a_vm->RegisterFunction(PapyrusNames::fGetWeatherEditorID,           PapyrusNames::PapyrusWeatherScript, GetWeatherEditorID);
    }

    void InstallEarly() {
        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_TESWeather[0] };
        SetEditorIDHook::func = vtbl.write_vfunc(0x33, SetEditorIDHook::thunk);
    }

    void RegisterEvents() {
        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_PlayerCharacter[0] };
        PlayerUpdateHook::func = vtbl.write_vfunc(0xAD, PlayerUpdateHook::thunk);
        if (weatherEditorIDs.empty())
            SKSE::log::warn("OnWeatherChanged: no weather editor IDs cached, GetWeatherEditorID will rely on po3 Tweaks");
    }
}