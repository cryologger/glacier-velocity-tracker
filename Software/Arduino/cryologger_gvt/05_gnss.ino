/*
  GNSS Module

  This module handles the initialization and configuration of the GNSS receiver,
  synchronizes RTC time with GNSS, and logs raw GNSS data to microSD.
*/

// ----------------------------------------------------------------------------
// Configure u-blox GNSS
// ----------------------------------------------------------------------------
void configureGnss() {
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Check if u-blox has been initialized
  if (!online.gnss) {
    // Disable internal I2C pull-ups
    disablePullups();

    // Uncomment  line to enable GNSS debug messages on Serial
    //gnss.enableDebugging();

    // Allocate sufficient RAM to store RAWX messages (>2 KB)
    gnss.setFileBufferSize(fileBufferSize);  // Must be called before gnss.begin()

    // Display OLED messages(s)
    displayInitialize("GNSS");

    // Initialize u-blox GNSS
    if (!gnss.begin()) {
      
      displayFailure();

      DEBUG_PRINTLN("[GNSS] Warning: u-blox failed to initialize. Reattempting...");

      // Delay between initialization attempts
      myDelay(2000);

      if (!gnss.begin()) {
        DEBUG_PRINTLN("[GNSS] Warning: u-blox failed to initialize! Please check wiring.");
        online.gnss = false;
        logDebug();  // Log system debug information

        // Display OLED messages(s)
        displayFailure();

        // Disable power to Qwiic connector
        qwiicPowerOff();

        // Disable power to peripherals
        peripheralPowerOff();
      } else {
        online.gnss = true;
        DEBUG_PRINTLN("[GNSS] Info: u-blox initialized.");

        // Display OLED messages(s)
        displaySuccess();
      }
    } else {
      online.gnss = true;
      DEBUG_PRINTLN("[GNSS] Info: u-blox initialized.");

      // Display OLED messages(s)
      displaySuccess();
    }

    // Configure communication interfaces and satellite signals only if program is running for the first time
    if (gnssConfigFlag) {
      bool response = true;

      // Configure communciation interfaces
      response &= gnss.newCfgValset();                             // Defaults to configuring the setting in RAM and BBR
      response &= gnss.newCfgValset8(UBLOX_CFG_I2C_ENABLED, 1);    // Enable I2C
      response &= gnss.addCfgValset8(UBLOX_CFG_SPI_ENABLED, 0);    // Disable SPI
      response &= gnss.addCfgValset8(UBLOX_CFG_UART1_ENABLED, 0);  // Disable UART1
      response &= gnss.addCfgValset8(UBLOX_CFG_UART2_ENABLED, 0);  // Enable UART2
      response &= gnss.addCfgValset8(UBLOX_CFG_USB_ENABLED, 0);    // Disable USB
      response &= gnss.sendCfgValset();                            // Send the packet using sendCfgValset

      if (response) {
        DEBUG_PRINTLN("[GNSS] Info: u-blox communication interfaces configured.");
      } else {
        DEBUG_PRINTLN("[GNSS] Warning: u-blox communication interfaces not configured!");
      }

      // Configure satellite signals
      response &= gnss.newCfgValset();                                             // Defaults to configuring the setting in RAM and BBR
      response &= gnss.newCfgValset8(UBLOX_CFG_SIGNAL_GPS_ENA, gnssGpsEnabled);    // Configure GPS
      response &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_GLO_ENA, gnssGloEnabled);    // Configure GLONASS
      response &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_GAL_ENA, gnssGalEnabled);    // Configure Galileo
      response &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_BDS_ENA, gnssBdsEnabled);    // Configure Beidou
      response &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_SBAS_ENA, gnssSbasEnabled);  // Configure SBAS
      response &= gnss.addCfgValset8(UBLOX_CFG_SIGNAL_QZSS_ENA, gnssQzssEnabled);  // Configure QZSS
      response &= gnss.sendCfgValset();                                            // Send the packet using sendCfgValset
      myDelay(2000);

      if (response) {
        DEBUG_PRINTLN("[GNSS] Info: u-blox satellite signals configured.");
      } else {
        DEBUG_PRINTLN("[GNSS] Warning: u-blox satellite signals not configured!");
      }

      // Clear flag
      gnssConfigFlag = false;

    }

    // Configure u-blox GNSS
    gnss.setI2COutput(COM_TYPE_UBX);                  // Set the I2C port to output UBX only (disable NMEA)
    gnss.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);  // Save communications port settings to flash and BBR
    gnss.setNavigationFrequency(1);                   // Produce 1 navigation solution(s) per second
    gnss.setAutoPVT(true);                            // Enable automatic NAV-PVT messages
    gnss.setAutoRXMSFRBX(true, false);                // Enable automatic RXM-SFRBX messages
    gnss.setAutoRXMRAWX(true, false);                 // Enable automatic RXM-RAWX messages
    gnss.logRXMSFRBX();                               // Enable RXM-SFRBX data logging
    gnss.logRXMRAWX();                                // Enable RXM-RAWX data logging
  } else {
    DEBUG_PRINTLN("[GNSS] Info: GNSS already initialized.");
  }

  // Stop the loop timer
  timer.gnss = millis() - loopStartTime;

  //DEBUG_PRINT("Debug - configureGnss(): "); DEBUG_PRINTLN(timer.gnss);
}

