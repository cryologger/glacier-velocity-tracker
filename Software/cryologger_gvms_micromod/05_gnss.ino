
void configureGnss()
{
  //gnss.enableDebugging();             // Uncomment to enable GNSS debug messages on Serial
  //gnss.enableDebugging(Serial, true); // Uncomment to enable only the important GNSS debug messages on Serial
  gnss.disableUBX7Fcheck();           // Disable the "7F" check in checkUbloxI2C as RAWX data can legitimately contain 0x7F

  // Allocate sufficient RAM to store RAWX messages (>2 KB)
  // getMaxFileBufferAvail indicates maximum number of bytes the file buffer has contained
  gnss.setFileBufferSize(fileBufferSize); // setFileBufferSize must be called before gnss.begin()

  // Connect to the u-blox module using Wire port
  if (gnss.begin())
  {
    gnss.setI2COutput(COM_TYPE_UBX);                  // Set the I2C port to output UBX only (disable NMEA)
    gnss.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);  // Save communications port settings to Flash and BBR
    gnss.setNavigationFrequency(1);                   // Produce one navigation solution per second
    gnss.setAutoRXMSFRBX(true, false);                // Enable automatic RXM SFRBX messages without callback (implicit update)
    gnss.logRXMSFRBX();                               // Enable RXM SFRBX data logging
    gnss.setAutoRXMRAWX(true, false);                 // Enable automatic RXM RAWX messages without callback (implicit update)
    gnss.logRXMRAWX();                                // Enable RXM RAWX data logging
    gnss.setAutoPVTcallback(&syncRtc);                // Enable automatic NAV PVT messages with callback to syncRtc
    gnss.logNAVPVT();                                 // Enable NAV PVT data logging

    DEBUG_PRINTLN("u-blox GNSS initialized.");
    online.gnss = true;
  }
  else
  {
    DEBUG_PRINTLN("Warning: u-blox GNSS not detected at default I2C address. Please check wiring.");
    online.gnss = false;
    digitalWrite(LED_BUILTIN, HIGH);
    while (1);
  }

  // Uncomment to reset u-blox module to default factory settings with 1 Hz navigation rate
  //gnss.factoryDefault();
  //delay(5000);

}

// Read the GNSS receiver
void readGnss()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  rtcSyncFlag = false; // Clear flag
  gnssFixFlag = false; // Clear flag
  gnssFixCounter = 0; // Reset counter

  // Check if GNSS initialized successfully
  if (online.gnss)
  {
    DEBUG_PRINTLN("Acquiring GNSS fix...");

    // Attempt to acquire a valid GNSS position fix
    while (!gnssFixFlag && millis() - loopStartTime < gnssTimeout * 1000UL)
    {
      gnss.checkUblox(); // Check for arrival of new data and process it
      gnss.checkCallbacks(); // Check if callbacks are waiting to be processed
    }
    if (!gnssFixFlag)
    {
      DEBUG_PRINTLN("Warning: Unable to acquire GNSS fix!");
    }
  }
  else
  {
    DEBUG_PRINTLN("Warning: u-blox GNSS offline!");
  }

  // Turn off LED
  digitalWrite(LED_BUILTIN, LOW);

  // Stop the loop timer
  timer.gnss = millis() - loopStartTime;
}

// Callback function to process UBX-NAV-PVT data
void syncRtc(UBX_NAV_PVT_data_t ubx)
{
  // Reset the Watchdog Timer every second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 1000)
  {
    previousMillis = currentMillis;
    petDog();
  }

  // Read battery voltage
  readBattery();

  // Blink LED
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  bool dateValidFlag = ubx.flags2.bits.confirmedDate;
  bool timeValidFlag = ubx.flags2.bits.confirmedTime;
  byte fixType = ubx.fixType;

#if DEBUG_GNSS
  char gnssBuffer[100];
  sprintf(gnssBuffer, "%04u-%02d-%02d %02d:%02d:%02d.%03d,%ld,%ld,%d,%d,%d,%d,%d",
          ubx.year, ubx.month, ubx.day,
          ubx.hour, ubx.min, ubx.sec, ubx.iTOW % 1000,
          ubx.lat, ubx.lon, ubx.numSV,
          ubx.pDOP, ubx.fixType,
          dateValidFlag, timeValidFlag);
  DEBUG_PRINTLN(gnssBuffer);
#endif

  // Check if GNSS fix is valid
  if (fixType == 3)
  {
    gnssFixCounter += 2; // Increment counter
  }
  else if (fixType == 2)
  {
    gnssFixCounter += 1; // Increment counter
  }

  // Check if GNSS fix threshold has been reached
  if (gnssFixCounter >= gnssFixCounterMax)
  {
    DEBUG_PRINTLN("A GNSS fix was found!");
    gnssFixFlag = true; // Set fix flag

    // Attempt to sync RTC with GNSS
    if (dateValidFlag && timeValidFlag)
    {
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
      int rtcDrift = rtc.getEpoch() - gnssEpoch;

      DEBUG_PRINT("RTC drift: "); DEBUG_PRINTLN(rtcDrift);

      // Set RTC date and time
      rtc.setTime(ubx.hour, ubx.min, ubx.sec, ubx.iTOW % 1000,
                  ubx.day, ubx.month, ubx.year - 2000);

      rtcSyncFlag = true; // Set flag
      DEBUG_PRINT("RTC time synced to: "); printDateTime();
    }
    else
    {
      DEBUG_PRINTLN("Warning: RTC sync not performed due to invalid GNSS fix!");
    }
  }
}

void logGnss()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Open log file
  if (!file.open(fileName, O_APPEND | O_WRITE))
  {
    DEBUG_PRINTLN("Warning: Unable to open file");
  }

  // Flush data
  gnss.flushRXMSFRBX();
  gnss.flushRXMRAWX();

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
      file.write(myBuffer, sdWriteSize);

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
      DEBUG_PRINT("Bytes written to SD card: "); DEBUG_PRINTLN(bytesWritten);

      // Get max file buffer size
      uint16_t maxBufferBytes = gnss.getMaxFileBufferAvail();

      DEBUG_PRINT("Max file buffer size: "); DEBUG_PRINTLN(maxBufferBytes);

      // Warning if fileBufferSize was more than 80% full
      if (maxBufferBytes > ((fileBufferSize / 5) * 4))
      {
        DEBUG_PRINTLN("Warning: File buffer >80% full. Data loss may have occurrred.");
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

    file.write(myBuffer, bytesToWrite); // Write bytesToWrite bytes from myBuffer to the ubxDataFile on the SD card

    bytesWritten += bytesToWrite; // Update bytesWritten
    remainingBytes -= bytesToWrite; // Decrement remainingBytes

  }

  // Print how many bytes have been written to SD card
  DEBUG_PRINT("Total number of bytes written to SD card: ");
  DEBUG_PRINTLN(bytesWritten);

  // Update the file access and write timestamps
  updateFileAccess();

  // Sync file
  file.sync();

  // Close logfile
  file.close();

  // Toggle logging flag
  loggingFlag = false;

  // Stop the loop timer
  timer.logGnss = millis() - loopStartTime;

}
