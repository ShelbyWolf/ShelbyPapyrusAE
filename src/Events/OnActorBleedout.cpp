#include "OnActorBleedout.h"
#include "PapyrusNames.h"
#include "EventRegistry.h"

#include <mutex>
#include <unordered_map>

namespace OnActorBleedout {
    namespace {
        Events::EventRegistry<RE::Actor*> registry{ PapyrusNames::kBleedoutEventName, 'BLED' };

        using Clock = std::chrono::steady_clock;

        std::mutex bleedoutMutex;
        std::unordered_map<RE::FormID, Clock::time_point> bleedoutTimes;

        struct BleedoutSink : RE::BSTEventSink<RE::TESEnterBleedoutEvent> {
            RE::BSEventNotifyControl ProcessEvent(const RE::TESEnterBleedoutEvent* a_event,
                                                  RE::BSTEventSource<RE::TESEnterBleedoutEvent>*) override {
                if (!a_event || !a_event->actor) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* actor = a_event->actor->As<RE::Actor>();
                if (!actor) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                {
                    std::lock_guard lock(bleedoutMutex);
                    bleedoutTimes[actor->GetFormID()] = Clock::now();
                }

                registry.Send(actor, actor);
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        BleedoutSink bleedoutSink;

        void RegisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest, RE::Actor* a_actor) {
            registry.Register(a_quest, a_actor);
        }

        void UnregisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest, RE::Actor* a_actor) {
            registry.Unregister(a_quest, a_actor);
        }

        float GetActorTimeSinceBleedout(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            if (!a_actor) return -1.0f;
            std::lock_guard lock(bleedoutMutex);
            auto it = bleedoutTimes.find(a_actor->GetFormID());
            if (it == bleedoutTimes.end()) return -1.0f;
            return std::chrono::duration<float>(Clock::now() - it->second).count();
        }
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::kRegisterBleedoutName, PapyrusNames::PapyrusQuestScript, RegisterNative);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterBleedoutName, PapyrusNames::PapyrusQuestScript, UnregisterNative);
        a_vm->RegisterFunction(PapyrusNames::fActorSinceBleedoutTime, PapyrusNames::PapyrusActorScript, GetActorTimeSinceBleedout);
    }

    void RegisterEvents() {
        if (auto* holder = RE::ScriptEventSourceHolder::GetSingleton()) {
            holder->GetEventSource<RE::TESEnterBleedoutEvent>()->AddEventSink(&bleedoutSink);
        }
    }
}