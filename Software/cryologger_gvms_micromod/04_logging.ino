// Configure microSD
void configureSd()
{
  if (sd.begin(SdSpiConfig(PIN_SD_CS, DEDICATED_SPI)))
  {
    online.microSd = true;
  }
  else
  {
    DEBUG_PRINTLN("Warning: microSD not detected! Please check wiring.");
    digitalWrite(LED_BUILTIN, HIGH);
    while (1);
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
    DEBUG_PRINTLN("Failed to create log file");
    return;
  }

  if (!debugFile.isOpen())
  {
    DEBUG_PRINTLN("Warning: Unable to open file");
  }

  // Write header to file
  debugFile.println("unixtime,timer_voltage,timer_rtc,timer_sd,timer_sensors,timer_gnss,online_microsd,online_gnss,watchdog");

  // Update file create timestamp
  updateFileCreate(&debugFile);

  // Sync the log file
  debugFile.sync();

  // Close log file
  debugFile.close();

  DEBUG_PRINT("Logging diagnostics to file: "); DEBUG_PRINTLN(debugFileName);
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
    DEBUG_PRINTLN("Failed to create log file");
    return;
  }

  if (!logFile.isOpen())
  {
    DEBUG_PRINTLN("Warning: Unable to open file");
  }

  // Update file create timestamp
  updateFileCreate(&logFile);

  // Sync the log file
  logFile.sync();

  // Close log file
  logFile.close();

  DEBUG_PRINT("Logging to file: "); DEBUG_PRINTLN(logFileName);
}

// Log data to microSD
void logDebugData()
{
  unsigned long loopStartTime = millis(); // Start loop timer

  // Open log file for writing
  // O_CREAT - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE - Open the file for writing
  if (debugFile.open(debugFileName, O_APPEND | O_WRITE))
  {
    debugFile.print(unixtime);        debugFile.print(",");
    debugFile.print(timer.voltage);   debugFile.print(",");
    debugFile.print(timer.rtc);       debugFile.print(",");
    debugFile.print(timer.microSd);   debugFile.print(",");
    debugFile.print(timer.sensors);   debugFile.print(",");
    debugFile.print(timer.logGnss);   debugFile.print(",");
    debugFile.print(online.microSd);  debugFile.print(",");
    debugFile.print(online.gnss);     debugFile.print(",");
    debugFile.println(watchdogCounter);
    
    updateFileAccess(&debugFile); // Update file access and write timestamps
  }
  else
  {
    DEBUG_PRINTLN("Warning: Unable to open file!");
  }

  // Sync log file
  if (!debugFile.sync())
  {
    DEBUG_PRINTLN("Warning: File sync error!");
  }

  // Check for write error
  if (debugFile.getWriteError())
  {
    DEBUG_PRINTLN("Warning: File write error!");
  }

  // Close log file
  if (!debugFile.close())
  {
    DEBUG_PRINTLN("Warning: File close error!");
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
