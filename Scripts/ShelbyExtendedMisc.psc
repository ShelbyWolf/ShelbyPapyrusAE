ScriptName ShelbyExtendedMisc

;/
    Returns True if the DLL exists in Data/SKSE/Plugins (installed, not necessarily loaded in Mod managers).
    The ".dll" extension is optional.
    E.g: DoesPluginExist("yourPluginName")

    If DoesPluginExist("yourPluginName")
        Debug.Notification("This DLL exists")
    EndIf
/;
Bool Function DoesPluginExist(String asDllName) Global Native