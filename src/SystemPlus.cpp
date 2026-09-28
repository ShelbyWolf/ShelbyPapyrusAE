#include "SystemPlus.h"
#include "Events/PapyrusNames.h"

#include <filesystem>

namespace SystemPlus {
    namespace {
        bool DoesPluginExist(RE::StaticFunctionTag*, RE::BSFixedString a_dllName) {
            if (a_dllName.empty()) return false;

            std::string name = a_dllName.c_str();
            if (!name.ends_with(".dll") && !name.ends_with(".DLL")) {
                name += ".dll";
            }

            std::error_code ec;
            return std::filesystem::exists(std::filesystem::path("Data/SKSE/Plugins") / name, ec);
        }
    }

    void Register(RE::BSScript::IVirtualMachine* a_vm) {
        a_vm->RegisterFunction(PapyrusNames::fDoesPluginExist, PapyrusNames::PapyrusSystemScript, DoesPluginExist);
    }
}