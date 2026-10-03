#include "Include/EventRegistry.h"
#include "Include/PapyrusNames.h"

namespace OnActorDialogue {
    namespace {
        Events::EventRegistry<RE::Actor*> registry{ PapyrusNames::kDialogueRegistryName, 'DLGE' };

        struct DialogueSink : RE::BSTEventSink<RE::MenuOpenCloseEvent> {
            RE::ActorHandle speaker;

            RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent *a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent> *a_eventSource) override {
                if (!a_event || a_event->menuName != RE::DialogueMenu::MENU_NAME)
                    return RE::BSEventNotifyControl::kContinue;

                if (a_event->opening) {
                    auto* topic = RE::MenuTopicManager::GetSingleton();
                    auto ref = topic ? topic->speaker.get() : nullptr;
                    auto* actor = ref ? ref->As<RE::Actor>() : nullptr;
                    if (!actor) return RE::BSEventNotifyControl::kContinue;
                    speaker = actor->GetHandle();
                    registry.SendAs(PapyrusNames::kDialogueEventName, actor, actor);
                } else if (auto actor = speaker.get()) {
                    registry.SendAs(PapyrusNames::kDialogueEndEventName, actor.get(), actor.get());
                    speaker.reset();
                }
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        DialogueSink dialogueSink;

        void RegisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest, RE::Actor* a_actor) {
            registry.Register(a_quest, a_actor);
        }

        void UnregisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest, RE::Actor* a_actor) {
            registry.Unregister(a_quest, a_actor);
        }
    };


    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::kRegisterDialogueName, PapyrusNames::PapyrusQuestScript, RegisterNative);
        a_vm->RegisterFunction(PapyrusNames::kUnregisterDialogueName, PapyrusNames::PapyrusQuestScript, UnregisterNative);
    }

    void RegisterEvents() {
        if (auto* ui = RE::UI::GetSingleton())
            ui->AddEventSink<RE::MenuOpenCloseEvent>(&dialogueSink);
    }
}
