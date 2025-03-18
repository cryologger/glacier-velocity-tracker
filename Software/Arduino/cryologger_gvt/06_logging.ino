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
  sprintf(debugFileName, "%s_debug.csv", uid);

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
      "datetime,battery,online_microsd,online_gnss,online_log_gnss,online_log_debug,"
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
    online.logDebug = false;  // Set flag
    return;
  } else {
    DEBUG_PRINT("[Logging] Info: Opened debug file: ");
    DEBUG_PRINTLN(debugFileName);
    online.logDebug = true;  // Set flag
  }

  // Create timestamp string.
  char dateTime[30];
  sprintf(dateTime, "20%02lu-%02lu-%02lu %02lu:%02lu:%02lu",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  // Log debugging information
  debugFile.print(dateTime);
  debugFile.print(",");
  debugFile.print(readBattery());
  debugFile.print(",");
  debugFile.print(online.microSd);
  debugFile.print(",");
  debugFile.print(online.gnss);
  debugFile.print(",");
  debugFile.print(online.logGnss);
  debugFile.print(",");
  debugFile.print(online.logDebug);
  debugFile.print(",");
  debugFile.print(timer.voltage);
  debugFile.print(",");
  debugFile.print(timer.microSd);
  debugFile.print(",");
  debugFile.print(timer.gnss);
  debugFile.print(",");
  debugFile.print(timer.syncRtc);
  debugFile.print(",");
  debugFile.print(timer.logGnss);
  debugFile.print(",");
  debugFile.print(timer.logDebug);
  debugFile.print(",");
  debugFile.print(rtcSyncFlag);
  debugFile.print(",");
  debugFile.print(rtcDrift);
  debugFile.print(",");
  debugFile.print(bytesWritten);
  debugFile.print(",");
  debugFile.print(maxBufferBytes);
  debugFile.print(",");
  debugFile.print(wdtCounterMax);
  debugFile.print(",");
  debugFile.print(writeFailCounter);
  debugFile.print(",");
  debugFile.print(syncFailCounter);
  debugFile.print(",");
  debugFile.print(closeFailCounter);
  debugFile.print(",");
  debugFile.println(debugCounter);

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

  // Print debugging information
  DEBUG_PRINT("[Logging] Info: ");
  DEBUG_PRINT(dateTime);
  DEBUG_PRINT(",");
  DEBUG_PRINT(readBattery());
  DEBUG_PRINT(",");
  DEBUG_PRINT(online.microSd);
  DEBUG_PRINT(",");
  DEBUG_PRINT(online.gnss);
  DEBUG_PRINT(",");
  DEBUG_PRINT(online.logGnss);
  DEBUG_PRINT(",");
  DEBUG_PRINT(online.logDebug);
  DEBUG_PRINT(",");
  DEBUG_PRINT(timer.voltage);
  DEBUG_PRINT(",");
  DEBUG_PRINT(timer.microSd);
  DEBUG_PRINT(",");
  DEBUG_PRINT(timer.gnss);
  DEBUG_PRINT(",");
  DEBUG_PRINT(timer.syncRtc);
  DEBUG_PRINT(",");
  DEBUG_PRINT(timer.logGnss);
  DEBUG_PRINT(",");
  DEBUG_PRINT(timer.logDebug);
  DEBUG_PRINT(",");
  DEBUG_PRINT(rtcSyncFlag);
  DEBUG_PRINT(",");
  DEBUG_PRINT(rtcDrift);
  DEBUG_PRINT(",");
  DEBUG_PRINT(bytesWritten);
  DEBUG_PRINT(",");
  DEBUG_PRINT(maxBufferBytes);
  DEBUG_PRINT(",");
  DEBUG_PRINT(wdtCounterMax);
  DEBUG_PRINT(",");
  DEBUG_PRINT(writeFailCounter);
  DEBUG_PRINT(",");
  DEBUG_PRINT(syncFailCounter);
  DEBUG_PRINT(",");
  DEBUG_PRINT(closeFailCounter);
  DEBUG_PRINT(",");
  DEBUG_PRINTLN(debugCounter);

  // Stop loop timer and store execution time.
  timer.logDebug = millis() - loopStartTime;
}
