#include "OnPlayerLockpick.h"
#include "PapyrusNames.h"
#include "EventRegistry.h"

#include <atomic>
#include <mutex>

namespace OnPlayerLockpick {
    namespace {
        constexpr RE::FormID kLockpickItem = 0x0000000A;
        constexpr RE::FormID kPlayer = 0x00000014;

        Events::EventRegistry<RE::TESObjectREFR*> registry{ PapyrusNames::kLockpickRegistryName, 'LOCK' };

        std::mutex lockMutex;
        RE::ObjectRefHandle currentLock;
        std::atomic_bool lockpickMenuOpen{ false };

        RE::TESObjectREFR* GetCurrentLock() {
            std::lock_guard lock(lockMutex);
            return currentLock.get().get();
        }

        struct MenuSink : RE::BSTEventSink<RE::MenuOpenCloseEvent> {
            RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event,
                                                  RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
                if (!a_event || a_event->menuName != RE::LockpickingMenu::MENU_NAME) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                if (a_event->opening) {
                    auto* pick = RE::CrosshairPickData::GetSingleton();
                    std::lock_guard lock(lockMutex);
                    currentLock = pick ? pick->target : RE::ObjectRefHandle{};
                    lockpickMenuOpen = true;
                    return RE::BSEventNotifyControl::kContinue;
                }

                lockpickMenuOpen = false;
                auto* lockRef = GetCurrentLock();
                {
                    std::lock_guard lock(lockMutex);
                    currentLock.reset();
                }
                if (!lockRef) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* player = RE::PlayerCharacter::GetSingleton();
                if (lockRef->IsLocked()) {
                    registry.SendAs(PapyrusNames::kPlayerLockpickFailEventName, player, lockRef);
                } else {
                    registry.SendAs(PapyrusNames::kPlayerLockpickEventName, player, lockRef);
                }
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        struct ContainerSink : RE::BSTEventSink<RE::TESContainerChangedEvent> {
            RE::BSEventNotifyControl ProcessEvent(const RE::TESContainerChangedEvent* a_event,
                                                  RE::BSTEventSource<RE::TESContainerChangedEvent>*) override {
                if (!a_event || !lockpickMenuOpen) {
                    return RE::BSEventNotifyControl::kContinue;
                }
                if (a_event->baseObj != kLockpickItem || a_event->oldContainer != kPlayer || a_event->newContainer != 0) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* lockRef = GetCurrentLock();
                registry.SendAs(PapyrusNames::kPlayerLockpickBrokeEventName, RE::PlayerCharacter::GetSingleton(), lockRef);
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        MenuSink menuSink;
        ContainerSink containerSink;

        void RegisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            registry.Register(a_quest, nullptr);
        }

        void UnregisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            registry.Unregister(a_quest, nullptr);
        }
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::kRegisterLockpickName, PapyrusNames::PapyrusQuestScript, RegisterNative);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterLockpickName, PapyrusNames::PapyrusQuestScript, UnregisterNative);
    }

    void RegisterEvents() {
        if (auto* ui = RE::UI::GetSingleton()) {
            ui->AddEventSink<RE::MenuOpenCloseEvent>(&menuSink);
        }
        if (auto* holder = RE::ScriptEventSourceHolder::GetSingleton()) {
            holder->GetEventSource<RE::TESContainerChangedEvent>()->AddEventSink(&containerSink);
        }
    }
}
