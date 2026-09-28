#pragma once

namespace OnWeatherChanged {
    void Register(RE::BSScript::IVirtualMachine* a_vm);
    void RegisterEvents();
    void InstallEarly();
    void Reset();
}