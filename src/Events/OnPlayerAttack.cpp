#include "OnActorAttack.h"
#include "EventNames.h"
#include "EventRegistry.h"

#include <mutex>
#include <unordered_map>
#include <unordered_set>

// Same Papyrus API as the old version:
//   ShelbyExtendedQuests.RegisterAttackEvent(Quest akQuest)
//   Event OnPlayerAttack(Actor akVictim)
//   Event OnPlayerMagicAttack(Actor akVictim)
//   Event OnPlayerWerewolfAttack(Actor akVictim)
// Now saved in the co-save like every other event.
namespace OnActorAttack {
    namespace {
        // One registration covers all three player attack events
        Events::EventRegistry<RE::Actor*> registry{ EventNames::kAttackRegistryName, 'ATCK' };

        // Hit tracking for IsActorLandingHit / GetActorLastHitTarget (any actor, runtime only)
        std::mutex hitMutex;
        std::unordered_set<RE::FormID> hittingActors;
        std::unordered_map<RE::FormID, RE::FormID> lastHitTargets;

        bool IsWerewolf(RE::Actor* a_actor) {
            static auto* werewolfRace = RE::TESForm::LookupByEditorID<RE::TESRace>("WerewolfBeastRace");
            return werewolfRace && a_actor->GetRace() == werewolfRace;
        }

        bool IsMagicSource(RE::FormID a_source) {
            if (a_source == 0) return false;
            auto* form = RE::TESForm::LookupByID(a_source);
            return form && (form->As<RE::SpellItem>() || form->As<RE::EnchantmentItem>());
        }

        struct HitSink : RE::BSTEventSink<RE::TESHitEvent> {
            RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* a_event,
                                                  RE::BSTEventSource<RE::TESHitEvent>*) override {
                if (!a_event || !a_event->cause || !a_event->target) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto* attacker = a_event->cause->As<RE::Actor>();
                auto* victim = a_event->target->As<RE::Actor>();
                if (!attacker || !victim || attacker->IsDead() || victim->IsDead()) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                // Physical hit tracking (any actor)
                {
                    std::lock_guard lock(hitMutex);
                    hittingActors.insert(attacker->GetFormID());
                    lastHitTargets[attacker->GetFormID()] = victim->GetFormID();
                }

                // Attack events: PLAYER ONLY
                auto* player = RE::PlayerCharacter::GetSingleton();
                if (attacker != player) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                if (IsWerewolf(player)) {
                    registry.SendAs(EventNames::kPlayerWWAttackEventName, player, victim);
                } else if (IsMagicSource(a_event->source)) {
                    registry.SendAs(EventNames::kPlayerMgcAttackEventName, player, victim);
                } else {
                    registry.SendAs(EventNames::kPlayerAttackEventName, player, victim);
                }
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        HitSink hitSink;

        void RegisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            registry.Register(a_quest, nullptr);  // events are player-only already, no actor filter needed
        }

        void UnregisterNative(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
            registry.Unregister(a_quest, nullptr);
        }

        RE::Actor* GetActorLastHitTarget(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            if (!a_actor) return nullptr;
            RE::FormID targetID = 0;
            {
                std::lock_guard lock(hitMutex);
                auto it = lastHitTargets.find(a_actor->GetFormID());
                if (it == lastHitTargets.end()) return nullptr;
                targetID = it->second;
            }
            return RE::TESForm::LookupByID<RE::Actor>(targetID);
        }

        // True once per landed hit: consumes the flag
        bool IsActorLandingHit(RE::StaticFunctionTag*, RE::Actor* a_actor) {
            if (!a_actor || a_actor->IsDead()) return false;
            std::lock_guard lock(hitMutex);
            return hittingActors.erase(a_actor->GetFormID()) > 0;
        }
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(EventNames::kRegisterPlayerAttackName, EventNames::PapyrusQuestScript, RegisterNative);
        a_vm->RegisterFunction(EventNames::kUnregisterPlayerAttackName, EventNames::PapyrusQuestScript, UnregisterNative);
        a_vm->RegisterFunction(EventNames::fLandingHit, EventNames::PapyrusActorScript, IsActorLandingHit);
        a_vm->RegisterFunction(EventNames::fLastHitTarget, EventNames::PapyrusActorScript, GetActorLastHitTarget);
    }

    void RegisterEvents() {
        if (auto* holder = RE::ScriptEventSourceHolder::GetSingleton()) {
            holder->GetEventSource<RE::TESHitEvent>()->AddEventSink(&hitSink);
            SKSE::log::info("Hit sink attached");
        }
    }
}
