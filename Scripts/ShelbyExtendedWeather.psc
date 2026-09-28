ScriptName ShelbyExtendedWeather

;/ Get Weather Editor ID
   Returns the editor ID of the given weather (e.g. "SkyrimClearSR", "SkyrimStormSnow").
   Weathers have no display name, so this is the only readable name they have.
   Returns "" if the weather is None or its ID couldn't be found.
/;
String Function GetWeatherEditorID(Weather akWeather) global native

;/
    Returns True if the current in-game hour is between afHour1 and afHour2.
    E.g: IsGameHourBetween(22.0, 6.0) = True between 10pm and 6am (overnight ranges supported).
    NOTE: Returns False if both hours are equal.
/;
Bool Function IsGameHourBetween(Float afHour1, Float afHour2) Global Native

;/
    Returns the current in-game hour (0.0 to 23.99).
/;
Float Function GetCurrentGameHour() Global Native

;/
    Returns the current day of the week (e.g. "Sundas", "Morndas").
/;
String Function GetCurrentDayName() Global Native

;/
    Returns the current month (e.g. "Morning Star", "Sun's Dawn").
/;
String Function GetCurrentMonthName() Global Native

;/
    Returns the current wind speed, from 0.0 (calm) to 1.0 (strongest).
    NOTE: Returns 0.0 when the player is in an interior.
/;
Float Function GetWindSpeed() Global Native

;/
    Returns the current wind direction in degrees (0.0 to 360.0).
    NOTE: Returns 0.0 when the player is in an interior.
/;
Float Function GetWindAngle() Global Native

;/
    Returns Masser's current phase (0 to 7).
    0 = Full, 1 = Waning Gibbous, 2 = Waning Quarter, 3 = Waning Crescent,
    4 = New Moon, 5 = Waxing Crescent, 6 = Waxing Quarter, 7 = Waxing Gibbous
    NOTE: Returns -1 on error.
/;
Int Function GetCurrentMoonPhase() Global Native