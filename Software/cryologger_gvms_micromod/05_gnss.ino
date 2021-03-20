// Configure u-blox module
void configureGnss()
{
  // Uncomment to enable GNSS debug messages on Serial
  //gnss.enableDebugging();

  // Allocate sufficient RAM to store RAWX messages (>2 KB)
  gnss.setFileBufferSize(fileBufferSize); // Must be called before gnss.begin()

  DEBUG_PRINTLN("Info: Opening Serial1 port at 230400 baud.");

  // Open Serial1 port and set data rate to 230400 baud
  Serial1.begin(230400);

  DEBUG_PRINTLN("Info: Attempting to initalize u-blox module.");

  // Initialize u-blox module
  if (gnss.begin(Serial1))
  {
    DEBUG_PRINTLN("Info: u-blox module initialized at 230400 bps.");
    online.gnss = true;
  }
  else
  {
    DEBUG_PRINTLN("Warning: u-blox module not detected at 230400 bps. Attempting 38400 bps...");

    // Change Serial1 data rate to 38400 baud (u-blox default)
    Serial1.begin(460800);

    // Initlialze u-blox module
    if (gnss.begin(Serial1))
    {
      DEBUG_PRINTLN("Info: u-blox module initialized at 38400 bps.");

      // Change UART1 baud rate to 230400 bps
      bool response = true;
      response &= gnss.setVal32(UBLOX_CFG_UART1_BAUDRATE, 230400);

      DEBUG_PRINTLN("Info: UART1 baud rate set to 230400 bps.");

      // Open Serial1 port and set data rate to 230400 baud
      Serial1.begin(230400);

      // Attempt to initialize u-blox module
      if (gnss.begin(Serial1))
      {
        DEBUG_PRINTLN("Info: u-blox module initialized at 230400 bps.");
        online.gnss = true;
      }
      else
      {
        DEBUG_PRINTLN("Warning: u-blox module not detected at 230400 bps. Please check wiring or baud rate.");
        online.gnss = false;
      }
    }
    else
    {
      DEBUG_PRINTLN("Warning: u-blox module not detected at 38400 bps. Please check wiring or baud rate.");
      online.gnss = false;
    }
  }

  // If u-blox module is still offline, there is a communication issue
  if (!online.gnss)
  {
    DEBUG_PRINTLN("Warning: Non-recoverable error due to u-blox module initialization failure.");
    peripheralPowerOff(); // Disable power to microSD and u-blox
    while (1)
    {
      wdt.stop(); // Stop watchdog timer
      blinkLed(2, 250);
      blinkLed(2, 1000);
    }
  }

  // Uncomment to reset u-blox module to default factory settings
  //gnss.factoryDefault();
  //delay(5000);

  // Check if u-blox module configuration is required
  if (!gnssConfigFlag)
  {
    // Configure u-blox module
    gnss.setUART1Output(COM_TYPE_UBX);        // Set the UART1 port to output UBX only (disable NMEA)
    gnss.setNavigationFrequency(1);           // Produce one navigation solution per second
    gnss.setAutoPVTcallback(&processNavPvt);  // Enable automatic NAV PVT messages with callback to processNavPvt()
    gnss.setAutoRXMRAWX(true);                // Enable automatic RXM RAWX reports at the navigation frequency
    gnss.setAutoRXMSFRBX(true);               // Enable automatic RXM SFRBX reports at the navigation frequency
    gnss.saveConfiguration();                 // Save current settings to flash and BBR

    // Configure satellite signals
    bool setValueSuccess = true;
    setValueSuccess &= gnss.newCfgValset8(UBLOX_CFG_SIGNAL_GPS_ENA, 1);   // Enable GPS
    setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_GLO_ENA, 1);   // Enable GLONASS
    setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_GAL_ENA, 0);   // Disable Galileo
    setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_BDS_ENA, 0);   // Disable BeiDou
    setValueSuccess &= gnss.sendCfgValset8(UBLOX_CFG_SIGNAL_QZSS_ENA, 0); // Disable QZSS
    if (!setValueSuccess)
    {
      DEBUG_PRINTLN("Warning: Satellite signals not configured!");
    }
    else
    {
      DEBUG_PRINTLN("Info: Satellite signals configured.");
    }
    gnssConfigFlag = true; // Set flag

    // Print current GNSS configuration settings
    printGnssSettings();
  }
}

