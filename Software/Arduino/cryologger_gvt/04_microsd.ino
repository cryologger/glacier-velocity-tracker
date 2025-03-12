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

      // Get storage information
      getSdSpaceInfo();
      getSdFileCount();
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

  // Allocate the JSON document
  JsonDocument doc;

  DeserializationError err = deserializeJson(doc, configFile);
  configFile.close();

  if (err) {
    DEBUG_PRINTLN(F("[microSD] Error: JSON parse failed. Using defaults."));
    return false;
  }

  DEBUG_PRINTLN(F("[microSD] Info: Validating config.json..."));

  // --------------------------
  // Declare local temp variables
  // --------------------------
  char tmpUid[32];

  OperationMode tmpOpMode;
  int tmpStartHour, tmpStartMin;
  int tmpStopHour, tmpStopMin;

  int tmpAwakeHours, tmpAwakeMinutes;
  int tmpSleepHours, tmpSleepMinutes;

  SeasonalMode tmpSeasonalMode;
  int tmpSeasonalStartDay, tmpSeasonalStartMonth;
  int tmpSeasonalEndDay, tmpSeasonalEndMonth;

  int tmpGnssRate;
  byte tmpGpsEnabled;
  byte tmpGloEnabled;
  byte tmpGalEnabled;
  byte tmpBdsEnabled;
  byte tmpSbasEnabled;
  byte tmpQzssEnabled;

  // --------------------------
  // Validate each field
  // --------------------------

  // UID
  if (!doc.containsKey("uid") || !doc["uid"].is<const char*>()) {
    DEBUG_PRINTLN("[Config] Error: 'uid' missing or invalid.");
    return false;
  }
  strncpy(tmpUid, doc["uid"].as<const char*>(), sizeof(tmpUid) - 1);
  tmpUid[sizeof(tmpUid) - 1] = '\0';

  // operationMode
  if (!doc.containsKey("operationMode") || !doc["operationMode"].is<const char*>()) {
    DEBUG_PRINTLN("[Config] Error: 'operationMode' missing or invalid.");
    return false;
  }
  {
    const char* modeStr = doc["operationMode"].as<const char*>();
    if (strcmp(modeStr, "DAILY") == 0) tmpOpMode = DAILY;
    else if (strcmp(modeStr, "ROLLING") == 0) tmpOpMode = ROLLING;
    else if (strcmp(modeStr, "CONTINUOUS") == 0) tmpOpMode = CONTINUOUS;
    else {
      DEBUG_PRINTLN("[Config] Error: 'operationMode' not recognized.");
      return false;
    }
  }

  // dailyStartHour
  if (!doc.containsKey("dailyStartHour") || !doc["dailyStartHour"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'dailyStartHour' missing or invalid.");
    return false;
  }
  tmpStartHour = doc["dailyStartHour"].as<int>();
  if (tmpStartHour < 0 || tmpStartHour > 23) {
    DEBUG_PRINTLN("[Config] Error: 'dailyStartHour' out of range (0–23).");
    return false;
  }

  // dailyStartMinute
  if (!doc.containsKey("dailyStartMinute") || !doc["dailyStartMinute"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'dailyStartMinute' missing or invalid.");
    return false;
  }
  tmpStartMin = doc["dailyStartMinute"].as<int>();
  if (tmpStartMin < 0 || tmpStartMin > 59) {
    DEBUG_PRINTLN("[Config] Error: 'dailyStartMinute' out of range (0–59).");
    return false;
  }

  // dailyStopHour
  if (!doc.containsKey("dailyStopHour") || !doc["dailyStopHour"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'dailyStopHour' missing or invalid.");
    return false;
  }
  tmpStopHour = doc["dailyStopHour"].as<int>();
  if (tmpStopHour < 0 || tmpStopHour > 23) {
    DEBUG_PRINTLN("[Config] Error: 'dailyStopHour' out of range (0–23).");
    return false;
  }

  // dailyStopMinute
  if (!doc.containsKey("dailyStopMinute") || !doc["dailyStopMinute"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'dailyStopMinute' missing or invalid.");
    return false;
  }
  tmpStopMin = doc["dailyStopMinute"].as<int>();
  if (tmpStopMin < 0 || tmpStopMin > 59) {
    DEBUG_PRINTLN("[Config] Error: 'dailyStopMinute' out of range (0–59).");
    return false;
  }

  // rollingAwakeHours
  if (!doc.containsKey("rollingAwakeHours") || !doc["rollingAwakeHours"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'rollingAwakeHours' missing or invalid.");
    return false;
  }
  tmpAwakeHours = doc["rollingAwakeHours"].as<int>();

  // rollingAwakeMinutes
  if (!doc.containsKey("rollingAwakeMinutes") || !doc["rollingAwakeMinutes"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'rollingAwakeMinutes' missing or invalid.");
    return false;
  }
  tmpAwakeMinutes = doc["rollingAwakeMinutes"].as<int>();

  // rollingSleepHours
  if (!doc.containsKey("rollingSleepHours") || !doc["rollingSleepHours"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'rollingSleepHours' missing or invalid.");
    return false;
  }
  tmpSleepHours = doc["rollingSleepHours"].as<int>();

  // rollingSleepMinutes
  if (!doc.containsKey("rollingSleepMinutes") || !doc["rollingSleepMinutes"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'rollingSleepMinutes' missing or invalid.");
    return false;
  }
  tmpSleepMinutes = doc["rollingSleepMinutes"].as<int>();

  // seasonalLoggingMode
  if (!doc.containsKey("seasonalLoggingMode") || !doc["seasonalLoggingMode"].is<const char*>()) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalLoggingMode' missing or invalid.");
    return false;
  }
  {
    const char* sMode = doc["seasonalLoggingMode"].as<const char*>();
    if (strcmp(sMode, "ENABLED") == 0) tmpSeasonalMode = ENABLED;
    else if (strcmp(sMode, "DISABLED") == 0) tmpSeasonalMode = DISABLED;
    else {
      DEBUG_PRINTLN("[Config] Error: 'seasonalLoggingMode' not recognized.");
      return false;
    }
  }

  // seasonalStartDay
  if (!doc.containsKey("seasonalStartDay") || !doc["seasonalStartDay"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalStartDay' missing or invalid.");
    return false;
  }
  tmpSeasonalStartDay = doc["seasonalStartDay"].as<int>();
  if (tmpSeasonalStartDay < 1 || tmpSeasonalStartDay > 31) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalStartDay' out of range (1–31).");
    return false;
  }

  // seasonalStartMonth
  if (!doc.containsKey("seasonalStartMonth") || !doc["seasonalStartMonth"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalStartMonth' missing or invalid.");
    return false;
  }
  tmpSeasonalStartMonth = doc["seasonalStartMonth"].as<int>();
  if (tmpSeasonalStartMonth < 1 || tmpSeasonalStartMonth > 12) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalStartMonth' out of range (1–12).");
    return false;
  }

  // seasonalEndDay
  if (!doc.containsKey("seasonalEndDay") || !doc["seasonalEndDay"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalEndDay' missing or invalid.");
    return false;
  }
  tmpSeasonalEndDay = doc["seasonalEndDay"].as<int>();
  if (tmpSeasonalEndDay < 1 || tmpSeasonalEndDay > 31) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalEndDay' out of range (1–31).");
    return false;
  }

  // seasonalEndMonth
  if (!doc.containsKey("seasonalEndMonth") || !doc["seasonalEndMonth"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalEndMonth' missing or invalid.");
    return false;
  }
  tmpSeasonalEndMonth = doc["seasonalEndMonth"].as<int>();
  if (tmpSeasonalEndMonth < 1 || tmpSeasonalEndMonth > 12) {
    DEBUG_PRINTLN("[Config] Error: 'seasonalEndMonth' out of range (1–12).");
    return false;
  }

  // gnssMeasurementRate
  if (!doc.containsKey("gnssMeasurementRate") || !doc["gnssMeasurementRate"].is<int>()) {
    DEBUG_PRINTLN("[Config] Error: 'gnssMeasurementRate' missing or invalid.");
    return false;
  }
  tmpGnssRate = doc["gnssMeasurementRate"].as<int>();
  if (tmpGnssRate < 250 || tmpGnssRate > 30000) {
    DEBUG_PRINTLN("[Config] Error: 'gnssMeasurementRate' out of range (250–30000).");
    return false;
  }

  // gnssGpsEnabled (optional, if present must be 0 or 1)
  if (doc.containsKey("gnssGpsEnabled")) {
    if (!doc["gnssGpsEnabled"].is<int>()) {
      DEBUG_PRINTLN("[Config] Error: 'gnssGpsEnabled' must be 0 or 1.");
      return false;
    }
    int val = doc["gnssGpsEnabled"].as<int>();
    if (val != 0 && val != 1) {
      DEBUG_PRINTLN("[Config] Error: 'gnssGpsEnabled' must be 0 or 1.");
      return false;
    }
    tmpGpsEnabled = (byte)val;
  }

  // gnssGloEnabled
  if (doc.containsKey("gnssGloEnabled")) {
    if (!doc["gnssGloEnabled"].is<int>()) {
      DEBUG_PRINTLN("[Config] Error: 'gnssGloEnabled' must be 0 or 1.");
      return false;
    }
    int val = doc["gnssGloEnabled"].as<int>();
    if (val != 0 && val != 1) {
      DEBUG_PRINTLN("[Config] Error: 'gnssGloEnabled' must be 0 or 1.");
      return false;
    }
    tmpGloEnabled = (byte)val;
  }

  // gnssGalEnabled
  if (doc.containsKey("gnssGalEnabled")) {
    if (!doc["gnssGalEnabled"].is<int>()) {
      DEBUG_PRINTLN("[Config] Error: 'gnssGalEnabled' must be 0 or 1.");
      return false;
    }
    int val = doc["gnssGalEnabled"].as<int>();
    if (val != 0 && val != 1) {
      DEBUG_PRINTLN("[Config] Error: 'gnssGalEnabled' must be 0 or 1.");
      return false;
    }
    tmpGalEnabled = (byte)val;
  }

  // gnssBdsEnabled
  if (doc.containsKey("gnssBdsEnabled")) {
    if (!doc["gnssBdsEnabled"].is<int>()) {
      DEBUG_PRINTLN("[Config] Error: 'gnssBdsEnabled' must be 0 or 1.");
      return false;
    }
    int val = doc["gnssBdsEnabled"].as<int>();
    if (val != 0 && val != 1) {
      DEBUG_PRINTLN("[Config] Error: 'gnssBdsEnabled' must be 0 or 1.");
      return false;
    }
    tmpBdsEnabled = (byte)val;
  }

  // gnssSbasEnabled
  if (doc.containsKey("gnssSbasEnabled")) {
    if (!doc["gnssSbasEnabled"].is<int>()) {
      DEBUG_PRINTLN("[Config] Error: 'gnssSbasEnabled' must be 0 or 1.");
      return false;
    }
    int val = doc["gnssSbasEnabled"].as<int>();
    if (val != 0 && val != 1) {
      DEBUG_PRINTLN("[Config] Error: 'gnssSbasEnabled' must be 0 or 1.");
      return false;
    }
    tmpSbasEnabled = (byte)val;
  }

  // gnssQzssEnabled
  if (doc.containsKey("gnssQzssEnabled")) {
    if (!doc["gnssQzssEnabled"].is<int>()) {
      DEBUG_PRINTLN("[Config] Error: 'gnssQzssEnabled' must be 0 or 1.");
      return false;
    }
    int val = doc["gnssQzssEnabled"].as<int>();
    if (val != 0 && val != 1) {
      DEBUG_PRINTLN("[Config] Error: 'gnssQzssEnabled' must be 0 or 1.");
      return false;
    }
    tmpQzssEnabled = (byte)val;
  }

  // --------------------------
  // If we reach here, it's all valid. Commit to globals:
  // --------------------------
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
  return true;
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
    DEBUG_PRINTLN(F("[microSD] Error: freeClusterCount() failed!"));
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
  //DEBUG_PRINT(F(" / "));
  //DEBUG_PRINT_DEC(sdTotalMB, 1);
  //DEBUG_PRINTLN(F(" MB"));
}

// ----------------------------------------------------------------------------
// Counts the number of files in the root directory of the microSD card.
// ----------------------------------------------------------------------------
void getSdFileCount() {

  // Open the root directory
  FsFile root = sd.open("/");
  if (!root) {
    DEBUG_PRINTLN(F("[microSD] Error: Failed to open root directory."));
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
  //DEBUG_PRINT(F("[microSD] Info: File count = "));
  //DEBUG_PRINTLN(sdFileCount);
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