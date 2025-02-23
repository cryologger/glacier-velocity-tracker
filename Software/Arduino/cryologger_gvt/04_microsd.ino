/*
  microSD Module

  This module handles the initialization and management of the microSD card.
  It ensures proper initialization, manages file timestamps, and logs errors
  when failures occur.
*/

// ----------------------------------------------------------------------------
// Configure and initialize the microSD card.
// This function attempts to initialize the microSD card and sets the
// appropriate online flag. If initialization fails, it retries before shutting
// down power to conserve energy.
// ----------------------------------------------------------------------------
void configureSd() {
  unsigned long loopStartTime = millis();  // Start loop timer.

  // Check if microSD is already initialized.
  if (online.microSd) {
    DEBUG_PRINTLN(F("[microSD] Info: Already initialized."));
    return;
  }

  displayInitialize("microSD");  // Display OLED message.

  // Attempt microSD initialization with a maximum of 2 retries.
  for (int attempt = 1; attempt <= 2; attempt++) {
    if (sd.begin(PIN_SD_CS, SD_SCK_MHZ(24))) {
      online.microSd = true;  // Set flag.
      DEBUG_PRINTLN(F("[microSD] Info: Initialized successfully."));
      displaySuccess();  // Display OLED success message.

      // Attempt to load config from "config.json"
      bool loaded = loadConfigFromSd();
      if (loaded) {
        DEBUG_PRINTLN(F("[microSD] Info: Configuration loaded successfully."));
      } else {
        DEBUG_PRINTLN(F("[microSD] Info: Using fallback defaults."));
      }
      break;  // Exit retry loop on success.
    }

    DEBUG_PRINTLN(F("[microSD] Warning: Initialization failed. Retrying..."));
    displayErrorMicrosd1();  // Display OLED error message.
    myDelay(2000);           // Non-blocking delay before retry.

    // On second failure, log error and disable peripherals.
    if (attempt == 2) {
      DEBUG_PRINTLN(F("[microSD] Error: Failed to initialize."));
      online.microSd = false;  // Set flag.

      displayErrorMicrosd2();  // Display OLED failure message.
      qwiicPowerOff();         // Disable power to Qwiic connector.
      peripheralPowerOff();    // Disable power to peripherals.
    }
  }

  timer.microSd = millis() - loopStartTime;  // Stop loop timer.
}

