// Configure microSD
void configureSd()
{
  // Initialize microSD
  if (!sd.begin(SdSpiConfig(PIN_SD_CS, DEDICATED_SPI)))
  {
    DEBUG_PRINTLN("Warning: microSD initialization failed. Reattempting...");

    // Non-blocking delay
    unsigned long currentMillis = millis();
    while (millis() - currentMillis <= sdPowerDelay)
    {
      petDog(); // Restart watchdog timer
    }
    // Reattempt initialization
    if (!sd.begin(SdSpiConfig(PIN_SD_CS, DEDICATED_SPI)))
    {
      DEBUG_PRINTLN("Warning: microSD initialization failed. Please check wiring.");
      online.microSd = false;
      peripheralPowerOff();
      wdt.stop();
      while (1)
      {
        blinkLed(2, 250);
        blinkLed(1, 1000);
      }
      //return;
    }
  }
  else
  {
    online.microSd = true; // Set flag
    DEBUG_PRINTLN("Info: microSD initalized.");
  }
}

// Create debugging log file
void createDebugFile()
{
  // Check if microSD initialized successfully
  if (online.microSd)
  {
    // Create debug file
    // O_CREAT - Create the file if it does not exist
    // O_APPEND - Seek to the end of the file prior to each write
    // O_WRITE - Open the file for writing
    if (!debugFile.open(debugFileName, O_CREAT | O_APPEND | O_WRITE))
    {
      online.debugLogging = false; // Clear flag
      DEBUG_PRINT("Warning: Failed to create "); DEBUG_PRINTLN(debugFileName);
      return;
    }
    else
    {
      online.debugLogging = true; // Set flag
      DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" created.");
    }

    // Write header to file
    debugFile.println("datetime,unixtime,timer_voltage,timer_rtc,timer_microsd,timer_sensors,timer_gnss,"
                      "bytes_written,max_buffer,online_microsd,online_gnss,online_dataLogging,online_debugLogging,rtc_drift,watchdog,sample");

    // Sync the log file
    if (!debugFile.sync())
    {
      DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" sync error.");
    }
    else
    {
      DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" synced.");
    }

    // Update file create timestamp
    updateFileCreate(&debugFile);

    // Close log file
    if (!debugFile.close())
    {
      DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" failed to close.");
    }
    else
    {
      DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" closed.");
    }

    DEBUG_PRINT("Info: Logging to "); DEBUG_PRINTLN(debugFileName);
  }
  else
  {
    DEBUG_PRINT("Warning: microSD offline.");
  }
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
    online.debugLogging = false; // Clear flag
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" failed to open.");
    return;
  }
  else
  {
    online.debugLogging = true; // Set flag
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
  debugFile.print(online.dataLogging);  debugFile.print(",");
  debugFile.print(online.debugLogging); debugFile.print(",");
  debugFile.print(rtcDrift);            debugFile.print(",");
  debugFile.print(watchdogCounterMax);  debugFile.print(",");
  debugFile.println(debugCounter);

  // Sync log file
  if (!debugFile.sync())
  {
    DEBUG_PRINT("Warning: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" sync error!");
  }
  else
  {
    DEBUG_PRINT("Info: "); DEBUG_PRINT(debugFileName); DEBUG_PRINTLN(" synced.");
  }

  // Update file access and write timestamps
  updateFileAccess(&debugFile);

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

  // Print debugging information
  printDebug();

  // Delay to allow previous log file to close
  blinkLed(2, 500);
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