// Acquire a valid GNSS fix and sync the RTC
void syncRtc()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Clear flag
  rtcSyncFlag = false;

  // Check if GNSS initialized successfully
  if (online.gnss)
  {
    DEBUG_PRINTLN("Info: Acquiring GNSS fix...");

    // Attempt to acquire a valid GNSS position fix
    while (!rtcSyncFlag && millis() - loopStartTime < gnssTimeout * 60UL * 1000UL)
    {
      petDog(); // Reset watchdog timer
      gnss.checkUblox(); // Check for arrival of new data and process it
      gnss.checkCallbacks(); // Check if callbacks are waiting to be processed
    }
    if (!rtcSyncFlag)
    {
      DEBUG_PRINTLN("Warning: Unable to sync RTC!");
    }
  }
  else
  {
    DEBUG_PRINTLN("Warning: u-blox module offline!");
  }

  // Stop the loop timer
  timer.gnss = millis() - loopStartTime;
}

// Callback function to process UBX-NAV-PVT data
void processNavPvt(UBX_NAV_PVT_data_t ubx)
{
  // Reset watchdog timer
  petDog();

  // Blink LED
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  bool dateValidFlag = ubx.flags2.bits.confirmedDate;
  bool timeValidFlag = ubx.flags2.bits.confirmedTime;
  byte fixType = ubx.fixType;

#if DEBUG_GNSS
  sprintf(gnssBuffer, "%04u-%02d-%02d %02d:%02d:%02d.%03d,%ld,%ld,%d,%d,%d,%d,%d",
          ubx.year, ubx.month, ubx.day,
          ubx.hour, ubx.min, ubx.sec, ubx.iTOW % 1000,
          ubx.lat, ubx.lon, ubx.numSV,
          ubx.pDOP, ubx.fixType,
          dateValidFlag, timeValidFlag);
  DEBUG_PRINTLN(gnssBuffer);
#endif

  // Sync RTC with GNSS when date and time are valid
  if (dateValidFlag && timeValidFlag)
  {
    DEBUG_PRINTLN("Info: A GNSS fix was found.");
    gnssFixFlag = true; // Set fix flag

    // Convert GNSS date and time to Unix Epoch time
    tmElements_t tm;
    tm.Year = ubx.year - 1970;
    tm.Month = ubx.month;
    tm.Day = ubx.day;
    tm.Hour = ubx.hour;
    tm.Minute = ubx.min;
    tm.Second = ubx.sec;
    time_t gnssEpoch = makeTime(tm);
    rtc.getTime(); // Get the RTC's date and time

    // Calculate drift (to the second)
    rtcDrift = rtc.getEpoch() - gnssEpoch;

    DEBUG_PRINT("Info: RTC drift of "); DEBUG_PRINT(rtcDrift); DEBUG_PRINTLN(" seconds.");

    // Set RTC date and time
    rtc.setTime(ubx.hour, ubx.min, ubx.sec, ubx.iTOW % 1000,
                ubx.day, ubx.month, ubx.year - 2000);

    rtcSyncFlag = true; // Set flag
    DEBUG_PRINT("Info: RTC time synced to "); printDateTime();
  }
}

