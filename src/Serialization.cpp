#include "Serialization.h"
#include "Events/EventRegistry.h"

namespace Serialization {
    namespace {
        void OnSave(SKSE::SerializationInterface* a_intf) {
            for (auto* registry : Events::GetAllRegistries()) {
                registry->Save(a_intf);
            }
        }

        void OnLoad(SKSE::SerializationInterface* a_intf) {
            std::uint32_t type = 0;
            std::uint32_t version = 0;
            std::uint32_t length = 0;
            while (a_intf->GetNextRecordInfo(type, version, length)) {
                bool handled = false;
                for (auto* registry : Events::GetAllRegistries()) {
                    if (registry->GetRecordType() == type) {
                        registry->Load(a_intf, version);
                        handled = true;
                        break;
                    }
                }
                if (!handled) {
                    SKSE::log::warn("Unknown co-save record {:08X}, skipping", type);
                }
            }
        }

        void OnRevert(SKSE::SerializationInterface*) {
            for (auto* registry : Events::GetAllRegistries()) {
                registry->Revert();
            }
        }
    }

    void Install() {
        auto* intf = SKSE::GetSerializationInterface();
        if (!intf) {
            SKSE::log::critical("Serialization interface unavailable - event registrations will not persist");
            return;
        }
        intf->SetUniqueID(kUniqueID);
        intf->SetSaveCallback(OnSave);
        intf->SetLoadCallback(OnLoad);
        intf->SetRevertCallback(OnRevert);
    }
}