// ---------------------------------------------------------------------------
// Attempts to open and parse "config.json" from the microSD card.
// If any required value is missing, non-numeric, or out of range, the
// entire config is rejected, and defaults are used.
// ---------------------------------------------------------------------------
bool loadConfigFromSd() {
  if (!online.microSd) {
    DEBUG_PRINTLN(F("[microSD] Warning: microSD not available."));
    return false;
  }

  FsFile configFile = sd.open("config.json", FILE_READ);
  if (!configFile) {
    DEBUG_PRINTLN(F("[microSD] Warning: 'config.json' not found on SD. Using defaults."));
    return false;
  }

  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, configFile);
  configFile.close();

  if (err) {
    DEBUG_PRINTLN(F("[microSD] Error: JSON parse failed. Using defaults."));
    return false;
  }

  DEBUG_PRINTLN(F("[microSD] Info: Validating config.json..."));

  // --------------------------
  // Validate UID (Device Identifier)
  // --------------------------
  if (doc["uid"].is<const char*>()) {
    strncpy(uid, doc["uid"].as<const char*>(), sizeof(uid) - 1);
    uid[sizeof(uid) - 1] = '\0';  // Ensure null termination
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'uid' is missing or invalid. Using default UID."));
    return false;
  }

  // --------------------------
  // Validate Operation Mode
  // --------------------------
  const char* modeStr = doc["operationMode"].as<const char*>();
  if (modeStr) {
    if (strcmp(modeStr, "DAILY") == 0) operationMode = DAILY;
    else if (strcmp(modeStr, "ROLLING") == 0) operationMode = ROLLING;
    else if (strcmp(modeStr, "CONTINUOUS") == 0) operationMode = CONTINUOUS;
    else {
      DEBUG_PRINTLN(F("[Config] Error: Invalid 'operationMode'. Using defaults."));
      return false;
    }
    normalOperationMode = operationMode;  // Store original mode for seasonal switching
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'operationMode' missing or invalid."));
    return false;
  }

  // --------------------------
  // Validate Daily Logging Times (0-23 for hours, 0-59 for minutes)
  // --------------------------
  if (doc["dailyStartHour"].is<int>()) {
    alarmStartHour = doc["dailyStartHour"].as<int>();
    if (alarmStartHour < 0 || alarmStartHour > 23) {
      DEBUG_PRINTLN(F("[Config] Error: 'dailyStartHour' out of range (0-23)."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'dailyStartHour' missing or invalid."));
    return false;
  }

  if (doc["dailyStartMinute"].is<int>()) {
    alarmStartMinute = doc["dailyStartMinute"].as<int>();
    if (alarmStartMinute < 0 || alarmStartMinute > 59) {
      DEBUG_PRINTLN(F("[Config] Error: 'dailyStartMinute' out of range (0-59)."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'dailyStartMinute' missing or invalid."));
    return false;
  }

  if (doc["dailyStopHour"].is<int>()) {
    alarmStopHour = doc["dailyStopHour"].as<int>();
    if (alarmStopHour < 0 || alarmStopHour > 23) {
      DEBUG_PRINTLN(F("[Config] Error: 'dailyStopHour' out of range (0-23)."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'dailyStopHour' missing or invalid."));
    return false;
  }

  if (doc["dailyStopMinute"].is<int>()) {
    alarmStopMinute = doc["dailyStopMinute"].as<int>();
    if (alarmStopMinute < 0 || alarmStopMinute > 59) {
      DEBUG_PRINTLN(F("[Config] Error: 'dailyStopMinute' out of range (0-59)."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'dailyStopMinute' missing or invalid."));
    return false;
  }

  // --------------------------
  // Validate Rolling Mode Times (0-23 for hours, 0-59 for minutes)
  // --------------------------
  if (doc["rollingAwakeHours"].is<int>()) {
    alarmAwakeHours = doc["rollingAwakeHours"].as<int>();
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'rollingAwakeHours' missing or invalid."));
    return false;
  }

  if (doc["rollingAwakeMinutes"].is<int>()) {
    alarmAwakeMinutes = doc["rollingAwakeMinutes"].as<int>();
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'rollingAwakeMinutes' missing or invalid."));
    return false;
  }

  if (doc["rollingSleepHours"].is<int>()) {
    alarmSleepHours = doc["rollingSleepHours"].as<int>();
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'rollingSleepHours' missing or invalid."));
    return false;
  }

  if (doc["rollingSleepMinutes"].is<int>()) {
    alarmSleepMinutes = doc["rollingSleepMinutes"].as<int>();
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'rollingSleepMinutes' missing or invalid."));
    return false;
  }

  // --------------------------
  // Validate Seasonal Mode
  // --------------------------
  const char* seasonalStr = doc["seasonalLoggingMode"].as<const char*>();
  if (seasonalStr) {
    if (strcmp(seasonalStr, "ENABLED") == 0) seasonalLoggingMode = ENABLED;
    else if (strcmp(seasonalStr, "DISABLED") == 0) seasonalLoggingMode = DISABLED;
    else {
      DEBUG_PRINTLN(F("[Config] Error: Invalid 'seasonalLoggingMode'. Using default."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'seasonalLoggingMode' missing or invalid."));
    return false;
  }

  // --------------------------
  // Validate Seasonal Window (Day: 1-31, Month: 1-12)
  // --------------------------
  if (doc["seasonalStartDay"].is<int>()) {
    alarmSeasonalStartDay = doc["seasonalStartDay"].as<int>();
    if (alarmSeasonalStartDay < 1 || alarmSeasonalStartDay > 31) {
      DEBUG_PRINTLN(F("[Config] Error: 'seasonalStartDay' out of range (1-31)."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'seasonalStartDay' missing or invalid."));
    return false;
  }

  if (doc["seasonalStartMonth"].is<int>()) {
    alarmSeasonalStartMonth = doc["seasonalStartMonth"].as<int>();
    if (alarmSeasonalStartMonth < 1 || alarmSeasonalStartMonth > 12) {
      DEBUG_PRINTLN(F("[Config] Error: 'seasonalStartMonth' out of range (1-12)."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'seasonalStartMonth' missing or invalid."));
    return false;
  }

  if (doc["seasonalEndDay"].is<int>()) {
    alarmSeasonalEndDay = doc["seasonalEndDay"].as<int>();
    if (alarmSeasonalEndDay < 1 || alarmSeasonalEndDay > 31) {
      DEBUG_PRINTLN(F("[Config] Error: 'seasonalEndDay' out of range (1-31)."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'seasonalEndDay' missing or invalid."));
    return false;
  }

  if (doc["seasonalEndMonth"].is<int>()) {
    alarmSeasonalEndMonth = doc["seasonalEndMonth"].as<int>();
    if (alarmSeasonalEndMonth < 1 || alarmSeasonalEndMonth > 12) {
      DEBUG_PRINTLN(F("[Config] Error: 'seasonalEndMonth' out of range (1-12)."));
      return false;
    }
  } else {
    DEBUG_PRINTLN(F("[Config] Error: 'seasonalEndMonth' missing or invalid."));
    return false;
  }

  // ----------------------------------------------------------------------------
  // GNSS Satellite Signal Enables (0=DISABLE, 1=ENABLE)
  // If the user provides them, override. Otherwise keep fallback.
  // ----------------------------------------------------------------------------

  // GPS
  if (doc["gnssGpsEnabled"].is<int>()) {
    int val = doc["gnssGpsEnabled"].as<int>();
    if (val == 0 || val == 1) {
      gnssGpsEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN(F("[Config] Warning: 'gnssGpsEnabled' must be 0 or 1. Using fallback."));
    }
  }

  // GLONASS
  if (doc["gnssGloEnabled"].is<int>()) {
    int val = doc["gnssGloEnabled"].as<int>();
    if (val == 0 || val == 1) {
      gnssGloEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN(F("[Config] Warning: 'gnssGloEnabled' must be 0 or 1. Using fallback."));
    }
  }

  // Galileo
  if (doc["gnssGalEnabled"].is<int>()) {
    int val = doc["gnssGalEnabled"].as<int>();
    if (val == 0 || val == 1) {
      gnssGalEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN(F("[Config] Warning: 'gnssGalEnabled' must be 0 or 1. Using fallback."));
    }
  }

  // BeiDou
  if (doc["gnssBdsEnabled"].is<int>()) {
    int val = doc["gnssBdsEnabled"].as<int>();
    if (val == 0 || val == 1) {
      gnssBdsEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN(F("[Config] Warning: 'gnssBdsEnabled' must be 0 or 1. Using fallback."));
    }
  }

  // SBAS
  if (doc["gnssSbasEnabled"].is<int>()) {
    int val = doc["gnssSbasEnabled"].as<int>();
    if (val == 0 || val == 1) {
      gnssSbasEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN(F("[Config] Warning: 'gnssSbasEnabled' must be 0 or 1. Using fallback."));
    }
  }

  // QZSS
  if (doc["gnssQzssEnabled"].is<int>()) {
    int val = doc["gnssQzssEnabled"].as<int>();
    if (val == 0 || val == 1) {
      gnssQzssEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN(F("[Config] Warning: 'gnssQzssEnabled' must be 0 or 1. Using fallback."));
    }
  }

  return true;
}

// ----------------------------------------------------------------------------
// Update the file creation timestamp.
// ----------------------------------------------------------------------------
void updateFileCreate(FsFile* dataFile) {
  rtc.getTime();  // Get current RTC date and time.

  if (!dataFile->timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN(F("[microSD] Warning: Could not update file create timestamp."));
  }
}

// ----------------------------------------------------------------------------
// Update file access and write timestamps.
// ----------------------------------------------------------------------------
void updateFileAccess(FsFile* dataFile) {
  rtc.getTime();  // Get current RTC date and time.

  if (!dataFile->timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN(F("[microSD] Warning: Could not update file access timestamp."));
  }

  if (!dataFile->timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN(F("[microSD] Warning: Could not update file write timestamp."));
  }
}