#pragma once
#include "Include/PCH.h"

namespace SystemPlus {
    bool DoesExist(std::string dllName); // If a dll plugin exist in SKSE/Plugins folder

    void Register(RE::BSScript::IVirtualMachine* a_vm);
}