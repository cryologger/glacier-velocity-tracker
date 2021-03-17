// Configure u-blox module
void configureGnss()
{
  // Uncomment to enable GNSS debug messages on Serial
  //gnss.enableDebugging();

  // Allocate sufficient RAM to store RAWX messages (>2 KB)
  gnss.setFileBufferSize(fileBufferSize); // setFileBufferSize must be called before gnss.begin()

  // Open Serial1 port and set data rate to 460800 baud
  Serial1.begin(460800);

  // Attempt to initialize u-blox module
  if (gnss.begin(Serial1))
  {
    DEBUG_PRINTLN("Info: u-blox initialized at 460800 bps.");
    online.gnss = true;
  }
  else
  {
    DEBUG_PRINTLN("Warning: u-blox not detected at 460800 bps. Attempting 38400 bps...");

    // Change Serial1 data rate to 38400 baud (u-blox default)
    Serial1.begin(38400);

    // Attempt to initlialze u-blox module
    if (gnss.begin(Serial1))
    {
      DEBUG_PRINTLN("Info: u-blox initialized at 38400 bps.");

      // Change UART1 baud rate to 460800 bps
      bool response = true;
      response &= gnss.setVal32(UBLOX_CFG_UART1_BAUDRATE, 460800);

      DEBUG_PRINTLN("Sucess: UART1 baud rate set to 460800 bps.");

      // Open Serial1 port and set data rate to 460800 baud
      Serial1.begin(460800);

      // Attempt to initialize u-blox module
      if (gnss.begin(Serial1))
      {
        DEBUG_PRINTLN("Info: u-blox initialized at 460800 bps.");
        online.gnss = true;
      }
      else
      {
        DEBUG_PRINTLN("Warning: u-blox not detected at 460800 bps. Please check wiring or baud rate.");
        online.gnss = false;
      }
    }
    else
    {
      DEBUG_PRINTLN("Warning: u-blox not detected at 38400 bps. Please check wiring or baud rate.");
      online.gnss = false;
    }
  }

  // If u-blox is offline, there is a communication issue
  // TO DO: Figure out what to do
  if (!online.gnss)
  {
    while (1)
    {
      blinkLed(2, 250);
      blinkLed(2, 1000);
    }
  }

  // Uncomment to reset u-blox to default factory settings with 1 Hz navigation rate
  //gnss.factoryDefault();
  //delay(5000);

  // Configure u-blox module
  gnss.setUART1Output(COM_TYPE_UBX);                // Set the UART1 port to output UBX only (disable NMEA)
  gnss.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);  // Save communications port settings to Flash and BBR
  gnss.setNavigationFrequency(10);                   // Produce one navigation solution per second
  gnss.setAutoPVTcallback(&processNavPvt);          // Enable automatic NAV PVT messages with callback to syncRtc
  gnss.setAutoRXMRAWX(true);                        // Enable automatic RXM RAWX reports at the navigation frequency
  gnss.setAutoRXMSFRBX(true);                       // Enable automatic RXM SFRBX reports at the navigation frequency


  // Configure satellite signals
  bool setValueSuccess = true;
  setValueSuccess &= gnss.newCfgValset8(UBLOX_CFG_SIGNAL_GPS_ENA, 1);   // Enable GPS
  setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_GLO_ENA, 1);   // Enable GLONASS
  setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_GAL_ENA, 0);   // Enable Galileo
  setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_BDS_ENA, 0);   // Disable BeiDou
  setValueSuccess &= gnss.sendCfgValset8(UBLOX_CFG_SIGNAL_QZSS_ENA, 0, 2100);  // Disable QZSS
  if (!setValueSuccess)
  {
    DEBUG_PRINTLN("Warning: Satellite signals not successfully set");
  }
  else
  {
    DEBUG_PRINTLN("Info: Satellite signals successfully set");
  }

  delay(2000);
  // Print GNSS configuration settings
  printGnssSettings();
}

// Read the GNSS receiver
void syncRtc()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  rtcSyncFlag = false; // Clear flag

  // Check if GNSS initialized successfully
  if (online.gnss)
  {
    DEBUG_PRINTLN("Info: Acquiring GNSS fix...");

    // Attempt to acquire a valid GNSS position fix
    while (!rtcSyncFlag && millis() - loopStartTime < gnssTimeout * 60UL * 1000UL)
    {
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
    DEBUG_PRINTLN("Warning: u-blox GNSS offline!");
  }

  // Stop the loop timer
  timer.gnss = millis() - loopStartTime;
}

