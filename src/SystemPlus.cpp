#include "SystemPlus.h"
#include "Include/PapyrusNames.h"

#include <filesystem>

namespace SystemPlus {
    bool DoesExist(std::string dllName) {
        if (dllName.empty()) return false;

        if (!dllName.ends_with(".dll") && !dllName.ends_with(".DLL")) {
            dllName += ".dll";
        }

        std::error_code ec;
        return std::filesystem::exists(std::filesystem::path("Data/SKSE/Plugins") / dllName, ec);
    }

    bool DoesPluginExist(RE::StaticFunctionTag*, RE::BSFixedString a_dllName) {
        return DoesExist(a_dllName.c_str());
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::fDoesPluginExist, PapyrusNames::PapyrusSystemScript, DoesPluginExist);
    }
}