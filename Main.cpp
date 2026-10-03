#include "src/Include/PCH.h"
#include "src/ActorPlus.h"
#include "src/WeatherPlus.h"
#include "src/SystemPlus.h"
#include "Events/OnActorUsingFurniture.h"
#include "Events/OnPlayerAttack.h"
#include "src/Include/Serialization.h"
#include "Events/OnActorBleedout.h"
#include "Events/OnPlayerLockpick.h"
#include "Events/OnActorRaceSwitched.h"
#include "Events/OnPlayerWerewolfFeeding.h"
#include "Events/OnPlayerHarvestedPlant.h"
#include "Events/OnWeatherChanged.h"
#include "Events/OnActorDialogue.h"
#include "Version.h"

extern "C" __declspec(dllexport) constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v{};
    v.PluginVersion(Plugin::VERSION);
    v.PluginName(Plugin::NAME);
    v.AuthorName(Plugin::AUTHOR);
    v.UsesAddressLibrary();
    v.UsesUpdatedStructs();
    return v;
}();

extern "C" __declspec(dllexport) bool SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* a_info) {
    a_info->infoVersion = SKSE::PluginInfo::kVersion;
    a_info->name = Plugin::NAME;
    a_info->version = Plugin::VERSION.major();
    return true;
}

bool RegisterFunctions(RE::BSScript::IVirtualMachine* a_vm) {
    ActorPlus::Register(a_vm);
    WeatherPlus::Register(a_vm);
    SystemPlus::Register(a_vm);
    OnActorAttack::Register(a_vm);
    OnActorBleedout::Register(a_vm);
    OnActorUsingFurniture::Register(a_vm);
    OnActorRaceSwitched::Register(a_vm);
    OnPlayerLockpick::Register(a_vm);
    OnPlayerWerewolfFeeding::Register(a_vm);
    OnPlayerHarvestedPlant::Register(a_vm);
    OnWeatherChanged::Register(a_vm);
    OnActorDialogue::Register(a_vm);
    return true;
}

extern "C" __declspec(dllexport) bool SKSEPlugin_Load(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);

    SKSE::log::init();
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::warn);

    SKSE::log::info("{} v{} by {} loaded", Plugin::NAME, Plugin::VERSION.string("."), Plugin::AUTHOR);

    Serialization::Install();
    OnWeatherChanged::InstallEarly();

    auto* papyrus = SKSE::GetPapyrusInterface();
    if (papyrus) {
        papyrus->Register(RegisterFunctions);
    }

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* a_msg) {
        switch (a_msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
                OnActorAttack::RegisterEvents();
                OnActorBleedout::RegisterEvents();
                OnActorUsingFurniture::RegisterEvents();
                OnPlayerLockpick::RegisterEvents();
                OnPlayerWerewolfFeeding::RegisterEvents();
                OnActorRaceSwitched::RegisterEvents();
                OnPlayerHarvestedPlant::RegisterEvents();
                OnWeatherChanged::RegisterEvents();
                OnActorDialogue::RegisterEvents();
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
        case SKSE::MessagingInterface::kNewGame:
            OnWeatherChanged::Reset();
            break;
        default:
            break;
        }
    });

    return true;
}