ScriptName ShelbyExtendedQuests

;NOTE: Registrations are saved with the game, register once no need to re-register on load.

;/
    Registers akQuest to receive the player attack events:
    Event OnPlayerAttack(Actor akVictim)            - weapon / unarmed hits
    Event OnPlayerMagicAttack(Actor akVictim)       - spell or enchantment hits
    Event OnPlayerWerewolfAttack(Actor akVictim)    - hits while in werewolf form
    NOTE: Define only the events you need; the others are ignored.
/;
Function RegisterAttackEvent(Quest akQuest) Global Native
Function UnregisterAttackEvent(Quest akQuest) Global Native

;/
    Registers akQuest to receive OnPlayerWerewolfFeeding when the player feeds on a corpse in werewolf form.
    NOTE: Fires on every feed attempt, including corpses that were already eaten (if applicable).

    E.g:
    RegisterWerewolfFeeding(Self)

    Event OnPlayerWerewolfFeeding(Actor akVictim)
        Debug.Notification("Fed on " + akVictim.GetDisplayName())
    EndEvent
/;
Function RegisterWerewolfFeeding(Quest akQuest) Global Native
Function UnregisterWerewolfFeeding(Quest akQuest) Global Native

;/
    Registers akQuest to receive OnPlayerHarvestedPlant when the player harvests a plant.

    E.g:
    RegisterHarvestedPlant(Self)

    Event OnPlayerHarvestedPlant(Form akProduce)   ; the ingredient the plant gave (e.g. Blue Mountain Flower)
        Debug.Notification("Harvested " + akProduce.GetName())
    EndEvent
/;
Function RegisterHarvestedPlant(Quest akQuest) Global Native
Function UnregisterHarvestedPlant(Quest akQuest) Global Native

;/
    Registers akQuest to receive OnPlayerSexChanged when the player's sex changes
    (RaceMenu or the console "sexchange" command).

    E.g:
    RegisterSexChanged(Self)

    Event OnPlayerSexChanged(Int aiNewSex)   ; 0 = Male, 1 = Female
        Debug.Notification("New sex: " + aiNewSex)
    EndEvent
/;
Function RegisterSexChanged(Quest akQuest) Global Native
Function UnregisterSexChanged(Quest akQuest) Global Native

;/
    Registers akQuest to receive OnActorBleedout when akActor enters bleedout.
    Pass None as akActor to receive the event for ANY actor.
    Event OnActorBleedout(Actor akActor)
/;
Function RegisterActorBleedout(Quest akQuest, Actor akActor) Global Native
Function UnregisterActorBleedout(Quest akQuest, Actor akActor) Global Native

;/
    Registers akQuest to receive OnActorRaceSwitch when akActor's race changes
    (vampirism, werewolf transform, SetRace, RaceMenu...).
    Pass None as akActor to receive the event for ANY actor.
    NOTE: For the player in RaceMenu, fires once when the menu closes, only if the race changed.

    Event OnActorRaceSwitch(Actor akActor, Race akNewRace)
/;
Function RegisterActorRaceSwitch(Quest akQuest, Actor akActor) Global Native
Function UnregisterActorRaceSwitch(Quest akQuest, Actor akActor) Global Native

;/
    Registers akQuest to receive OnActorUsingFurniture when akActor enters or leaves furniture.
    Pass None as akActor to receive the event for ANY actor.
    NOTE: Also fires when mounting or dismounting horses and other rideable creatures.

    E.g:
    RegisterActorFurnitureUsed(Self, Game.GetPlayer())

    Event OnActorUsingFurniture(Actor akActor, ObjectReference akFurniture, Bool abEntered)
        If abEntered
            Debug.Notification(akActor.GetDisplayName() + " using " + akFurniture.GetDisplayName())
        EndIf
    EndEvent
/;
Function RegisterActorFurnitureUsed(Quest akQuest, Actor akActor) Global Native
Function UnregisterActorFurnitureUsed(Quest akQuest, Actor akActor) Global Native

;/
    Registers akQuest to receive the player lockpicking events:
    Event OnPlayerLockpickSuccess(ObjectReference akLock)  - lockpicking menu closed, lock is open
    Event OnPlayerLockpickFail(ObjectReference akLock)     - lockpicking menu closed, lock still locked
    Event OnPlayerLockpickBroke(ObjectReference akLock)    - a lockpick broke (can fire several times per attempt)
    NOTE: Define only the events you need; the others are ignored.
/;
Function RegisterLockpickEvent(Quest akQuest) Global Native
Function UnregisterLockpickEvent(Quest akQuest) Global Native

;/ Weather Changed
   Registers the quest to receive OnWeatherChanged whenever the exterior weather changes.
   Fires on natural transitions, scripts, console (fw/sw), fast travel and waiting.
   Interiors never fire.

   Event OnWeatherChanged(Weather akOldWeather, Weather akNewWeather, String asOldName, String asNewName)
/;
Function RegisterWeatherChanged(Quest akQuest) global native
Function UnregisterWeatherChanged(Quest akQuest) global native