// ----------------------------------------------------------------------------
// Acquire valid GNSS fix and sync RTC
// ----------------------------------------------------------------------------
void syncRtc() {
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Check if u-blox GNSS initialized successfully
  if (online.gnss) {
    // Disable internal I2C pull-ups
    disablePullups();

    // Clear flag
    rtcSyncFlag = false;
    rtcDrift = 0;
    fixCounter = 0;

    DEBUG_PRINTLN("[GNSS] Info: Attempting to sync RTC with GNSS...");

    // Attempt to acquire a valid GNSS position fix for up to 5 minutes
    while (!rtcSyncFlag && millis() - loopStartTime < gnssTimeout * 1000UL) {
      petDog();  // Reset WDT

      // Check for UBX-NAV-PVT messages
      if (gnss.getPVT()) {
        // Blink LED
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        bool dateValidFlag = gnss.getConfirmedDate();
        bool timeValidFlag = gnss.getConfirmedTime();
        byte fixType = gnss.getFixType();

#if DEBUG_GNSS
        char gnssBuffer[100];
        sprintf(gnssBuffer, "%04u-%02d-%02d %02d:%02d:%02d.%03d,%ld,%ld,%d,%d,%d,%d,%d",
                gnss.getYear(), gnss.getMonth(), gnss.getDay(),
                gnss.getHour(), gnss.getMinute(), gnss.getSecond(), gnss.getMillisecond(),
                gnss.getLatitude(), gnss.getLongitude(), gnss.getSIV(),
                gnss.getPDOP(), gnss.getFixType(),
                dateValidFlag, timeValidFlag);
        DEBUG_PRINT("[GNSS] Info: ");
        DEBUG_PRINTLN(gnssBuffer);

        // Display OLED messages(s)
        displayRtcSyncStatus();
#endif

        // Check if date and time are valid
        if (fixType >= 2 && dateValidFlag && timeValidFlag) {
          fixCounter++;

          // Collect a minimum number of valid positions before synchronizing RTC with GNSS
          if (fixCounter >= 10) {
            unsigned long rtcEpoch = rtc.getEpoch();        // Get RTC epoch time
            unsigned long gnssEpoch = gnss.getUnixEpoch();  // Get GNSS epoch time
            rtc.setEpoch(gnssEpoch);                        // Set RTC date and time
            rtcDrift = gnssEpoch - rtcEpoch;                // Calculate RTC drift
            rtcSyncFlag = true;                             // Set flag

            DEBUG_PRINT("[GNSS] Info: RTC time synced to ");
            printDateTime();
            DEBUG_PRINT("[GNSS] Info: RTC drift = ");
            DEBUG_PRINTLN(rtcDrift);

            // Update logfile timestamp if more than 30 seconds of drift
            if (abs(rtcDrift) > 30) {
              DEBUG_PRINTLN("[GNSS] Info: Updating logfile timestamp");
              rtc.getTime();     // Get the RTC's date and time
              getLogFileName();  // Update logfile timestamp
            }

            // Display OLED messages(s)
            displayRtcOffset(rtcDrift);
          }
        }
      }
    }
    if (!rtcSyncFlag) {
      DEBUG_PRINTLN("[GNSS] Warning: Unable to sync RTC!");

      // Display OLED messages(s)
      displayRtcFailure();
    }
  } else {
    DEBUG_PRINTLN("[GNSS] Warning: GNSS offline!");
    rtcSyncFlag = false;  // Clear flag
  }

  // Stop the loop timer
  timer.syncRtc = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Log UBX-RXM-RAWX/SFRBX data
// ----------------------------------------------------------------------------
void logGnss() {
  // Start loop timer
  unsigned long loopStartTime = millis();

  bool displayDebug = true;
  byte displayCounter = 0;
  bool displayToggle = false;

  // Record logging start time
  logStartTime = rtc.getEpoch();

  // Check if microSD and u-blox GNSS initialized successfully
  if (online.microSd && online.gnss) {
    // Disable internal I2C pull-ups
    disablePullups();

    // Create a new log file and open for writing
    // O_CREAT  - Create the file if it does not exist
    // O_APPEND - Seek to the end of the file prior to each write
    // O_WRITE  - Open the file for writing
    if (!logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE)) {
      DEBUG_PRINT("[GNSS] Warning: Failed to create log file");
      DEBUG_PRINTLN(logFileName);
      return;
    } else {
      online.logGnss = true;
      DEBUG_PRINT("[GNSS] Info: Created log file ");
      DEBUG_PRINTLN(logFileName);
    }

    // Update file create timestamp
    updateFileCreate(&logFile);

    // Reset counters
    bytesWritten = 0;
    writeFailCounter = 0;
    syncFailCounter = 0;
    closeFailCounter = 0;

    gnss.clearFileBuffer();          // Clear file buffer
    gnss.clearMaxFileBufferAvail();  // Reset max file buffer size

    DEBUG_PRINTLN("[GNSS] Info: Starting logging...");

    // Log data until logging alarm triggers
    while (!alarmFlag) {
      // Reset watchdog
      petDog();

      // Check for the arrival of new data and process it
      gnss.checkUblox();

      // Check if sdWriteSize bytes are waiting in the buffer
      while (gnss.fileBufferAvailable() >= sdWriteSize) {
        // Reset WDT
        petDog();

        // Turn on LED during SD writes
        digitalWrite(LED_BUILTIN, HIGH);

        // Create buffer to store data during writes to SD card
        uint8_t myBuffer[sdWriteSize];

        // Extract exactly sdWriteSize bytes from the UBX file buffer and put them into myBuffer
        gnss.extractFileBufferData((uint8_t *)&myBuffer, sdWriteSize);

        // Write exactly sdWriteSize bytes from myBuffer to the ubxDataFile on the SD card
        if (!logFile.write(myBuffer, sdWriteSize)) {
          DEBUG_PRINTLN("[GNSS] Warning: Failed to write to log file!");
          writeFailCounter++;  // Count number of failed writes to microSD
        }

        // Update bytesWritten
        bytesWritten += sdWriteSize;

        // If SD writing is slow or there is a lot of data to write, keep checking for the arrival of new data
        gnss.checkUblox();  // Check for the arrival of new data and process it

        // Turn off LED
        digitalWrite(LED_BUILTIN, LOW);
      }

      // Periodically print number of bytes written
      if (millis() - previousMillis > 10000) {
        // Sync the log file
        if (!logFile.sync()) {
          DEBUG_PRINTLN("[GNSS] Warning: Failed to sync log file!");
          syncFailCounter++;  // Count number of failed file syncs
        }

        // Print number of bytes written to SD card
        DEBUG_PRINT("[GNSS] Info: ");
        DEBUG_PRINT(bytesWritten);
        DEBUG_PRINT(" bytes written. ");

        // Get max file buffer size
        maxBufferBytes = gnss.getMaxFileBufferAvail();
        DEBUG_PRINT("Max buffer: ");
        DEBUG_PRINTLN(maxBufferBytes);

        // Warn if fileBufferSize was more than 80% full
        if (maxBufferBytes > ((fileBufferSize / 5) * 4)) {
          DEBUG_PRINTLN("[GNSS] Warning: File buffer >80 % full. Data loss may have occurrred.");
        }

        // Display logging information to OLED display
        if (online.oled && displayDebug) {
          // After a specified number of cycles put OLED to sleep (1.2 uA)
          if (displayCounter <= 100)  // Use >= 0 for testing and <= 100 for deployment
          {
            displayCounter++;
            if (!displayToggle) {
              displayScreen1();  // Display information panel 1
              displayToggle = !displayToggle;
            } else {
              displayScreen2();  // Display information panel 2
              displayToggle = !displayToggle;
            }
          } else {
            oled.displayPower(0);  // Put OLED display into sleep mode
            displayDebug = false;  // Clear flag
          }
        }
        previousMillis = millis();  // Update previousMillis
      }
    }

    // Check for bytes remaining in file buffer
    uint16_t remainingBytes = gnss.fileBufferAvailable();

    while (remainingBytes > 0) {
      // Reset WDT
      petDog();

      // Turn on LED during SD writes
      digitalWrite(LED_BUILTIN, HIGH);

      // Create buffer to store data during writes to SD card
      uint8_t myBuffer[sdWriteSize];

      // Write the remaining bytes to SD card sdWriteSize bytes at a time
      uint16_t bytesToWrite = remainingBytes;
      if (bytesToWrite > sdWriteSize) {
        bytesToWrite = sdWriteSize;
      }

      // Extract bytesToWrite bytes from the UBX file buffer and put them into myBuffer
      gnss.extractFileBufferData((uint8_t *)&myBuffer, bytesToWrite);

      // Write bytesToWrite bytes from myBuffer to the ubxDataFile on the SD card
      logFile.write(myBuffer, bytesToWrite);

      bytesWritten += bytesToWrite;    // Update bytesWritten
      remainingBytes -= bytesToWrite;  // Decrement remainingBytes

      // Turn off LED.
      digitalWrite(LED_BUILTIN, LOW);
    }

    // Print total number of bytes written to SD card.
    DEBUG_PRINT("[GNSS] Info: Total bytes written is ");
    DEBUG_PRINTLN(bytesWritten);

    // Sync the log file
    if (!logFile.sync()) {
      DEBUG_PRINTLN("[GNSS] Warning: Failed to sync log file!");
      syncFailCounter++;  // Count number of failed file syncs
    }

    // Update file access timestamps.
    updateFileAccess(&logFile);

    // Close the log file.
    if (!logFile.close()) {
      DEBUG_PRINTLN("[GNSS] Warning: Failed to close log file!");
      closeFailCounter++;  // Count number of failed file closes
    } else {
      DEBUG_PRINTLN("[GNSS] Info: Log file closed.");
    }
  } else {
    online.logGnss = false;
    DEBUG_PRINTLN("[GNSS] Warning: u-blox ofline!");
  }

  // Stop the loop timer.
  timer.logGnss = millis() - loopStartTime;
}
