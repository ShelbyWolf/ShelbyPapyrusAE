#include "OnPlayerWerewolfFeeding.h"

#include "ActorPlus.h"
#include "Include/PapyrusNames.h"
#include "../Include/EventRegistry.h"

namespace OnPlayerWerewolfFeeding {
    namespace {
        constexpr RE::FormID kWerewolfFeedEffect = 0x00106395;

        Events::EventRegistry<RE::Actor*> registry{ PapyrusNames::kPlayerWWFeedingRegistryName, 'WWFD' };

        struct FeedingSink : RE::BSTEventSink<RE::TESMagicEffectApplyEvent> {
            RE::BSEventNotifyControl ProcessEvent(const RE::TESMagicEffectApplyEvent* a_event,
                                                  RE::BSTEventSource<RE::TESMagicEffectApplyEvent>*) override {
                if (!a_event || a_event->magicEffect != kWerewolfFeedEffect) {
                    return RE::BSEventNotifyControl::kContinue;
                }
                if (!a_event->caster || !a_event->target) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* player = RE::PlayerCharacter::GetSingleton();
                if (a_event->caster.get() != player || !ActorPlus::IsWerewolf(player) || !ActorPlus::IsActorAlive(player)) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* victim = a_event->target->As<RE::Actor>();
                if (!victim) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                registry.SendAs(PapyrusNames::kPlayerWWFeedingEventName, player, victim);
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        FeedingSink feedingSink;

        void RegisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            registry.Register(a_quest, nullptr);
        }

        void UnregisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            registry.Unregister(a_quest, nullptr);
        }
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::kRegisterPlayerWWFeeding, PapyrusNames::PapyrusQuestScript, RegisterNative);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterPlayerWWFeeding, PapyrusNames::PapyrusQuestScript, UnregisterNative);
    }

    void RegisterEvents() {
        if (auto* holder = RE::ScriptEventSourceHolder::GetSingleton()) {
            holder->GetEventSource<RE::TESMagicEffectApplyEvent>()->AddEventSink(&feedingSink);
        }
    }
}