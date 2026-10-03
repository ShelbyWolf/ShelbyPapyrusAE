#pragma once
#include "ActorPlus.h"

#include "Include/PCH.h"
#include "Include/PapyrusNames.h"

// TODO / Future Function Ideas

// IsActorInCombatWith(Actor, Actor)     - Check if two specific actors are in combat with each other
// GetDaysSinceLastSleep()               - Days since player last slept, cleaner than GetTimeSinceLastRest() / 24
// GetPlayerTimeInBeastForm()            - How long the player has been in werewolf form
// IsActorFalling(Actor)                 - Mid-air falling state, no vanilla equivalent
// IsActorSwimming(Actor)                - Swimming state, confirmed missing from all libraries
// GetActorTimeSinceLastHit(Actor)       - Pairs with IsActorLandingHit tracking
// GetPlayerKillCount()                  - Kills this session
// GetActorAltitude(Actor)               - Returns the Actor current altitude based-of the lowest point of the map
// GetLastDroppedItem(Actor)             - Pickpocket mods, item tracking systems, crime mods

namespace ActorPlus {

    bool IsAttacking(RE::Actor* a_actor) {
        if (!IsActorAlive(a_actor)) return false;
        return a_actor->IsAttacking();
    }
    bool IsActorAttacking(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsAttacking(a_actor);
    }

    bool IsWerewolf(RE::Actor* a_actor) {
        if (!a_actor) return false;
        static auto* werewolfRace = RE::TESForm::LookupByEditorID<RE::TESRace>("WerewolfBeastRace");
        return werewolfRace && a_actor->GetRace() == werewolfRace;
    }
    bool IsActorWerewolf(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsWerewolf(a_actor);
    }

    bool IsPowerAttacking(RE::Actor* a_actor) {
        if (!IsActorAlive(a_actor) || !a_actor->IsAttacking()) return false;

        auto* process = a_actor->GetActorRuntimeData().currentProcess;
        if (!process || !process->high) return false;

        auto& attackData = process->high->attackData;
        return attackData && attackData->data.flags.all(RE::AttackData::AttackFlag::kPowerAttack);
    }
    bool IsActorPowerAttacking(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsPowerAttacking(a_actor);
    }

    bool IsDualWielding(RE::Actor* a_actor) {
        if (!a_actor) return false;
        auto* rightHand = a_actor->GetEquippedObject(false);
        auto* leftHand = a_actor->GetEquippedObject(true);
        if (!rightHand || !leftHand) return false;

        auto* rightWeapon = rightHand->As<RE::TESObjectWEAP>();
        auto* leftWeapon = leftHand->As<RE::TESObjectWEAP>();

        if (!rightWeapon || !leftWeapon) return false;

        // exclude two-handed weapons and bows
        auto rightType = rightWeapon->GetWeaponType();
        auto leftType = leftWeapon->GetWeaponType();

        using WT = RE::WEAPON_TYPE;
        auto isTwoHanded = [](WT t) {
            return t == WT::kTwoHandSword ||
                   t == WT::kTwoHandAxe ||
                   t == WT::kBow ||
                   t == WT::kCrossbow ||
                   t == WT::kStaff;
        };

        return !isTwoHanded(rightType) && !isTwoHanded(leftType) && rightWeapon != leftWeapon;
    }
    bool IsActorDualWielding(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsDualWielding(a_actor);
    }

    bool IsFemale(RE::Actor* a_actor) {
        if (!a_actor) return false;
        auto* Base = ActorPlus::GetActorBase(a_actor);
        if (!Base) return false;
        bool female = Base->GetSex();
        if (!female) return false;
        return female;
    }
    bool IsActorFemale(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsFemale(a_actor);
    }

    bool IsInExterior(RE::Actor* a_actor) {
        if (!a_actor) return false;
        auto* cell = a_actor->GetParentCell();
        if (!cell) return false;
        return !cell->IsInteriorCell();
    }
    bool IsActorInExterior(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsInExterior(a_actor);
    }

    bool IsUsingFurniture(RE::Actor* a_actor) {
        if (!a_actor) return false;
        return a_actor->GetOccupiedFurniture().native_handle() != 0;
    }
    bool IsActorUsingFurniture(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsUsingFurniture(a_actor);
    }

    bool IsStaggered(RE::Actor* a_actor) {
        if (!a_actor) return false;
        return a_actor->AsActorState()->actorState2.staggered;
    }
    bool IsActorStaggered(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsStaggered(a_actor);
    }

    bool IsHeadtracking(RE::Actor* a_actor) {
        if (!a_actor) return false;
        return a_actor->AsActorState()->actorState2.headTracking;
    }
    bool IsActorHeadtracking(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        return IsHeadtracking(a_actor);
    }

    int GetActorEquippedWeaponType(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        if (!IsActorAlive(a_actor)) return -1;

        auto* equipped = a_actor->GetEquippedObject(false);
        if (!equipped) return 0; // 0 = unarmed

        auto* weapon = equipped->As<RE::TESObjectWEAP>();
        if (!weapon) return 0;

        return static_cast<int>(weapon->GetWeaponType());
    }

    float GetActorInventoryWeight(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        float totalWeightValue = 0.0f;
        ForActorEachInventoryItems(a_actor, [&](RE::TESBoundObject* item, std::int32_t count, RE::InventoryEntryData*) {
            totalWeightValue += item->GetWeight() * count;
        });
        return totalWeightValue;
    }

    int GetActorInventoryItemCount(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        int total = 0;
        ForActorEachInventoryItems(a_actor, [&](RE::TESBoundObject*, std::int32_t count, RE::InventoryEntryData*) {
            total += count;
        });
        return total;
    };

    int GetActorInventoryGoldValue(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        int totalValue = 0;
        ForActorEachInventoryItems(a_actor, [&](RE::TESBoundObject* item, std::int32_t count, RE::InventoryEntryData*) {
            totalValue += item->GetGoldValue() * count;
        });
        return totalValue;
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {

        a_vm->RegisterFunction(PapyrusNames::fActorFemale, PapyrusNames::PapyrusActorScript, IsActorFemale);
        a_vm->RegisterFunction(PapyrusNames::fActorWerewolf, PapyrusNames::PapyrusActorScript, IsActorWerewolf);
        a_vm->RegisterFunction(PapyrusNames::fActorFurniture, PapyrusNames::PapyrusActorScript, IsActorUsingFurniture);
        a_vm->RegisterFunction(PapyrusNames::fActorAttacking, PapyrusNames::PapyrusActorScript, IsActorAttacking);
        a_vm->RegisterFunction(PapyrusNames::fActorPowerAttacking, PapyrusNames::PapyrusActorScript, IsActorPowerAttacking);
        a_vm->RegisterFunction(PapyrusNames::fActorExterior, PapyrusNames::PapyrusActorScript, IsActorInExterior);
        a_vm->RegisterFunction(PapyrusNames::fActorEquippedWeapon, PapyrusNames::PapyrusActorScript, GetActorEquippedWeaponType);
        a_vm->RegisterFunction(PapyrusNames::fActorInvWeight, PapyrusNames::PapyrusActorScript, GetActorInventoryWeight);
        a_vm->RegisterFunction(PapyrusNames::fActorInvItemCount, PapyrusNames::PapyrusActorScript, GetActorInventoryItemCount);
        a_vm->RegisterFunction(PapyrusNames::fActorInvGoldValue, PapyrusNames::PapyrusActorScript, GetActorInventoryGoldValue);

    }
}