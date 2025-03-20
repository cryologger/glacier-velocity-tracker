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
    DEBUG_PRINTLN("[microSD] Info: Already initialized.");
    return;
  }

  displayInitialize("microSD");  // Display OLED message.

  // Attempt microSD initialization with a maximum of 2 retries.
  for (int attempt = 1; attempt <= 2; attempt++) {
    if (sd.begin(PIN_SD_CS, SD_SCK_MHZ(24))) {
      online.microSd = true;  // Set flag.
      DEBUG_PRINTLN("[microSD] Info: Initialized successfully.");
      displaySuccess();  // Display OLED success message.

      // Get storage information
      getSdSpaceInfo();
      getSdFileCount();
      break;  // Exit retry loop on success.
    }

    DEBUG_PRINTLN("[microSD] Warning: Initialization failed. Retrying...");
    displayErrorMicrosd1();  // Display OLED error message.
    myDelay(2000);           // Non-blocking delay before retry.

    // On second failure, log error and disable peripherals.
    if (attempt == 2) {
      DEBUG_PRINTLN("[microSD] Error: Failed to initialize.");
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
  // Initially assume the config is valid
  bool configValid = true;

  if (!online.microSd) {
    DEBUG_PRINTLN("[microSD] Warning: microSD not available.");
    return false;
  }

  FsFile configFile = sd.open("config.json", FILE_READ);
  if (!configFile) {
    DEBUG_PRINTLN("[microSD] Warning: 'config.json' not found on SD. Using defaults.");
    displayConfigStatus(false);
    return false;
  }

  // Allocate the JSON document
  JsonDocument doc;

  DeserializationError err = deserializeJson(doc, configFile);
  configFile.close();

  if (err) {
    DEBUG_PRINTLN("[microSD] Error: JSON parse failed. Using defaults.");
    displayConfigStatus(false);
    return false;
  }

  DEBUG_PRINTLN("[microSD] Info: Validating config.json...");

  // --------------------------
  // Declare local temp variables
  // --------------------------
  char tmpUid[32];

  OperationMode tmpOpMode;
  SeasonalMode tmpSeasonalMode;

  int tmpStartHour, tmpStartMin,
    tmpStopHour, tmpStopMin,
    tmpAwakeHours, tmpAwakeMinutes,
    tmpSleepHours, tmpSleepMinutes;

  int tmpSeasonalStartDay, tmpSeasonalStartMonth,
    tmpSeasonalEndDay, tmpSeasonalEndMonth;

  int tmpGnssRate;
  byte tmpGpsEnabled, tmpGloEnabled,
    tmpGalEnabled, tmpBdsEnabled,
    tmpSbasEnabled, tmpQzssEnabled;
  // --------------------------
  // Validate each field
  // --------------------------

  // uid
  if (doc["uid"].is<const char*>()) {
    strncpy(tmpUid, doc["uid"].as<const char*>(), sizeof(tmpUid) - 1);
    tmpUid[sizeof(tmpUid) - 1] = '\0';
  } else {
    DEBUG_PRINTLN("[Config] Error: 'uid' missing or invalid.");
    configValid = false;
  }

  // operationMode
  if (doc["operationMode"].is<const char*>()) {
    const char* modeStr = doc["operationMode"].as<const char*>();
    if (strcmp(modeStr, "DAILY") == 0) tmpOpMode = DAILY;
    else if (strcmp(modeStr, "ROLLING") == 0) tmpOpMode = ROLLING;
    else if (strcmp(modeStr, "CONTINUOUS") == 0) tmpOpMode = CONTINUOUS;
    else {
      DEBUG_PRINTLN("[Config] Error: 'operationMode' not recognized.");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'operationMode' missing or invalid.");
    configValid = false;
  }

  // dailyStartHour
  if (doc["dailyStartHour"].is<int>()) {
    tmpStartHour = doc["dailyStartHour"].as<int>();
    if (tmpStartHour < 0 || tmpStartHour > 23) {
      DEBUG_PRINTLN("[Config] Error: 'dailyStartHour' out of range (0–23).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'dailyStartHour' missing or invalid.");
    configValid = false;
  }

  // dailyStartMinute
  if (doc["dailyStartMinute"].is<int>()) {
    tmpStartMin = doc["dailyStartMinute"].as<int>();
    if (tmpStartMin < 0 || tmpStartMin > 59) {
      DEBUG_PRINTLN("[Config] Error: 'dailyStartMinute' out of range (0–59).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'dailyStartMinute' missing or invalid.");
    configValid = false;
  }

  // dailyStopHour
  if (doc["dailyStopHour"].is<int>()) {
    tmpStopHour = doc["dailyStopHour"].as<int>();
    if (tmpStopHour < 0 || tmpStopHour > 23) {
      DEBUG_PRINTLN("[Config] Error: 'dailyStopHour' out of range (0–23).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'dailyStopHour' missing or invalid.");
    configValid = false;
  }

  // dailyStopMinute
  if (doc["dailyStopMinute"].is<int>()) {
    tmpStopMin = doc["dailyStopMinute"].as<int>();
    if (tmpStopMin < 0 || tmpStopMin > 59) {
      DEBUG_PRINTLN("[Config] Error: 'dailyStopMinute' out of range (0–59).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'dailyStopMinute' missing or invalid.");
    configValid = false;
  }

  // rollingAwakeHours
  if (doc["rollingAwakeHours"].is<int>()) {
    tmpAwakeHours = doc["rollingAwakeHours"].as<int>();
  } else {
    DEBUG_PRINTLN("[Config] Error: 'rollingAwakeHours' missing or invalid.");
    configValid = false;
  }

  // rollingAwakeMinutes
  if (doc["rollingAwakeMinutes"].is<int>()) {
    tmpAwakeMinutes = doc["rollingAwakeMinutes"].as<int>();
  } else {
    DEBUG_PRINTLN("[Config] Error: 'rollingAwakeMinutes' missing or invalid.");
    configValid = false;
  }

  // rollingSleepHours
  if (doc["rollingSleepHours"].is<int>()) {
    tmpSleepHours = doc["rollingSleepHours"].as<int>();
  } else {
    DEBUG_PRINTLN("[Config] Error: 'rollingSleepHours' missing or invalid.");
    configValid = false;
  }

  // rollingSleepMinutes
  if (doc["rollingSleepMinutes"].is<int>()) {
    tmpSleepMinutes = doc["rollingSleepMinutes"].as<int>();
  } else {
    DEBUG_PRINTLN("[Config] Error: 'rollingSleepMinutes' missing or invalid.");
    configValid = false;
  }

  // seasonalLoggingMode
  if (doc["seasonalLoggingMode"].is<const char*>()) {
    const char* sMode = doc["seasonalLoggingMode"].as<const char*>();
    if (strcmp(sMode, "ENABLED") == 0) tmpSeasonalMode = ENABLED;
    else if (strcmp(sMode, "DISABLED") == 0) tmpSeasonalMode = DISABLED;
    else {
      DEBUG_PRINTLN("[Config] Error: 'seasonalLoggingMode' not recognized.");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'seasonalLoggingMode' missing or invalid.");
    configValid = false;
  }

  // seasonalStartDay
  if (doc["seasonalStartDay"].is<int>()) {
    tmpSeasonalStartDay = doc["seasonalStartDay"].as<int>();
    if (tmpSeasonalStartDay < 1 || tmpSeasonalStartDay > 31) {
      DEBUG_PRINTLN("[Config] Error: 'seasonalStartDay' out of range (1–31).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'seasonalStartDay' missing or invalid.");
    configValid = false;
  }

  // seasonalStartMonth
  if (doc["seasonalStartMonth"].is<int>()) {
    tmpSeasonalStartMonth = doc["seasonalStartMonth"].as<int>();
    if (tmpSeasonalStartMonth < 1 || tmpSeasonalStartMonth > 12) {
      DEBUG_PRINTLN("[Config] Error: 'seasonalStartMonth' out of range (1–12).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'seasonalStartMonth' missing or invalid.");
    configValid = false;
  }

  // seasonalEndDay
  if (doc["seasonalEndDay"].is<int>()) {
    tmpSeasonalEndDay = doc["seasonalEndDay"].as<int>();
    if (tmpSeasonalEndDay < 1 || tmpSeasonalEndDay > 31) {
      DEBUG_PRINTLN("[Config] Error: 'seasonalEndDay' out of range (1–31).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'seasonalEndDay' missing or invalid.");
    configValid = false;
  }

  // seasonalEndMonth
  if (doc["seasonalEndMonth"].is<int>()) {
    tmpSeasonalEndMonth = doc["seasonalEndMonth"].as<int>();
    if (tmpSeasonalEndMonth < 1 || tmpSeasonalEndMonth > 12) {
      DEBUG_PRINTLN("[Config] Error: 'seasonalEndMonth' out of range (1–12).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'seasonalEndMonth' missing or invalid.");
    configValid = false;
  }

  // gnssMeasurementRate
  if (doc["gnssMeasurementRate"].is<int>()) {
    tmpGnssRate = doc["gnssMeasurementRate"].as<int>();
    if (tmpGnssRate < 250 || tmpGnssRate > 30000) {
      DEBUG_PRINTLN("[Config] Error: 'gnssMeasurementRate' out of range (250–30000).");
      configValid = false;
    }
  } else {
    DEBUG_PRINTLN("[Config] Error: 'gnssMeasurementRate' missing or invalid.");
    configValid = false;
  }

  // gnssGpsEnabled
  if (doc["gnssGpsEnabled"].is<int>()) {
    int val = doc["gnssGpsEnabled"].as<int>();
    if (val == 0 || val == 1) {
      tmpGpsEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN("[Config] Error: 'gnssGpsEnabled' must be 0 or 1.");
      configValid = false;
    }
  }

  // gnssGloEnabled
  if (doc["gnssGloEnabled"].is<int>()) {
    int val = doc["gnssGloEnabled"].as<int>();
    if (val == 0 || val == 1) {
      tmpGloEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN("[Config] Error: 'gnssGloEnabled' must be 0 or 1.");
      configValid = false;
    }
  }

  // gnssGalEnabled
  if (doc["gnssGalEnabled"].is<int>()) {
    int val = doc["gnssGalEnabled"].as<int>();
    if (val == 0 || val == 1) {
      tmpGalEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN("[Config] Error: 'gnssGalEnabled' must be 0 or 1.");
      configValid = false;
    }
  }

  // gnssBdsEnabled
  if (doc["gnssBdsEnabled"].is<int>()) {
    int val = doc["gnssBdsEnabled"].as<int>();
    if (val == 0 || val == 1) {
      tmpBdsEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN("[Config] Error: 'gnssBdsEnabled' must be 0 or 1.");
      configValid = false;
    }
  }

  // gnssSbasEnabled
  if (doc["gnssSbasEnabled"].is<int>()) {
    int val = doc["gnssSbasEnabled"].as<int>();
    if (val == 0 || val == 1) {
      tmpSbasEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN("[Config] Error: 'gnssSbasEnabled' must be 0 or 1.");
      configValid = false;
    }
  }

  // gnssQzssEnabled
  if (doc["gnssQzssEnabled"].is<int>()) {
    int val = doc["gnssQzssEnabled"].as<int>();
    if (val == 0 || val == 1) {
      tmpQzssEnabled = (byte)val;
    } else {
      DEBUG_PRINTLN("[Config] Error: 'gnssQzssEnabled' must be 0 or 1.");
      configValid = false;
    }
  }

  // ---------------------------------------
  // If configValid is still true, commit
  // ---------------------------------------
  if (configValid) {
    // Copy from temp vars into global
    strncpy(uid, tmpUid, sizeof(uid) - 1);
    uid[sizeof(uid) - 1] = '\0';

    operationMode = tmpOpMode;
    normalOperationMode = tmpOpMode;

    alarmStartHour = tmpStartHour;
    alarmStartMinute = tmpStartMin;
    alarmStopHour = tmpStopHour;
    alarmStopMinute = tmpStopMin;

    alarmAwakeHours = tmpAwakeHours;
    alarmAwakeMinutes = tmpAwakeMinutes;
    alarmSleepHours = tmpSleepHours;
    alarmSleepMinutes = tmpSleepMinutes;

    seasonalLoggingMode = tmpSeasonalMode;
    alarmSeasonalStartDay = tmpSeasonalStartDay;
    alarmSeasonalStartMonth = tmpSeasonalStartMonth;
    alarmSeasonalEndDay = tmpSeasonalEndDay;
    alarmSeasonalEndMonth = tmpSeasonalEndMonth;

    gnssMeasurementRate = tmpGnssRate;
    gnssGpsEnabled = tmpGpsEnabled;
    gnssGloEnabled = tmpGloEnabled;
    gnssGalEnabled = tmpGalEnabled;
    gnssBdsEnabled = tmpBdsEnabled;
    gnssSbasEnabled = tmpSbasEnabled;
    gnssQzssEnabled = tmpQzssEnabled;

    DEBUG_PRINTLN("[microSD] Info: Configuration loaded and validated!");
  } else {
    DEBUG_PRINTLN("[microSD] Warning: JSON config invalid; using defaults!");
  }
  // Always show an OLED message at the end
  displayConfigStatus(configValid);

  // Return whether we ended up with valid data or not
  return configValid;
}

// ----------------------------------------------------------------------------
// Returns the total and free space in megabytes with a single-line debug output.
// ----------------------------------------------------------------------------
void getSdSpaceInfo() {

  // Number of clusters on the volume
  uint32_t clusterCount = sd.clusterCount();
  // Number of free clusters
  int32_t freeClusterCount = sd.freeClusterCount();

  if (freeClusterCount < 0) {
    DEBUG_PRINTLN("[microSD] Error: freeClusterCount() failed!");
    return;
  }

  // Sectors per cluster
  uint32_t spc = sd.sectorsPerCluster();  // Each sector = 512 bytes

  // Compute total volume size in bytes
  uint64_t totalBytes = (uint64_t)clusterCount * spc * 512ULL;
  // Compute free space in bytes
  uint64_t freeBytes = (uint64_t)freeClusterCount * spc * 512ULL;
  // Compute used space in bytes
  //uint64_t usedBytes = totalBytes - freeBytes;

  // Convert to MB (1 MB = 1,000,000 bytes)
  sdTotalMB = (double)totalBytes / 1000000.0;
  sdFreeMB = (double)freeBytes / 1000000.0;
  sdUsedMB = sdTotalMB - sdFreeMB;

  // Print single-line debug message: "Used / Total MB"
  //DEBUG_PRINT_DEC(sdUsedMB, 1);
  //DEBUG_PRINT(" / ");
  //DEBUG_PRINT_DEC(sdTotalMB, 1);
  //DEBUG_PRINTLN(" MB");
}

// ----------------------------------------------------------------------------
// Counts the number of files in the root directory of the microSD card.
// ----------------------------------------------------------------------------
void getSdFileCount() {

  // Open the root directory
  FsFile root = sd.open("/");
  if (!root) {
    DEBUG_PRINTLN("[microSD] Error: Failed to open root directory.");
    return;
  }

  sdFileCount = 0;

  FsFile file;

  // Iterate through files in the root directory
  while ((file = root.openNextFile())) {
    if (!file.isDirectory()) {
      sdFileCount++;
    }
    file.close();
  }
  root.close();

  // Print debug message
  //DEBUG_PRINT("[microSD] Info: File count = ");
  //DEBUG_PRINTLN(sdFileCount);
}

// ----------------------------------------------------------------------------
// Update the file creation timestamp.
// ----------------------------------------------------------------------------
void updateFileCreate(FsFile* dataFile) {
  rtc.getTime();  // Get current RTC date and time.

  if (!dataFile->timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN("[microSD] Warning: Could not update file create timestamp.");
  }
}

// ----------------------------------------------------------------------------
// Update file access and write timestamps.
// ----------------------------------------------------------------------------
void updateFileAccess(FsFile* dataFile) {
  rtc.getTime();  // Get current RTC date and time.

  if (!dataFile->timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN("[microSD] Warning: Could not update file access timestamp.");
  }

  if (!dataFile->timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN("[microSD] Warning: Could not update file write timestamp.");
  }
}