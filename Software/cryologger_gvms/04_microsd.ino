// Configure microSD
void configureSd()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Initialize microSD
  if (!sd.begin(PIN_SD_CS, SD_SCK_MHZ(24)))
  {
    DEBUG_PRINTLN("Warning: microSD failed to initialize. Reattempting...");

    // Delay between initialization attempts
    myDelay(2000);

    if (!sd.begin(PIN_SD_CS, SD_SCK_MHZ(24)))
    {
      DEBUG_PRINTLN("Warning: microSD failed to initialize.");
      online.microSd = false;
      while (1) 
      {
        // Force watchdog timer to reset system
        blinkLed(2, 250);
        delay(2000);
      }
    }
    else
    {
      DEBUG_PRINTLN("Info: microSD initialized.");
      online.microSd = true;
    }
  }
  else
  {
    DEBUG_PRINTLN("Info: microSD initialized.");
    online.microSd = true;
  }
  // Stop the loop timer
  timer.microSd = millis() - loopStartTime;
}

// Update the file create timestamp
void updateFileCreate(FsFile *dataFile)
{
  // Get the RTC's current date and time
  rtc.getTime();

  // Update the file create timestamp
  if (!dataFile->timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    DEBUG_PRINT("Warning: Could not update file create timestamp.");
  }
}

// Update the file access and write timestamps
void updateFileAccess(FsFile *dataFile)
{
  // Get the RTC's current date and time
  rtc.getTime();

  // Update the file access and write timestamps
  if (!dataFile->timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    DEBUG_PRINT("Warning: Could not update file access timestamp.");
  }
  if (!dataFile->timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds))
  {
    DEBUG_PRINT("Warning: Could not update file write timestamp.");
  }
}
