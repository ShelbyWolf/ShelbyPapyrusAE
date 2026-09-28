ScriptName ShelbyExtendedActor

;/
    Returns True if the actor's base is female.
/;
Bool Function IsActorFemale(Actor akActor) Global Native

;/
    Returns True if the actor is currently in werewolf beast form.
    NOTE: Returns False if the actor is None.
    NOTE: Vampire Lord form returns False.
/;
Bool Function IsActorWerewolf(Actor akActor) Global Native

;/
    Returns True if the actor is currently in an exterior cell (e.g. Whiterun).
/;
Bool Function IsActorInExterior(Actor akActor) Global Native

;/
    Returns True if the actor is currently performing an attack (Excludes casting spells like Flames or Frostbite)
    NOTE: Returns False if the actor is dead or None ( Obviously o.O ).
/;
Bool Function IsActorAttacking(Actor akActor) Global Native

;/
    Returns True if the actor is currently performing a power attack.
    NOTE: Power attacks also return True from IsActorAttacking.
    NOTE: Returns False if the actor is dead or None ( Obviously o.O ).
/;
Bool Function IsActorPowerAttacking(Actor akActor) Global Native

;/
    Returns True if the actor is currently using furniture (chair, bed, crafting station...).
/;
Bool Function IsActorUsingFurniture(Actor akActor) Global Native

;/
    Returns True once after the actor lands a hit, then resets (consumes the flag).
/;
Bool Function IsActorLandingHit(Actor akActor) Global Native

;/
    Returns the last actor this actor hit, or None.
/;
Actor Function GetActorLastHitTarget(Actor akActor) Global Native

;/
    Returns real-time seconds since the actor last entered bleedout.
    NOTE: Keeps counting while menus are open, and resets after loading a save.
    NOTE: Returns -1.0 if the actor hasn't bled out this session.
/;
Float Function GetActorTimeSinceBleedout(Actor akActor) Global Native

;/
    Returns the weapon type of the actor's right-hand weapon.
    -1 = Invalid, 0 = Unarmed, 1 = OneHandSword, 2 = OneHandDagger, 3 = OneHandAxe,
    4 = OneHandMace, 5 = TwoHandSword, 6 = TwoHandAxe, 7 = Bow, 8 = Staff, 9 = Crossbow
    NOTE: Returns 0 (Unarmed) when no weapon or a spell is equipped.
    NOTE: Returns -1 if the actor is dead or None.
/;
Int Function GetActorEquippedWeaponType(Actor akActor) Global Native

;/
    Returns the total weight of the actor's inventory.
    NOTE: Excludes equipped armors and weapons.
/;
Float Function GetActorInventoryWeight(Actor akActor) Global Native

;/
    Returns the total number of items in the actor's inventory.
    NOTE: Excludes quest items.
/;
Int Function GetActorInventoryItemCount(Actor akActor) Global Native

;/
    Returns the total gold value of the actor's inventory (base values).
    E.g: An item worth 2 gold and an item worth 3 gold return 5.
    NOTE: Carried gold coins count toward the total.
/;
Int Function GetActorInventoryGoldValue(Actor akActor) Global Native