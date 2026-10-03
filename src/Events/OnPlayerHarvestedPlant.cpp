#include "OnPlayerHarvestedPlant.h"
#include "Include/PapyrusNames.h"
#include "../Include/EventRegistry.h"

namespace OnPlayerHarvestedPlant {
    namespace {
        Events::EventRegistry<RE::TESForm*> registry{ PapyrusNames::kPlayerHarvestedPlantEventName, 'HRVS' };

        struct HarvestSink : RE::BSTEventSink<RE::TESHarvestedEvent::ItemHarvested> {
            RE::BSEventNotifyControl ProcessEvent(const RE::TESHarvestedEvent::ItemHarvested* a_event,
                                                  RE::BSTEventSource<RE::TESHarvestedEvent::ItemHarvested>*) override {
                if (!a_event || !a_event->harvester || !a_event->produceItem) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* player = RE::PlayerCharacter::GetSingleton();
                if (a_event->harvester != player) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                registry.Send(player, a_event->produceItem);
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        HarvestSink harvestSink;

        void RegisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            registry.Register(a_quest, nullptr);
        }

        void UnregisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            registry.Unregister(a_quest, nullptr);
        }
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::kRegisterHarvestedPlantName, PapyrusNames::PapyrusQuestScript, RegisterNative);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterHarvestedPlantName, PapyrusNames::PapyrusQuestScript, UnregisterNative);
    }

    void RegisterEvents() {
        if (auto* source = RE::TESHarvestedEvent::GetEventSource()) {
            source->AddEventSink<RE::TESHarvestedEvent::ItemHarvested>(&harvestSink);
        }
    }
}
