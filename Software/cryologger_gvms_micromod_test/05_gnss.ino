// Configure u-blox GNSS
void configureGnss()
{
  // Allocate sufficient RAM to store RAWX messages (>2 KB)
  gnss.setFileBufferSize(fileBufferSize); // Must be called before gnss.begin()

  Serial.println("Info: Opening Serial1 port at 230400 baud");

  // Open Serial1 port and set data rate to 230400 baud
  Serial1.begin(230400);

  // Initialize u-blox GNSS
  if (gnss.begin(Serial1))
  {
    Serial.println("Info: u-blox initialized at 230400 baud.");
  }
  else
  {
    Serial.println("Info: u-blox failed to initialize at 230400 bad. Attempting 38400 baud.");
    /*
        // Open Serial1 port and set data rate to 38400 baud
        Serial1.begin(38400);

        // Initialize u-blox GNSS
        if (gnss.begin(Serial1))
        {
          Serial.println("Info: u-blox initialized at 38400 baud.");

          // Set baud rate of u-blox UART1 port
          gnss.setSerialRate(230400);
          Serial.println("Info: u-blox UART1 set to 230400 baud.");
          delay(100);

          // Open Serial1 port and set data rate to 230400 baud
          Serial1.begin(230400);

          // Initialize u-blox GNSS
          if (gnss.begin(Serial1))
          {
            Serial.println("Info: u-blox initialized at 230400 baud.");
          }
          else
          {
            Serial.println("Warning: u-blox GNSS not detected! Please check wiring or baud rate.");
            //peripheralPowerOff(); // Disable power to microSD and u-blox
            //wdt.stop(); // Stop watchdog timer
            //while (1)
            //{
            //  blinkLed(2, 250);
            //  blinkLed(2, 1000);
            //}
          }
        }
        else
        {
          Serial.println("Warning: u-blox GNSS not detected at baud rates 38400 or 230400.");
        }
    */
  }

  // Configure u-blox GNSS
  gnss.setUART1Output(COM_TYPE_UBX);                // Set the UART1 port to output UBX only (disable NMEA)
  gnss.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);  // Save communications port settings to flash and BBR
  gnss.setNavigationFrequency(10);                  // Produce one navigation solution per second
  gnss.setAutoPVTcallback(&processNavPvt);          // Enable automatic NAV PVT messages with callback to processNavPvt()
  gnss.setAutoRXMSFRBX(true, false);                // Enable automatic RXM SFRBX messages
  gnss.setAutoRXMRAWX(true, false);                 // Enable automatic RXM RAWX messages

  if (!gnssConfigFlag)
  {
    // Configure communciation interfaces
    bool setValueSuccess = true;
    setValueSuccess &= gnss.newCfgValset8(UBLOX_CFG_I2C_ENABLED, 0);        // Disable I2C
    setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SPI_ENABLED, 0);        // Disable SPI
    setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_UART2_ENABLED, 0);      // Disable UART2
    setValueSuccess &= gnss.sendCfgValset8(UBLOX_CFG_USB_ENABLED, 0, 2000); // Disable USB
    if (!setValueSuccess)
    {
      Serial.println("Warning: Communication interfaces not configured!");
    }

    // Configure satellite signals
    setValueSuccess = true;
    setValueSuccess &= gnss.newCfgValset8(UBLOX_CFG_SIGNAL_GPS_ENA, 1);         // Enable GPS
    setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_GLO_ENA, 1);         // Enable GLONASS
    setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_GAL_ENA, 0);         // Disable Galileo
    setValueSuccess &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_BDS_ENA, 0);         // Disable BeiDou
    setValueSuccess &= gnss.sendCfgValset8(UBLOX_CFG_SIGNAL_QZSS_ENA, 0, 4000); // Disable QZSS
    if (!setValueSuccess)
    {
      Serial.println("Warning: Satellite signals not configured!");
    }
    gnssConfigFlag = true;
  }
  // Print current GNSS settings
  printGnssSettings();
}

// Acquire valid GNSS fix and sync RTC
void syncRtc()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Clear flag
  rtcSyncFlag = false;

  Serial.println("Info: Acquiring GNSS fix...");

  // Attempt to acquire a valid GNSS position fix for up to 5 minutes
  while (!rtcSyncFlag && millis() - loopStartTime < gnssTimeout * 60UL * 1000UL)
  {
    petDog(); // Reset watchdog timer
    gnss.checkUblox(); // Check for arrival of new data and process it
    gnss.checkCallbacks(); // Check if callbacks are waiting to be processed
  }
  if (!rtcSyncFlag)
  {
    Serial.println("Warning: Unable to sync RTC!");
  }

  // Stop loop timer
  unsigned long loopEndTime = millis() - loopStartTime;
  Serial.print("Timer: syncRtc() "); Serial.print(loopEndTime); Serial.println(" ms.");
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
  char gnssBuffer[200];
  sprintf(gnssBuffer, "%04u-%02d-%02d %02d:%02d:%02d.%03d,%ld,%ld,%d,%d,%d,%d,%d",
          ubx.year, ubx.month, ubx.day,
          ubx.hour, ubx.min, ubx.sec, ubx.iTOW % 1000,
          ubx.lat, ubx.lon, ubx.numSV,
          ubx.pDOP, ubx.fixType,
          dateValidFlag, timeValidFlag);
  Serial.println(gnssBuffer);
