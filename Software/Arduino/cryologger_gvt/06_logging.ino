/*
  Logging Module

  This module handles the creation and management of log files. It generates
  timestamped filenames, logs debugging information, and ensures data integrity
  by syncing and closing files properly.

  Log Files:
  - UBX Log: Stores raw GNSS data with a timestamped filename.
  - Debug Log: Stores system status, timers, and errors in CSV format.
*/

// ----------------------------------------------------------------------------
// Create a timestamped log file name.
// Generates a filename using the current RTC timestamp. This ensures that
// each log session is uniquely named and avoids overwriting previous logs.
// ----------------------------------------------------------------------------
void getLogFileName() {
  snprintf(logFileName, sizeof(logFileName),
           "%s_20%02lu%02lu%02lu_%02lu%02lu%02lu.ubx",
           uid, rtc.year, rtc.month, rtc.dayOfMonth,
           rtc.hour, rtc.minute, rtc.seconds);

  DEBUG_PRINT("[Logging] Info: logFileName = ");
  DEBUG_PRINTLN(logFileName);
}

// ----------------------------------------------------------------------------
// Create a debugging log file.
// Initializes the debug log file and writes the CSV header if the file is
// newly created. Ensures that debug logs persist across sessions.
// ----------------------------------------------------------------------------
void createDebugFile() {
  // Generate debug log filename.
  snprintf(debugFileName, sizeof(debugFileName), "%s_debug.csv", uid);

  // Open or create the debug log file.
  // O_CREAT - Creates the file if it does not exist.
  // O_APPEND - Seeks to the end of the file prior to each write.
  // O_WRITE - Open the file for writing.
  if (!debugFile.open(debugFileName, O_CREAT | O_APPEND | O_WRITE)) {
    DEBUG_PRINTLN("[Logging] Warning: Failed to create debug file.");
    return;
  }
  DEBUG_PRINT("[Logging] Info: Created or opened debug file: ");
  DEBUG_PRINTLN(debugFileName);

  // Write CSV header if necessary.
  if (debugFile.size() == 0) {
    debugFile.println(
      "datetime,battery,temperature,online_microsd,online_gnss,online_log_gnss,online_log_debug,"
      "timer_battery,timer_microsd,timer_gnss,timer_sync_rtc,timer_log_gnss,timer_log_debug,"
      "rtc_sync_flag,rtc_drift,bytes_written,max_buffer_bytes,wdt_counter_max,"
      "write_fail_counter,sync_fail_counter,close_fail_counter,debug_counter");
    DEBUG_PRINT("[Logging] Info: Header written to ");
    DEBUG_PRINTLN(debugFileName);
  } else {
    DEBUG_PRINT("[Logging] Info: Header already exists in ");
    DEBUG_PRINTLN(debugFileName);
  }
  // Sync the debug file to ensure integrity.
  if (!debugFile.sync()) {
    DEBUG_PRINTLN("[Logging] Warning: Failed to sync debug file.");
  } else {
    DEBUG_PRINTLN("[Logging] Info: Synced debug file.");
  }

  // Update the file creation timestamp.
  updateFileCreate(&debugFile);

  // Close the debug file.
  if (!debugFile.close()) {
    DEBUG_PRINTLN("[Logging] Warning: Failed to close debug file.");
  } else {
    DEBUG_PRINTLN("[Logging] Info: Closed debug file.");
  }
}

// ----------------------------------------------------------------------------
// Write a single debug record to the given output stream.
// Used for both the debug log file and Serial.
// ----------------------------------------------------------------------------
void printDebugRecord(Print &out, const char *dateTime, float battery, float temperature) {
  out.print(dateTime);
  out.print(",");
  out.print(battery);
  out.print(",");
  out.print(temperature);
  out.print(",");
  out.print(online.microSd);
  out.print(",");
  out.print(online.gnss);
  out.print(",");
  out.print(online.logGnss);
  out.print(",");
  out.print(online.logDebug);
  out.print(",");
  out.print(timer.voltage);
  out.print(",");
  out.print(timer.microSd);
  out.print(",");
  out.print(timer.gnss);
  out.print(",");
  out.print(timer.syncRtc);
  out.print(",");
  out.print(timer.logGnss);
  out.print(",");
  out.print(timer.logDebug);
  out.print(",");
  out.print(rtcSyncFlag);
  out.print(",");
  out.print(rtcDrift);
  out.print(",");
  out.print(bytesWritten);
  out.print(",");
  out.print(maxBufferBytes);
  out.print(",");
  out.print(wdtCounterMax);
  out.print(",");
  out.print(writeFailCounter);
  out.print(",");
  out.print(syncFailCounter);
  out.print(",");
  out.print(closeFailCounter);
  out.print(",");
  out.println(debugCounter);
}

// ----------------------------------------------------------------------------
// Log debugging information.
// Collects and writes system status, timers, and operational flags to the
// debug log file. This function provides insight into system performance
// and potential failure points.
// ----------------------------------------------------------------------------
void logDebug() {
  // Start loop timer for profiling.
  unsigned long loopStartTime = millis();

  // Increment debug counter.
  debugCounter++;

  // Check if debug file is open.
  if (debugFile.isOpen()) {
    debugFile.close();
    DEBUG_PRINTLN("[Logging] Info: Debug file closed before reopening.");
  } else {
    DEBUG_PRINTLN("[Logging] Debug: Debug file is already closed.");
  }

  // Open debug log file.
  if (!debugFile.open(debugFileName, O_APPEND | O_WRITE)) {
    DEBUG_PRINT("[Logging] Warning: Failed to open debug file: ");
    DEBUG_PRINTLN(debugFileName);
    online.logDebug = false;                    // Set flag
    timer.logDebug = millis() - loopStartTime;  // Stop loop timer
    return;
  }

  online.logDebug = true;  // Set flag
  DEBUG_PRINT("[Logging] Info: Opened debug file: ");
  DEBUG_PRINTLN(debugFileName);

  // Create timestamp string.
  char dateTime[30];
  snprintf(dateTime, sizeof(dateTime),
           "20%02lu-%02lu-%02lu %02lu:%02lu:%02lu",
           rtc.year, rtc.month, rtc.dayOfMonth,
           rtc.hour, rtc.minute, rtc.seconds);

  // Read sensors once so the file and Serial records match
  float battery = readBattery();
  float temperature = readInternalTemp();

  // Stop loop timer before writing so the record carries the current value
  timer.logDebug = millis() - loopStartTime;

  // Write the debug record to the log file
  printDebugRecord(debugFile, dateTime, battery, temperature);

  // Sync the debug file to disk.
  if (!debugFile.sync()) {
    DEBUG_PRINTLN("[Logging] Warning: Failed to sync debug file.");
    syncFailCounter++;  // Track failed sync attempts
  } else {
    DEBUG_PRINTLN("[Logging] Info: Synced debug file.");
  }

  // Update file access timestamps.
  updateFileAccess(&debugFile);

  // Close the debug file.
  if (!debugFile.close()) {
    DEBUG_PRINTLN("[Logging] Warning: Failed to close debug file.");
    closeFailCounter++;  // Track failed close attempts
  } else {
    DEBUG_PRINTLN("[Logging] Info: Closed debug file.");
  }

#if DEBUG
  // Write the same debug record to Serial
  DEBUG_PRINT("[Logging] Info: ");
  printDebugRecord(Serial, dateTime, battery, temperature);
#endif
}
