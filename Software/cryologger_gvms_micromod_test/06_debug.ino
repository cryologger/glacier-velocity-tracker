// Create debugging log file
void createDebugFile()
{
  // Create debug log file
  // O_CREAT - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE - Open the file for writing
  if (!debugFile.open(debugFileName, O_CREAT | O_APPEND | O_WRITE))
  {
    Serial.println("Warning: Failed to create debug file.");
    return;
  }
  else
  {
    Serial.print("Info: Created debug file "); Serial.println(debugFileName);
  }

  // Write header to file
  debugFile.println("datetime,timer_voltage,timer_rtc,timer_microsd,timer_sensors,timer_gnss,"
                    "bytes_written,max_buffer,online_microsd,online_gnss,online_dataLogging,online_debugLogging,rtc_drift,watchdog,sample");

  // Sync the debug file
  if (!debugFile.sync())
  {
    Serial.println("Warning: Failed to sync debug file.");
  }

  // Update the file create timestamp
  if (!debugFile.timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    Serial.print("Warning: Could not update file create timestamp.");
  }

  // Close log file
  if (!debugFile.close())
  {
    Serial.println("Warning: Failed to close debug file.");
  }
}


// Log debugging information
void logDebug()
{
  char dateTime[30];
  sprintf(dateTime, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  // Open debug file for writing
  // O_CREAT - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE - Open the file for writing
  if (!debugFile.open(debugFileName, O_APPEND | O_WRITE))
  {
    Serial.println("Warning: Failed to open debug file.");
    return;
  }

  // Log debugging information
  debugFile.print(dateTime);            debugFile.print(",");
  debugFile.print(bytesWritten);        debugFile.print(",");
  debugFile.println(maxBufferBytes);

  // Sync the debug file
  if (!debugFile.sync())
  {
    Serial.println("Warning: Failed to sync debug file.");
  }

  // Update the file access timestamp
  if (!debugFile.timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    Serial.print("Warning: Could not update debug file access timestamp.");
  }

  // Update the file write timestamp
  if (!debugFile.timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    Serial.print("Warning: Could not update debug file write timestamp.");
  }

  // Close the debug file
  if (!debugFile.close())
  {
    Serial.println("Warning: Failed to close debug file.");
  }

}