// Callback function to process UBX-NAV-PVT data
void processNavPvt(UBX_NAV_PVT_data_t ubx)
{
  petDog();
  // Blink LED
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  bool dateValidFlag = ubx.flags2.bits.confirmedDate;
  bool timeValidFlag = ubx.flags2.bits.confirmedTime;
  byte fixType = ubx.fixType;

#if DEBUG_GNSS
  Serial.printf("%04u-%02d-%02d %02d:%02d:%02d.%03d,%ld,%ld,%d,%d,%d,%d,%d\n",
                ubx.year, ubx.month, ubx.day,
                ubx.hour, ubx.min, ubx.sec, ubx.iTOW % 1000,
                ubx.lat, ubx.lon, ubx.numSV,
                ubx.pDOP, ubx.fixType,
                dateValidFlag, timeValidFlag);
#endif


  // Attempt to sync RTC with GNSS
  if (fixType >= 2 && dateValidFlag && timeValidFlag)
  {
    DEBUG_PRINTLN("A GNSS fix was found!");
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

    DEBUG_PRINT("RTC drift: "); DEBUG_PRINTLN(rtcDrift);

    // Set RTC date and time
    rtc.setTime(ubx.hour, ubx.min, ubx.sec, ubx.iTOW % 1000,
                ubx.day, ubx.month, ubx.year - 2000);

    rtcSyncFlag = true; // Set flag
    DEBUG_PRINT("RTC time synced to: "); printDateTime();
  }

}

void logGnss()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  gnss.logRXMRAWX();  // Enable RXM RAWX data logging
  gnss.logRXMSFRBX(); // Enable RXM SFRBX data logging

  // Reset bytesWritten counter
  bytesWritten = 0;

  // Open log file
  if (!logFile.open(logFileName, O_APPEND | O_WRITE))
  {
    DEBUG_PRINTLN("Warning: Unable to open file");
  }

  // Log data until logging alarm triggers
  while (!alarmFlag)
  {
    // Check for the arrival of new data and process it
    gnss.checkUblox();

    // Reset watchdog
    petDog();

    // Check if sdWriteSize bytes waiting in the buffer
    while (gnss.fileBufferAvailable() >= sdWriteSize)
    {
      // Blink to indicate SD write
      digitalWrite(LED_BUILTIN, HIGH);

      // Create buffer to hold data while we write it to SD card
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
    if (millis() > previousMillis + 1000)
    {
      // Print how many bytes have been written to SD card
      DEBUG_PRINT("Bytes written: "); DEBUG_PRINT(bytesWritten);

      // Get max file buffer size
      maxBufferBytes = gnss.getMaxFileBufferAvail();

      DEBUG_PRINT(" Max file buffer: "); DEBUG_PRINTLN(maxBufferBytes);

      // Warning if fileBufferSize was more than 80% full
      if (maxBufferBytes > ((fileBufferSize / 5) * 4))
      {
        //DEBUG_PRINTLN("Warning: File buffer >80% full. Data loss may have occurrred.");
      }

      // Update millis counter
      previousMillis = millis();
    }
  } // Exit log function

  // Stop logging

  // Check for bytes remaining in file buffer
  uint16_t remainingBytes = gnss.fileBufferAvailable();

  while (remainingBytes > 0)
  {
    uint8_t myBuffer[sdWriteSize]; // Create buffer to store data while writing to SD card

    uint16_t bytesToWrite = remainingBytes; // Write the remaining bytes to SD card sdWriteSize bytes at a time
    if (bytesToWrite > sdWriteSize)
    {
      bytesToWrite = sdWriteSize;
    }
    gnss.extractFileBufferData((uint8_t *)&myBuffer, bytesToWrite); // Extract bytesToWrite bytes from the UBX file buffer and put them into myBuffer

    logFile.write(myBuffer, bytesToWrite); // Write bytesToWrite bytes from myBuffer to the ubxDataFile on the SD card

    bytesWritten += bytesToWrite; // Update bytesWritten
    remainingBytes -= bytesToWrite; // Decrement remainingBytes

  }

  // Print how many bytes have been written to SD card
  DEBUG_PRINT("Total number of bytes written to SD card: ");
  DEBUG_PRINTLN(bytesWritten);

  // Update the file access and write timestamps
  updateFileAccess(&logFile);

  // Sync file
  logFile.sync();

  // Close logfile
  logFile.close();

  // Toggle logging flag
  loggingFlag = false;

  // Stop the loop timer
  timer.logGnss = millis() - loopStartTime;
}
