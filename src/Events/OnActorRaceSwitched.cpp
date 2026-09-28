#include "OnActorRaceSwitched.h"
#include "PapyrusNames.h"
#include "EventRegistry.h"

#include <atomic>

namespace OnActorRaceSwitched {
    namespace {
        Events::EventRegistry<RE::Actor*, RE::TESRace*> registry{ PapyrusNames::kActorRaceSwitchEventName, 'RACE' };
        Events::EventRegistry<std::int32_t> sexRegistry{ PapyrusNames::kPlayerSexChangedEventName, 'SEXC' };

        std::atomic_bool raceMenuOpen{ false };
        std::atomic<RE::FormID> raceAtMenuOpen{ 0 };
        std::atomic<std::int32_t> sexAtMenuOpen{ -1 };

        void SendRaceSwitch(RE::Actor* a_actor) {
            auto* race = a_actor->GetRace();
            registry.Send(a_actor, a_actor, race);
        }

        std::int32_t GetSex(RE::Actor* a_actor) {
            auto* base = a_actor ? a_actor->GetActorBase() : nullptr;
            return base ? static_cast<std::int32_t>(base->GetSex()) : -1;
        }

        struct MenuSink : RE::BSTEventSink<RE::MenuOpenCloseEvent> {
            RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event,
                                                  RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
                if (!a_event) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                const bool isRaceMenu = a_event->menuName == RE::RaceSexMenu::MENU_NAME;
                const bool isConsole = a_event->menuName == RE::Console::MENU_NAME;
                if (!isRaceMenu && !isConsole) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* player = RE::PlayerCharacter::GetSingleton();
                if (!player) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                if (a_event->opening) {
                    if (isRaceMenu) {
                        raceMenuOpen = true;
                        raceAtMenuOpen = player->GetRace() ? player->GetRace()->GetFormID() : 0;
                    }
                    sexAtMenuOpen = GetSex(player);
                    return RE::BSEventNotifyControl::kContinue;
                }

                if (isRaceMenu) {
                    raceMenuOpen = false;
                    if (player->GetRace() && player->GetRace()->GetFormID() != raceAtMenuOpen) {
                        SendRaceSwitch(player);
                    }
                }

                const auto oldSex = sexAtMenuOpen.exchange(-1);
                const auto newSex = GetSex(player);
                if (oldSex != -1 && newSex != -1 && newSex != oldSex) {
                    sexRegistry.Send(player, newSex);
                }
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        struct SwitchRaceSink : RE::BSTEventSink<RE::TESSwitchRaceCompleteEvent> {
            RE::BSEventNotifyControl ProcessEvent(const RE::TESSwitchRaceCompleteEvent* a_event,
                                                  RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*) override {
                if (!a_event || !a_event->subject) {
                    return RE::BSEventNotifyControl::kContinue;
                }
                auto* actor = a_event->subject->As<RE::Actor>();
                if (!actor) {
                    return RE::BSEventNotifyControl::kContinue;
                }
                if (raceMenuOpen && actor->IsPlayerRef()) {
                    return RE::BSEventNotifyControl::kContinue;
                }
                SendRaceSwitch(actor);
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        MenuSink menuSink;
        SwitchRaceSink switchRaceSink;

        void RegisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest, RE::Actor* a_actor) {
            registry.Register(a_quest, a_actor);
        }

        void UnregisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest, RE::Actor* a_actor) {
            registry.Unregister(a_quest, a_actor);
        }

        void RegisterSexNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            sexRegistry.Register(a_quest, nullptr);
        }

        void UnregisterSexNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            sexRegistry.Unregister(a_quest, nullptr);
        }
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::kRegisterRaceSwitchName, PapyrusNames::PapyrusQuestScript, RegisterNative);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterRaceSwitchName, PapyrusNames::PapyrusQuestScript, UnregisterNative);
        a_vm->RegisterFunction(PapyrusNames::kRegisterSexChangedName, PapyrusNames::PapyrusQuestScript, RegisterSexNative);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterSexChangedName, PapyrusNames::PapyrusQuestScript, UnregisterSexNative);
    }

    void RegisterEvents() {
        if (auto* ui = RE::UI::GetSingleton()) {
            ui->AddEventSink<RE::MenuOpenCloseEvent>(&menuSink);
        }
        if (auto* holder = RE::ScriptEventSourceHolder::GetSingleton()) {
            holder->GetEventSource<RE::TESSwitchRaceCompleteEvent>()->AddEventSink(&switchRaceSink);
        }
    }
}
