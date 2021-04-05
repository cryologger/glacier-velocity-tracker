// Create debugging log file
void createDebugFile()
{
  // Create debug log file
  // O_CREAT - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE - Open the file for writing
  if (!debugFile.open(debugFileName, O_CREAT | O_APPEND | O_WRITE))
  {
    DEBUG_PRINTLN("Warning: Failed to create debug file.");
    return;
  }
  else
  {
    DEBUG_PRINT("Info: Created "); DEBUG_PRINTLN(debugFileName);
  }

  // Write header to file
  debugFile.println("datetime,online_microSd,online_gnss,online_logGnss,online_logDebug,"
                    "timer_microsd,timer_gnss,timer_syncRtc,timer_logGnss,timer_logDebug,"
                    "rtcSyncFlag,rtcDrift,bytesWritten,maxBufferBytes,wdtCounterMax,"
                    "writeFailCounter,syncFailCounter,closeFailCounter,debugCounter");

  // Sync the debug file
  if (!debugFile.sync())
  {
    DEBUG_PRINTLN("Warning: Failed to sync debug file.");
  }

  // Update the file create timestamp
  updateFileCreate(&debugFile);

  // Close log file
  if (!debugFile.close())
  {
    DEBUG_PRINTLN("Warning: Failed to close debug file.");
  }
}

// Log debugging information
void logDebug()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Increment debug counter
  debugCounter++;

  // Open debug file for writing
  if (!debugFile.open(debugFileName, O_APPEND | O_WRITE))
  {
    DEBUG_PRINTLN("Warning: Failed to open debug file.");
    online.logDebug = false; // Set flag
    return;
  }
  else
  {
    DEBUG_PRINT("Info: Opened "); DEBUG_PRINTLN(debugFileName);
    online.logDebug = true; // Set flag
  }

  // Create datetime string
  char dateTime[30];
  sprintf(dateTime, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  // Log debugging information
  debugFile.print(dateTime);            debugFile.print(",");
  debugFile.print(online.microSd);      debugFile.print(",");
  debugFile.print(online.gnss);         debugFile.print(",");
  debugFile.print(online.logGnss);      debugFile.print(",");
  debugFile.print(online.logDebug);     debugFile.print(",");
  debugFile.print(timer.microSd);       debugFile.print(",");
  debugFile.print(timer.gnss);          debugFile.print(",");
  debugFile.print(timer.syncRtc);       debugFile.print(",");
  debugFile.print(timer.logGnss);       debugFile.print(",");
  debugFile.print(timer.logDebug);      debugFile.print(",");
  debugFile.print(rtcSyncFlag);         debugFile.print(",");
  debugFile.print(rtcDrift);            debugFile.print(",");
  debugFile.print(bytesWritten);        debugFile.print(",");
  debugFile.print(maxBufferBytes);      debugFile.print(",");
  debugFile.print(wdtCounterMax);       debugFile.print(",");
  debugFile.print(writeFailCounter);    debugFile.print(",");
  debugFile.print(syncFailCounter);     debugFile.print(",");
  debugFile.print(closeFailCounter);    debugFile.print(",");
  debugFile.println(debugCounter);

  // Sync the debug file
  if (!debugFile.sync())
  {
    DEBUG_PRINTLN("Warning: Failed to sync debug file.");
    syncFailCounter++; // Count number of failed file syncs
  }

  // Update file access timestamps
  updateFileAccess(&debugFile);

  // Close the debug file
  if (!debugFile.close())
  {
    DEBUG_PRINTLN("Warning: Failed to close debug file.");
    closeFailCounter++; // Count number of failed file closes
  }
  // Stop the loop timer
  timer.logDebug = millis() - loopStartTime;
}
