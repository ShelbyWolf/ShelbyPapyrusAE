#pragma once
#include "Include/PCH.h"

namespace ActorPlus {
    bool IsWerewolf(RE::Actor* a_actor);
    bool IsInExterior(RE::Actor* a_actor);
    bool IsUsingFurniture(RE::Actor* a_actor);
    bool IsStaggered(RE::Actor* a_actor);
    bool IsFemale(RE::Actor* a_actor);
    bool IsHeadtracking(RE::Actor* a_actor);
    bool IsDualWielding(RE::Actor* a_actor);
    bool IsPowerAttacking(RE::Actor* a_actor);

    inline void ForActorEachInventoryItems(RE::Actor* a_actor, std::function<void(RE::TESBoundObject*, std::int32_t, RE::InventoryEntryData*)> a_func) {
        if (!a_actor) return;
        auto inventory = a_actor->GetInventory();
        for (auto& [item, data] : inventory) {
            if (!item) continue;
            const auto& [count, entry] = data;
            a_func(item, count, entry.get());
        }
    }

    // Returns true only if the actor is non-null and alive.
    // Do not use for functions that operate on dead actors.
    inline bool IsActorAlive(RE::Actor* a_actor) {
        return a_actor && !a_actor->IsDead();
    }

    inline RE::TESNPC* GetActorBase(RE::Actor* a_actor) {
        if (!a_actor) return nullptr;
        return a_actor->GetActorBase();
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm);
}