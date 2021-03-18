// Configure microSD
void configureSd()
{
  if (sd.begin(SdSpiConfig(PIN_SD_CS, DEDICATED_SPI)))
  {
    DEBUG_PRINTLN("Info: microSD initalized.");
    online.microSd = true;
  }
  else
  {
    DEBUG_PRINTLN("Warning: microSD not detected! Please check wiring.");
    DEBUG_PRINTLN("Warning: Non-recoverable error due to microSD initialization.");
    while (1)
    {
      wdt.stop();
      blinkLed(2, 250);
      blinkLed(1, 1000);
    }
  }
}

// Create log file
void createDebugLogFile()
{
  // Open log file for writing
  // O_CREAT - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE - Open the file for writing
  if (!debugFile.open(debugFileName, O_CREAT | O_APPEND | O_WRITE))
  {
    DEBUG_PRINT("Warning: Failed to create "); DEBUG_PRINTLN(debugFileName);
    return;
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" created.");
  }

  // Write header to file
  debugFile.println("datetime,unixtime,timer_voltage,timer_rtc,timer_sd,timer_sensors,timer_gnss,bytes_written,max_buffer,online_microsd,online_gnss,rtc_drift,watchdog,sample");

  // Update file create timestamp
  updateFileCreate(&debugFile);

  // Sync the log file
  if (!debugFile.sync())
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" sync error.");
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" synced.");
  }

  // Close log file
  if (!debugFile.close())
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" failed to close.");
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" closed.");
  }

  DEBUG_PRINT("Info: Debug logging to "); DEBUG_PRINTLN(debugFileName);
}

// Create log file
void createLogFile()
{
  // Check if file is open and close it
  if (logFile.isOpen())
  {
    logFile.close();
  }

  // Get the RTC's current date and time
  rtc.getTime();

  // Create timestamped log file name
  sprintf(logFileName, "20%02d%02d%02d_%02d%02d%02d.ubx",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  // Open log file for writing
  // O_CREAT - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE - Open the file for writing
  if (!logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE))
  {
    DEBUG_PRINT("Warning: Failed to create "); DEBUG_PRINTLN(logFileName);
    return;
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" created.");
  }

  // Update file create timestamp
  updateFileCreate(&logFile);

  // Sync the log file
  if (!logFile.sync())
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" sync error.");
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" synced.");
  }

  // Close log file
  if (!logFile.close())
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" failed to close.");
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" closed.");
  }

  DEBUG_PRINT("Info: Logging data to "); DEBUG_PRINTLN(logFileName);
}

// Log data to microSD
void logDebugData()
{
  unsigned long loopStartTime = millis(); // Start loop timer

  debugCounter++; // Increment debug counter

  char dateTime[30];
  sprintf(dateTime, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  // Open log file for writing
  // O_CREAT - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE - Open the file for writing
  if (!debugFile.open(debugFileName, O_APPEND | O_WRITE))
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" failed to open.");
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" opened.");
  }

  // Log debugging information
  debugFile.print(dateTime);            debugFile.print(",");
  debugFile.print(unixtime);            debugFile.print(",");
  debugFile.print(timer.voltage);       debugFile.print(",");
  debugFile.print(timer.rtc);           debugFile.print(",");
  debugFile.print(timer.microSd);       debugFile.print(",");
  debugFile.print(timer.sensors);       debugFile.print(",");
  debugFile.print(timer.logGnss);       debugFile.print(",");
  debugFile.print(bytesWritten);        debugFile.print(",");
  debugFile.print(maxBufferBytes);      debugFile.print(",");
  debugFile.print(online.microSd);      debugFile.print(",");
  debugFile.print(online.gnss);         debugFile.print(",");
  debugFile.print(rtcDrift);            debugFile.print(",");
  debugFile.print(watchdogCounterMax);  debugFile.print(",");
  debugFile.println(debugCounter);

  // Update file access and write timestamps
  updateFileAccess(&debugFile);

  // Sync log file
  if (!debugFile.sync())
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" sync error!");
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" synced.");
  }

  // Check for write error
  if (debugFile.getWriteError())
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" write error!");
  }

  // Close log file
  if (!debugFile.close())
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" failed to close!");
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" closed.");
  }

  // Stop the loop timer
  timer.microSd = millis() - loopStartTime;

  // Blink LED
  blinkLed(2, 100);
}

// Update the file create timestamp
void updateFileCreate(FsFile *dataFile)
{
  // Get the RTC's current date and time
  rtc.getTime();

  // Update the file create timestamp
  dataFile->timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
}

// Update the file access and write timestamps
void updateFileAccess(FsFile *dataFile)
{
  // Get the RTC's current date and time
  rtc.getTime();

  // Update the file access and write timestamps
  dataFile->timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
  dataFile->timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
}