#endif

  // Check if date and time are valid and sync RTC with GNSS
  if (dateValidFlag && timeValidFlag)
  {
    // Set RTC date and time
    rtc.setTime(ubx.hour, ubx.min, ubx.sec, ubx.iTOW % 1000,
                ubx.day, ubx.month, ubx.year - 2000);

    rtcSyncFlag = true; // Set flag
    Serial.print("Info: RTC time synced to "); printDateTime();
  }
}

// Log UBX-RXM-RAWX/SFRBX data
void logGnss()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Create timestamped log file name
  sprintf(fileName, "20%02d%02d%02d_%02d%02d%02d.ubx",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  // Create a new log file and open for writing
  // O_CREAT  - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE  - Open the file for writing
  if (!file.open(fileName, O_CREAT | O_APPEND | O_WRITE))
  {
    Serial.print("Warning: Failed to create log file"); Serial.println(fileName);
    return;
  }
  else
  {
    Serial.print("Info: Created log file "); Serial.println(fileName);
  }

  // Update file create timestamp
  updateFileCreate();

  // Reset bytesWritten counter
  bytesWritten = 0;

  // Enable UBX-RXM-SFRBX data logging
  gnss.logRXMSFRBX();

  // Enable UBX-RXM-RAWX data logging
  gnss.logRXMRAWX();

  // Log data until logging alarm triggers
  while (!alarmFlag)
  {
    // Reset watchdog
    petDog();

    // Check for the arrival of new data and process it
    gnss.checkUblox();

    // Check if sdWriteSize bytes are waiting in the buffer
    while (gnss.fileBufferAvailable() >= sdWriteSize)
    {
      // Reset watchdog timer
      petDog(); 
      
      // Turn on LED during SD writes
      digitalWrite(LED_BUILTIN, HIGH);

      // Create buffer to store data during writes to SD card
      uint8_t myBuffer[sdWriteSize];

      // Extract exactly sdWriteSize bytes from the UBX file buffer and put them into myBuffer
      gnss.extractFileBufferData((uint8_t *)&myBuffer, sdWriteSize);

      // Write exactly sdWriteSize bytes from myBuffer to the ubxDataFile on the SD card
      file.write(myBuffer, sdWriteSize);

      // Update bytesWritten
      bytesWritten += sdWriteSize;

      // If SD writing is slow or there is a lot of data to write, keep checking for the arrival of new data
      gnss.checkUblox(); // Check for the arrival of new data and process it

      // Turn off LED
      digitalWrite(LED_BUILTIN, LOW);
    }

    // Print bytes written every second
    if (millis() > (previousMillis + 1000))
    {
      // Print number of bytes written to SD card
      Serial.print(bytesWritten); Serial.print(" bytes written. ");

      // Get max file buffer size
      uint16_t maxBufferBytes = gnss.getMaxFileBufferAvail();

      Serial.print("Max file buffer: "); Serial.println(maxBufferBytes);

      // Warn if fileBufferSize was more than 80% full
      if (maxBufferBytes > ((fileBufferSize / 5) * 4))
      {
        Serial.println("Warning: File buffer > 80 % full. Data loss may have occurrred.");
      }

      previousMillis = millis(); // Update previousMillis
    }
  }

  // Check for bytes remaining in file buffer
  uint16_t remainingBytes = gnss.fileBufferAvailable();

  while (remainingBytes > 0)
  {
    // Reset watchdog timer
    petDog();
    
    // Turn on LED during SD writes
    digitalWrite(LED_BUILTIN, HIGH);

    // Create buffer to store data during writes to SD card
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
    file.write(myBuffer, bytesToWrite);

    bytesWritten += bytesToWrite; // Update bytesWritten
    remainingBytes -= bytesToWrite; // Decrement remainingBytes

    // Turn off LED
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Print total number of bytes written to SD card
  Serial.print("Info: Total bytes written is "); Serial.println(bytesWritten);

  // Sync the log file
  if (!file.sync())
  {
    Serial.println("Warning: Failed to sync log file!");
  }

  // Update file access timestamps
  updateFileAccess();

  // Close the log file
  if (!file.close())
  {
    Serial.println("Warning: Failed to close log file!");
  }

  // Close Serial1 port
  Serial1.end();

  // Toggle logging flag
  loggingFlag = false;

  // Stop loop timer
  unsigned long loopEndTime = millis() - loopStartTime;
  Serial.print("Timer: logGnss() "); Serial.print(loopEndTime); Serial.println(" ms.");
}
