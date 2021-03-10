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
  }
}

// Create log file
void createLogFile()
{
  // Check if file is open and close it
  if (file.isOpen())
  {
    file.close();
  }

  // Get the RTC's current date and time
  rtc.getTime();

  // Create timestamped log file name
  sprintf(fileName, "20%02d%02d%02d_%02d%02d%02d.ubx",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  // Open log file for writing
  // O_CREAT - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE - Open the file for writing
  if (!file.open(fileName, O_CREAT | O_APPEND | O_WRITE))
  {
    DEBUG_PRINTLN("Failed to create log file");
    return;
  }

  if (!file.isOpen())
  {
    DEBUG_PRINTLN("Warning: Unable to open file");
  }

  // Update file create timestamp
  updateFileCreate();

  // Sync the log file
  file.sync();

  // Close log file
  file.close();

  DEBUG_PRINT("Logging to file: "); DEBUG_PRINTLN(fileName);
}

// Log data to microSD
void logData()
{
  unsigned long loopStartTime = millis(); // Start loop timer

  // Open log file and append data
  if (file.open(fileName, O_APPEND | O_WRITE))
  {
    file.print(moMessage.unixtime);             file.print(",");
    file.print(moMessage.temperature / 100.0);  file.print(",");
    file.print(moMessage.humidity / 100.0);     file.print(",");
    file.print(moMessage.pressure / 100.0);     file.print(",");
    file.print(moMessage.latitude);             file.print(",");
    file.print(moMessage.longitude);            file.print(",");
    file.print(moMessage.satellites);           file.print(",");
    file.print(moMessage.pdop);                 file.print(",");
    file.print(moMessage.rtcDrift);             file.print(",");
    file.print(moMessage.voltage);              file.print(",");
    file.print(moMessage.transmitDuration);     file.print(",");
    file.print(messageCounter);                 file.print(",");
    file.print(timer.voltage);                  file.print(",");
    file.print(timer.rtc);                      file.print(",");
    file.print(timer.microSd);                  file.print(",");
    file.print(timer.sensor);                   file.print(",");
    file.print(timer.gnss);                     file.print(",");
    file.print(timer.iridium);                  file.print(",");
    file.print(online.microSd);                 file.print(",");
    file.print(online.gnss);                    file.print(",");
    file.println(online.iridium);

    updateFileAccess(); // Update file access and write timestamps
  }
  else
  {
    DEBUG_PRINTLN("Warning: Unable to open file!");
  }

  // Sync log file
  if (!file.sync())
  {
    DEBUG_PRINTLN("Warning: File sync error!");
  }

  // Check for write error
  if (file.getWriteError())
  {
    DEBUG_PRINTLN("Warning: File write error!");
  }

  // Close log file
  if (!file.close())
  {
    DEBUG_PRINTLN("Warning: File close error!");
  }
  
  // Stop the loop timer
  timer.microSd = millis() - loopStartTime;

  // Blink LED
  blinkLed(2, 100);
}

// Update the file create timestamp
void updateFileCreate()
{
  // Get the RTC's current date and time
  rtc.getTime();
  
  // Update the file create timestamp
  if (!file.timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    DEBUG_PRINTLN("Warning: Unable to write file create timestamp");
  }
}

// Update the file access and write timestamps
void updateFileAccess()
{
  // Get the RTC's current date and time
  rtc.getTime();
  
  // Update the file access and write timestamps
  if (!file.timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    DEBUG_PRINTLN("Warning: Unable to write file access timestamp");
  }
  if (!file.timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    DEBUG_PRINTLN("Warning: Unable to write file write timestamp");
  }
}
