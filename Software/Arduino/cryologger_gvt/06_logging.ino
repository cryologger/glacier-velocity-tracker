/*
  Logging Module

  This module handles the creation and management of log files. It generates 
  timestamped filenames, logs debugging information, and ensures data integrity 
  by syncing and closing files properly. 

  ----------------------------------------------------------------------------
  Log Files:
  ----------------------------------------------------------------------------
  - UBX Log: Stores raw GNSS data with a timestamped filename.
  - Debug Log: Stores system status, timers, and errors in CSV format.
*/

// Create a timestamped log file name.
//
// Generates a filename using the current RTC timestamp. This ensures that
// each log session is uniquely named and avoids overwriting previous logs.
void getLogFileName() {
  sprintf(logFileName, "%s-20%02d%02d%02d_%02d%02d%02d.ubx",
          SERIAL, rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  DEBUG_PRINT(F("[Logging] Info: logFileName = "));
  DEBUG_PRINTLN(logFileName);
}

// Create a debugging log file.
//
// Initializes the debug log file and writes the CSV header if the file is
// newly created. Ensures that debug logs persist across sessions.
void createDebugFile() {
  // Generate debug log filename.
  sprintf(debugFileName, "%s-debug.csv", SERIAL);

  // Open or create the debug log file.
  // O_CREAT - Creates the file if it does not exist.
  // O_APPEND - Seeks to the end of the file prior to each write.
  // O_WRITE - Open the file for writing.
  if (!debugFile.open(debugFileName, O_CREAT | O_APPEND | O_WRITE)) {
    DEBUG_PRINTLN(F("[Logging] Warning: Failed to create debug file."));
    return;
  }
  DEBUG_PRINT(F("[Logging] Info: Created debug file: "));
  DEBUG_PRINTLN(debugFileName);

  // Write CSV header if necessary.
  debugFile.println(
    "datetime,battery,online_microsd,online_gnss,online_log_gnss,online_log_debug,"
    "timer_battery,timer_microsd,timer_gnss,timer_sync_rtc,timer_log_gnss,timer_log_debug,"
    "rtc_sync_flag,rtc_drift,bytes_written,max_buffer_bytes,wdt_counter_max,"
    "write_fail_counter,sync_fail_counter,close_fail_counter,debug_counter");

  // Sync the debug file to ensure integrity.
  if (!debugFile.sync()) {
    DEBUG_PRINTLN(F("[Logging] Warning: Failed to sync debug file."));
  } else {
    DEBUG_PRINTLN(F("[Logging] Info: Synced debug file."));
  }

  // Update the file creation timestamp.
  updateFileCreate(&debugFile);

  // Close the debug file.
  if (!debugFile.close()) {
    DEBUG_PRINTLN(F("[Logging] Warning: Failed to close debug file."));
  } else {
    DEBUG_PRINTLN(F("[Logging] Info: Closed debug file."));
  }
}

// Log debugging information.
//
// Collects and writes system status, timers, and operational flags to the
// debug log file. This function provides insight into system performance
// and potential failure points.
void logDebug() {
  // Start loop timer for profiling.
  unsigned long loopStartTime = millis();

  // Increment debug counter.
  debugCounter++;

  // Check if debug file is open.
  if (debugFile.isOpen()) {
    debugFile.close();
    DEBUG_PRINTLN(F("[Logging] Info: Debug file closed before reopening."));
  } else {
    DEBUG_PRINTLN(F("[Logging] Debug: Debug file is closed."));
  }

  // Open debug log file.
  if (!debugFile.open(debugFileName, O_APPEND | O_WRITE)) {
    DEBUG_PRINT(F("[Logging] Warning: Failed to open debug file: "));
    DEBUG_PRINTLN(debugFileName);
    online.logDebug = false;  // Set flag
    return;
  } else {
    DEBUG_PRINT(F("[Logging] Info: Opened debug file: "));
    DEBUG_PRINTLN(debugFileName);
    online.logDebug = true;  // Set flag
  }

  // Create timestamp string.
  char dateTime[30];
  sprintf(dateTime, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  // Define an array of values to log.
  const int NUM_FIELDS = 20;
  long values[NUM_FIELDS] = {
    readBattery(),
    online.microSd,
    online.gnss,
    online.logGnss,
    online.logDebug,
    timer.voltage,
    timer.microSd,
    timer.gnss,
    timer.syncRtc,
    timer.logGnss,
    timer.logDebug,
    rtcSyncFlag,
    rtcDrift,
    bytesWritten,
    maxBufferBytes,
    wdtCounterMax,
    writeFailCounter,
    syncFailCounter,
    closeFailCounter,
    debugCounter
  };

  // Write timestamp first.
  debugFile.print(dateTime);
  DEBUG_PRINT(dateTime);

  // Print all values using a loop.
  for (int i = 0; i < NUM_FIELDS; i++) {
    debugFile.print(",");
    debugFile.print(values[i]);

    DEBUG_PRINT(",");
    DEBUG_PRINT(values[i]);
  }

  // Finish with a newline.
  debugFile.println();
  DEBUG_PRINTLN();

  // Sync the debug file to disk.
  if (!debugFile.sync()) {
    DEBUG_PRINTLN(F("[Logging] Warning: Failed to sync debug file."));
    syncFailCounter++;  // Track failed sync attempts
  } else {
    DEBUG_PRINTLN(F("[Logging] Info: Synced debug file."));
  }

  // Update file access timestamps.
  updateFileAccess(&debugFile);

  // Close the debug file.
  if (!debugFile.close()) {
    DEBUG_PRINTLN(F("[Logging] Warning: Failed to close debug file."));
    closeFailCounter++;  // Track failed close attempts
  } else {
    DEBUG_PRINTLN(F("[Logging] Info: Closed debug file."));
  }

  // Stop loop timer and store execution time.
  timer.logDebug = millis() - loopStartTime;
}