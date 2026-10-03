#include "OnActorUsingFurniture.h"
#include "Include/PapyrusNames.h"
#include "../Include/EventRegistry.h"

namespace OnActorUsingFurniture {
    namespace {
        Events::EventRegistry<RE::Actor*, RE::TESObjectREFR*, bool> registry{ PapyrusNames::kFurnitureEventName, 'FURN' };

        struct FurnitureSink : RE::BSTEventSink<RE::TESFurnitureEvent> {
            RE::BSEventNotifyControl ProcessEvent(const RE::TESFurnitureEvent* a_event,
                                                  RE::BSTEventSource<RE::TESFurnitureEvent>*) override {
                if (!a_event || !a_event->actor || !a_event->targetFurniture) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* actor = a_event->actor->As<RE::Actor>();
                auto* furniture = a_event->targetFurniture.get();
                if (actor && furniture) {
                    const bool entered = a_event->type == RE::TESFurnitureEvent::FurnitureEventType::kEnter;
                    registry.Send(actor, actor, furniture, entered);
                }
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        FurnitureSink furnitureSink;

        void RegisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest, RE::Actor* a_actor) {
            registry.Register(a_quest, a_actor);
        }

        void UnregisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest, RE::Actor* a_actor) {
            registry.Unregister(a_quest, a_actor);
        }
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::kRegisterFurnitureName, PapyrusNames::PapyrusQuestScript, RegisterNative);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterFurnitureName, PapyrusNames::PapyrusQuestScript, UnregisterNative);
    }

    void RegisterEvents() {
        if (auto* holder = RE::ScriptEventSourceHolder::GetSingleton()) {
            holder->GetEventSource<RE::TESFurnitureEvent>()->AddEventSink(&furnitureSink);
        }
    }
}