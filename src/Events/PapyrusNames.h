#pragma once

// Centralized names for every Papyrus script, native function and event.
namespace PapyrusNames {

    //  PAPYRUS SCRIPTS

    static const char* PapyrusActorScript   = "ShelbyExtendedActor";
    static const char* PapyrusQuestScript   = "ShelbyExtendedQuests";
    static const char* PapyrusWeatherScript = "ShelbyExtendedWeather";
    static const char* PapyrusSystemScript = "ShelbyExtendedMisc";

    //  FUNCTIONS - ShelbyExtendedActor

    // --- State checks ---
    static const char* fActorFemale         = "IsActorFemale";
    static const char* fActorWerewolf       = "IsActorWerewolf";
    static const char* fActorExterior       = "IsActorInExterior";
    static const char* fActorFurniture      = "IsActorUsingFurniture";

    // --- Combat ---
    static const char* fActorAttacking      = "IsActorAttacking";
    static const char* fActorPowerAttacking = "IsActorPowerAttacking";
    static const char* fActorEquippedWeapon = "GetActorEquippedWeaponType";
    static const char* fLandingHit          = "IsActorLandingHit";
    static const char* fLastHitTarget       = "GetActorLastHitTarget";
    static const char* fActorSinceBleedoutTime = "GetActorTimeSinceBleedout";

    // --- Inventory ---
    static const char* fActorInvWeight      = "GetActorInventoryWeight";
    static const char* fActorInvItemCount   = "GetActorInventoryItemCount";
    static const char* fActorInvGoldValue   = "GetActorInventoryGoldValue";

    //  FUNCTIONS - ShelbyExtendedWeather

    // --- Time & calendar ---
    static const char* fWeatherHourBetween  = "IsGameHourBetween";
    static const char* fWeatherGameHour     = "GetCurrentGameHour";
    static const char* fWeatherDayName      = "GetCurrentDayName";
    static const char* fWeatherMonthName    = "GetCurrentMonthName";
    static const char* fWeatherMoonPhase    = "GetCurrentMoonPhase";

    // --- Weather ---
    static const char* fWeatherWindSpeed    = "GetWindSpeed";
    static const char* fWeatherWindAngle    = "GetWindAngle";
    static const char* fGetWeatherEditorID  = "GetWeatherEditorID";

    // --- System ---
    static const char* fDoesPluginExist = "DoesPluginExist";

    // ============================================================
    //  EVENTS - PLAYER (registered with Quest only)
    // ============================================================
    // --- Attack (one registration, three events) ---
    static const char* kAttackRegistryName         = "AttackEvents";
    static const char* kPlayerAttackEventName      = "OnPlayerAttack";
    static const char* kPlayerMgcAttackEventName   = "OnPlayerMagicAttack";
    static const char* kPlayerWWAttackEventName    = "OnPlayerWerewolfAttack";
    static const char* kRegisterPlayerAttackName   = "RegisterAttackEvent";
    static const char* kUnregisterPlayerAttackName = "UnregisterAttackEvent";

    // --- Lockpick (one registration, three events) ---
    static const char* kLockpickRegistryName         = "LockpickEvents";
    static const char* kPlayerLockpickEventName      = "OnPlayerLockpickSuccess";
    static const char* kPlayerLockpickFailEventName  = "OnPlayerLockpickFail";
    static const char* kPlayerLockpickBrokeEventName = "OnPlayerLockpickBroke";
    static const char* kRegisterLockpickName         = "RegisterLockpickEvent";
    static const char* kUnregisterLockpickName       = "UnregisterLockpickEvent";

    // --- Sex changed ---
    static const char* kPlayerSexChangedEventName = "OnPlayerSexChanged";
    static const char* kRegisterSexChangedName    = "RegisterSexChanged";
    static const char* kUnregisterSexChangedName  = "UnregisterSexChanged";

    // --- Werewolf feeding ---
    static const char* kPlayerWWFeedingRegistryName = "WWFeedingEvents";
    static const char* kPlayerWWFeedingEventName    = "OnPlayerWerewolfFeeding";
    static const char* kRegisterPlayerWWFeeding     = "RegisterWerewolfFeeding";
    static const char* kUnregisterPlayerWWFeeding   = "UnregisterWerewolfFeeding";

    // --- Harvested plant ---
    static const char* kPlayerHarvestedPlantEventName = "OnPlayerHarvestedPlant";
    static const char* kRegisterHarvestedPlantName    = "RegisterHarvestedPlant";
    static const char* kUnregisterHarvestedPlantName  = "UnregisterHarvestedPlant";
    // ============================================================
    //  EVENTS - ACTOR (registered with Quest + Actor, None = any actor)
    // ============================================================
    // --- Bleedout ---
    static const char* kBleedoutEventName      = "OnActorBleedout";
    static const char* kRegisterBleedoutName   = "RegisterActorBleedout";
    static const char* kUnregisterBleedoutName = "UnregisterActorBleedout";

    // --- Furniture ---
    static const char* kFurnitureEventName      = "OnActorUsingFurniture";
    static const char* kRegisterFurnitureName   = "RegisterActorFurnitureUsed";
    static const char* kUnregisterFurnitureName = "UnregisterActorFurnitureUsed";

    // --- Race switch ---
    static const char* kActorRaceSwitchEventName = "OnActorRaceSwitch";
    static const char* kRegisterRaceSwitchName   = "RegisterActorRaceSwitch";
    static const char* kUnregisterRaceSwitchName = "UnregisterActorRaceSwitch";

    // ============================================================
    //  EVENTS - WEATHER (registered with Quest)
    // ============================================================
    // --- Weather Change ---
    static constexpr auto kWeatherChangedEventName     = "OnWeatherChanged";
    static constexpr auto kRegisterWeatherChangedName   = "RegisterWeatherChanged";
    static constexpr auto kUnregisterWeatherChangedName = "UnregisterWeatherChanged";
}