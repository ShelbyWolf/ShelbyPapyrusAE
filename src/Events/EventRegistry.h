#pragma once
#include "PCH.h"

#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace Events {

    class IEventRegistry {
    public:
        virtual ~IEventRegistry() = default;
        virtual std::uint32_t GetRecordType() const = 0;
        virtual void Save(SKSE::SerializationInterface* a_intf) = 0;
        virtual void Load(SKSE::SerializationInterface* a_intf, std::uint32_t a_version) = 0;
        virtual void Revert() = 0;
    };

    inline std::vector<IEventRegistry*>& GetAllRegistries() {
        static std::vector<IEventRegistry*> registries;
        return registries;
    }

    template <class... Args>
    class EventRegistry : public IEventRegistry {
    public:
        static constexpr std::uint32_t kVersion = 1;
        static constexpr RE::FormID kAnyActor = 0;

        EventRegistry(std::string a_eventName, std::uint32_t a_recordType) :
            _eventName(std::move(a_eventName)),
            _recordType(a_recordType) {
            GetAllRegistries().push_back(this);
        }

        void Register(const RE::TESQuest* a_quest, const RE::Actor* a_filter) {
            if (!a_quest) return;
            std::lock_guard lock(_mutex);
            _registrations[ToFilter(a_filter)].insert(a_quest->GetFormID());
            SKSE::log::info("[{}] registered quest {:08X} for actor {:08X}",
                _eventName, a_quest->GetFormID(), ToFilter(a_filter));
        }

        void Unregister(const RE::TESQuest* a_quest, const RE::Actor* a_filter) {
            if (!a_quest) return;
            std::lock_guard lock(_mutex);
            auto it = _registrations.find(ToFilter(a_filter));
            if (it == _registrations.end()) return;
            it->second.erase(a_quest->GetFormID());
            if (it->second.empty()) _registrations.erase(it);
            SKSE::log::info("[{}] unregistered quest {:08X}", _eventName, a_quest->GetFormID());
        }

        void Send(const RE::Actor* a_filterActor, Args... a_args) {
            SendImpl(RE::BSFixedString(_eventName), a_filterActor, a_args...);
        }

        void SendAs(std::string_view a_eventName, const RE::Actor* a_filterActor, Args... a_args) {
            SendImpl(RE::BSFixedString(a_eventName), a_filterActor, a_args...);
        }

        std::uint32_t GetRecordType() const override { return _recordType; }

        void Save(SKSE::SerializationInterface* a_intf) override {
            std::lock_guard lock(_mutex);
            if (!a_intf->OpenRecord(_recordType, kVersion)) {
                SKSE::log::error("[{}] Failed to open co-save record", _eventName);
                return;
            }

            std::uint32_t count = 0;
            for (const auto& [filter, questIDs] : _registrations) {
                count += static_cast<std::uint32_t>(questIDs.size());
            }
            a_intf->WriteRecordData(count);

            for (const auto& [filter, questIDs] : _registrations) {
                for (auto questID : questIDs) {
                    a_intf->WriteRecordData(questID);
                    a_intf->WriteRecordData(filter);
                }
            }
        }

        void Load(SKSE::SerializationInterface* a_intf, std::uint32_t) override {
            std::lock_guard lock(_mutex);
            _registrations.clear();

            std::uint32_t count = 0;
            if (!a_intf->ReadRecordData(count)) return;

            std::uint32_t loaded = 0;
            for (std::uint32_t i = 0; i < count; ++i) {
                RE::FormID questID = 0;
                RE::FormID filter = 0;
                if (!a_intf->ReadRecordData(questID) || !a_intf->ReadRecordData(filter)) break;

                if (!a_intf->ResolveFormID(questID, questID)) continue;
                if (filter != kAnyActor && !a_intf->ResolveFormID(filter, filter)) continue;

                _registrations[filter].insert(questID);
                ++loaded;
            }
            SKSE::log::info("[{}] Restored {}/{} registrations", _eventName, loaded, count);
        }

        void Revert() override {
            std::lock_guard lock(_mutex);
            _registrations.clear();
        }

    private:
        void SendImpl(const RE::BSFixedString& a_eventName, const RE::Actor* a_filterActor, Args... a_args) {
            std::unordered_set<RE::FormID> quests;
            {
                std::lock_guard lock(_mutex);
                if (a_filterActor) {
                    Collect(a_filterActor->GetFormID(), quests);
                }
                Collect(kAnyActor, quests);
            }
            if (quests.empty()) return;

            auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
            if (!vm) return;
            auto* policy = vm->GetObjectHandlePolicy();
            if (!policy) return;

            for (auto questID : quests) {
                auto* quest = RE::TESForm::LookupByID<RE::TESQuest>(questID);
                if (!quest) continue;

                const auto handle = policy->GetHandleForObject(static_cast<RE::VMTypeID>(RE::TESQuest::FORMTYPE), quest);
                if (handle == policy->EmptyHandle()) continue;

                auto* args = RE::MakeFunctionArguments(Args(a_args)...);
                vm->SendEvent(handle, a_eventName, args);
            }
        }

        static RE::FormID ToFilter(const RE::Actor* a_actor) {
            return a_actor ? a_actor->GetFormID() : kAnyActor;
        }

        void Collect(RE::FormID a_filter, std::unordered_set<RE::FormID>& a_out) const {
            auto it = _registrations.find(a_filter);
            if (it != _registrations.end()) {
                a_out.insert(it->second.begin(), it->second.end());
            }
        }

        std::string _eventName;
        std::uint32_t _recordType;
        mutable std::mutex _mutex;
        std::unordered_map<RE::FormID, std::unordered_set<RE::FormID>> _registrations;
    };
}
