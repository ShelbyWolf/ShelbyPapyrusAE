#pragma once
#include "PCH.h"

namespace ActorPlus {
    bool IsActorWerewolf(const RE::Actor* a_this);
    void Register(RE::BSScript::IVirtualMachine* a_vm);
}