// Log UBX-RXM-RAWX/SFRBX data
void logData()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Check if microSD and u-blox module intialized successfully
  if (online.microSd && online.gnss)
  {
    // Enable UBX-RXM-RAWX/SFRBX data logging
    gnss.logRXMRAWX();
    gnss.logRXMSFRBX();

    // Reset bytesWritten counter
    bytesWritten = 0;

    // Create timestamped log file name
    sprintf(logFileName, "20%02d%02d%02d_%02d%02d%02d.ubx",
            rtc.year, rtc.month, rtc.dayOfMonth,
            rtc.hour, rtc.minute, rtc.seconds);

    // Create and open a new log file
    // O_CREAT  - Create the file if it does not exist
    // O_APPEND - Seek to the end of the file prior to each write
    // O_WRITE  - Open the file for writing
    if (!logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE))
    {
      online.dataLogging = false; // Clear flag
      DEBUG_PRINT("Warning: Failed to create log file"); DEBUG_PRINTLN(logFileName);
      return;
    }
    else
    {
      online.dataLogging = true; // Set flag
      DEBUG_PRINT("Info: Created log file "); DEBUG_PRINTLN(logFileName);
    }

    // Update file create timestamp
    updateFileCreate(&logFile);

    DEBUG_PRINT("Info: Logging data to "); DEBUG_PRINTLN(logFileName);

    // Log data until logging alarm triggers
    while (!alarmFlag)
    {
      // Reset watchdog
      petDog();

      // Check for the arrival of new data and process it
      gnss.checkUblox();

      // Check if sdWriteSize bytes waiting in the buffer
      while (gnss.fileBufferAvailable() >= sdWriteSize)
      {
        // Blink to indicate SD write
        digitalWrite(LED_BUILTIN, HIGH);

        // Reset watchdog
        petDog();

        // Create buffer to store data during writes to SD card
        uint8_t myBuffer[sdWriteSize];

        // Extract exactly sdWriteSize bytes from the UBX file buffer and put them into myBuffer
        gnss.extractFileBufferData((uint8_t *)&myBuffer, sdWriteSize);

        // Write exactly sdWriteSize bytes from myBuffer to the ubxDataFile on the SD card
        logFile.write(myBuffer, sdWriteSize);

        // Update bytesWritten
        bytesWritten += sdWriteSize;

        // Check for the arrival of new data and process it
        gnss.checkUblox();

        digitalWrite(LED_BUILTIN, LOW);
      }

      // Print fileBufferSize every 5 seconds
      if (millis() > (previousMillis + 30000))
      {
        // Sync log file
        if (!logFile.sync())
        {
          DEBUG_PRINT("Warning: "); DEBUG_PRINT(logFileName); DEBUG_PRINT(" sync error!");
        }
        else
        {
          //DEBUG_PRINT("Info: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" synced.");
        }

        // Print how many bytes have been written to SD card
        DEBUG_PRINT("Info: "); DEBUG_PRINT(bytesWritten); DEBUG_PRINT(" bytes written. ");

        // Get max file buffer size
        maxBufferBytes = gnss.getMaxFileBufferAvail();

        DEBUG_PRINT(" Max file buffer is "); DEBUG_PRINT(maxBufferBytes); DEBUG_PRINTLN(" bytes.");

        // Warning if fileBufferSize was more than 80% full
        if (maxBufferBytes > ((fileBufferSize / 5) * 4))
        {
          DEBUG_PRINTLN("Warning: File buffer > 80 % full. Data loss may have occurrred.");
        }

        // Update millis counter
        previousMillis = millis();
      }
    }

    // Check for bytes remaining in file buffer
    uint16_t remainingBytes = gnss.fileBufferAvailable();

    while (remainingBytes > 0)
    {
      // Reset watchdog
      petDog();

      // Create buffer to store data while writing to SD card
      uint8_t myBuffer[sdWriteSize];

      // Write the remaining bytes to SD card sdWriteSize bytes at a time
      uint16_t bytesToWrite = remainingBytes;
      if (bytesToWrite > sdWriteSize)
      {
        bytesToWrite = sdWriteSize;
      }

      // Extract bytesToWrite bytes from the UBX file buffer and put them into myBuffer
      gnss.extractFileBufferData((uint8_t *)&myBuffer, bytesToWrite);

      // Write bytesToWrite bytes from myBuffer to the ubxDataFile on the SD card
      logFile.write(myBuffer, bytesToWrite);

      bytesWritten += bytesToWrite; // Update total bytes written counter
      remainingBytes -= bytesToWrite; // Decrement remainingBytes
    }

    // Print how many bytes have been written to SD card
    DEBUG_PRINT("Info: "); DEBUG_PRINT(bytesWritten); DEBUG_PRINTLN(" bytes written to SD.");

    // Sync file
    if (!logFile.sync())
    {
      DEBUG_PRINT("Warning: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" sync error!");
    }
    else
    {
      DEBUG_PRINT("Info: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" synced.");
    }

    // Update the file access and write timestamps
    updateFileAccess(&logFile);

    // Close logfile
    if (!logFile.close())
    {
      DEBUG_PRINT("Warning: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" failed to close!");
    }
    else
    {
      DEBUG_PRINT("Info: "); DEBUG_PRINT(logFileName); DEBUG_PRINTLN(" closed.");
    }
  }
  else
  {
    online.dataLogging = false; // Clear flag
  }

  // Close Serial1 port
  Serial1.end();

  // Toggle logging flag
  loggingFlag = false;

  // Delay to allow previous log file to close
  blinkLed(2, 500);

  // Stop the loop timer
  timer.logGnss = millis() - loopStartTime;
}
