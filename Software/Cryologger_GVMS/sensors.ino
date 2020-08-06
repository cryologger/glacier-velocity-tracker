// Initialize all enabled sensors
bool beginSensors()
{
  // If no sensors are available then return
  if (!detectQwiicDevices()) // Set I2C bus to 100 kHz
  {
    if (settings.printMajorDebugMessages)
    {
      Serial.println(F("beginSensors: no Qwiic devices detected!"));
    }
    qwiicOnline.uBlox = false;
    return (false);
  }

  determineMaxI2CSpeed(); // Try for 400kHz but reduce if the user has selected a slower speed

  if (qwiicAvailable.uBlox && (!qwiicOnline.uBlox || gnssSettingsFlag)) // Only do this if the sensor is not online
  {
    gnssSettingsFlag = false;
    if (gnss.begin(qwiic, settings.sensor_uBlox.ubloxI2Caddress)) // Wire port, Address. Default is 0x42.
    {
      // Try up to three times to get the module info
      if (!getModuleInfo(1100)) // Try to get the module info
      {
        if (settings.printMajorDebugMessages)
        {
          Serial.println(F("beginSensors: first getModuleInfo call failed. Trying again..."));
        }
        if (!getModuleInfo(1100)) // Try to get the module info
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("beginSensors: second getModuleInfo call failed. Trying again..."));
          }
          if (!getModuleInfo(1100)) // Try to get the module info
          {
            if (settings.printMajorDebugMessages)
            {
              Serial.println(F("beginSensors: third getModuleInfo call failed! Giving up..."));
              qwiicOnline.uBlox = false;
              return (false);
            }
          }
        }
      }

      // Print the PROTVER
      if (settings.printMajorDebugMessages)
      {
        Serial.print(F("beginSensors: GNSS module found: PROTVER="));
        Serial.print(minfo.protVerMajor);
        Serial.print(F("."));
        Serial.print(minfo.protVerMinor);
        Serial.print(F(" SPG=")); // Standard Precision
        Serial.print(minfo.SPG);
        Serial.print(F(" HPG=")); // High Precision (ZED-F9P)
        Serial.print(minfo.HPG);  // RAWX can only be enabled on HPG and TIM modules
        Serial.print(F(" ADR=")); // Dead Reckoning (ZED-F9K)
        Serial.print(minfo.ADR);
        Serial.print(F(" UDR=")); // Untethered Dead Reckoning (NEO-M8U)
        Serial.print(minfo.UDR);
        Serial.print(F(" TIM=")); // Time sync (ZED-F9T)
        Serial.print(minfo.TIM);  // RAWX can only be enabled on HPG and TIM modules
        Serial.print(F(" FTS=")); // Frequency and time sync
        Serial.print(minfo.FTS);  // Let's guess that we can enable RAWX on FTS modules
        Serial.print(F(" LAP=")); // Lane accurate (ZED-F9R)
        Serial.print(minfo.LAP);
        Serial.print(F(" HDG=")); // Heading (ZED-F9H)
        Serial.print(minfo.HDG);
        Serial.print(F(" MOD=")); // Module type
        Serial.println(minfo.mod);
      }

      // Check the PROTVER is >= 27
      if (minfo.protVerMajor < 27)
      {
        if (settings.printMajorDebugMessages)
        {
          Serial.print(F("beginSensors: module does not support the configuration interface. Aborting!"));
        }
        qwiicOnline.uBlox = false;
        return (false);
      }

      // Set I2C port to output UBX only (turn off NMEA noise)
      gnss.newCfgValset8(0x10720001, 1, VAL_LAYER_RAM | VAL_LAYER_BBR); // CFG-I2COUTPROT-UBX     : Enable UBX output on the I2C port (in RAM and BBR)
      gnss.addCfgValset8(0x10720002, 0);                                // CFG-I2COUTPROT-NMEA    : Disable NMEA output on the I2C port
      if (minfo.HPG)
      {
        gnss.addCfgValset8(0x10720004, 0);                              // CFG-I2COUTPROT-RTCM3X  : Disable RTCM3 output on the I2C port (Precision modules only)
      }
      uint8_t success = gnss.sendCfgValset8(0x20920001, 0, 2100);       // CFG-INFMSG-UBX_I2C     : Disable UBX INFO messages on the I2C port (maxWait 2100ms)
      if (success == 0)
      {
        if (settings.printMajorDebugMessages)
        {
          Serial.println(F("beginSensors: sendCfgValset failed when setting the I2C port to output UBX only"));
        }
      }
      else
      {
        if (settings.printMinorDebugMessages)
        {
          Serial.println(F("beginSensors: sendCfgValset was successful when setting the I2C port to output UBX only"));
        }
      }

      // Disable all messages in RAM (maxWait 2100)
      disableMessages(2100);

      // Disable USB port if required
      if (!settings.sensor_uBlox.enableUSB)
      {
        success = gnss.setVal8(0x10650001, 0, VAL_LAYER_RAM, 1100); // CFG-USB-ENABLED (in RAM only)
        if (success == 0)
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset failed when disabling the USB port"));
          }
        }
        else
        {
          if (settings.printMinorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset was successful when disabling the USB port"));
          }
        }
      }

      // Disable UART1 port if required
      if (!settings.sensor_uBlox.enableUART1)
      {
        success = gnss.setVal8(0x10520005, 0, VAL_LAYER_RAM, 1100); // CFG-UART1-ENABLED (in RAM only)
        if (success == 0)
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset failed when disabling UART1"));
          }
        }
        else
        {
          if (settings.printMinorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset was successful when disabling UART1"));
          }
        }
      }

      // Disable UART2 port if required
      if (!settings.sensor_uBlox.enableUART2)
      {
        success = gnss.setVal8(0x10530005, 0, VAL_LAYER_RAM, 1100); // CFG-UART2-ENABLED (in RAM only)
        if (success == 0)
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset failed when disabling UART2"));
          }
        }
        else
        {
          if (settings.printMinorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset was successful when disabling UART2"));
          }
        }
      }

      // Disable SPI port if required
      if (!settings.sensor_uBlox.enableSPI)
      {
        success = gnss.setVal8(0x10640006, 0, VAL_LAYER_RAM, 1100); // CFG-SPI-ENABLED (in RAM only)
        if (success == 0)
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset failed when disabling the SPI port"));
          }
        }
        else
        {
          if (settings.printMinorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset was successful when disabling the SPI port"));
          }
        }
      }

      // Update settings.sensor_uBlox.minMeasIntervalGps and settings.sensor_uBlox.minMeasIntervalAll according to module type
      if (strcmp(minfo.mod, "ZED-F9P") == 0)            // Is this a ZED-F9P?
      {
        // GPS + GLO RAW = 25 Hz = 1 / 25 * 1000 = 40 ms
        // GPS + GLO + GAL + BDS = 20 Hz = 1 / 20 * 1000 = 50 ms
        settings.sensor_uBlox.minMeasIntervalGps = 40;  // ZED-F9P can do 20 Hz RTK
        settings.sensor_uBlox.minMeasIntervalAll = 50; // ZED-F9P can do 8 Hz RTK
      }
      else if (strcmp(minfo.mod, "ZED-F9K") == 0)       // Is this a ZED-F9K?
      {
        settings.sensor_uBlox.minMeasIntervalGps = 33;  // ZED-F9K can do 30 Hz
        settings.sensor_uBlox.minMeasIntervalAll = 100; // ZED-F9K can do 10 Hz (Guess!)
      }
      else if (strcmp(minfo.mod, "ZED-F9R") == 0)       // Is this a ZED-F9R?
      {
        settings.sensor_uBlox.minMeasIntervalGps = 33;  // ZED-F9R can do 30 Hz
        settings.sensor_uBlox.minMeasIntervalAll = 100; // ZED-F9R can do 10 Hz (Guess!)
      }
      else if (strcmp(minfo.mod, "ZED-F9H") == 0)       // Is this a ZED-F9H?
      {
        settings.sensor_uBlox.minMeasIntervalGps = 33;  // ZED-F9H can do 30 Hz
        settings.sensor_uBlox.minMeasIntervalAll = 100; // ZED-F9H can do 10 Hz (Guess!)
      }
      else if (strcmp(minfo.mod, "ZED-F9T") == 0)       // Is this a ZED-F9T?
      {
        settings.sensor_uBlox.minMeasIntervalGps = 50;  // ZED-F9T can do 20 Hz
        settings.sensor_uBlox.minMeasIntervalAll = 125; // ZED-F9T can do 8 Hz
      }
      else if (strcmp(minfo.mod, "NEO-M9N") == 0)       // Is this a NEO-M9N?
      {
        settings.sensor_uBlox.minMeasIntervalGps = 40;  // NEO-M9N can do 25 Hz
        settings.sensor_uBlox.minMeasIntervalAll = 40;  // NEO-M9N can do 25 Hz
      }
      else
      {
        settings.sensor_uBlox.minMeasIntervalGps = 50;  // Default to 20 Hz
        settings.sensor_uBlox.minMeasIntervalAll = 125; // Default to 8 Hz
      }

      // Calculate measurement rate
      uint16_t measRate;
      if (settings.usBetweenReadings < (((uint32_t)settings.sensor_uBlox.minMeasIntervalGps) * 1000)) // Check if usBetweenReadings is too low
      {
        measRate = settings.sensor_uBlox.minMeasIntervalGps;
      }
      else if (settings.usBetweenReadings > (0xFFFF * 1000)) // Check if usBetweenReadings is too high
      {
        measRate = 0xFFFF;
      }
      else
      {
        measRate = (uint16_t)(settings.usBetweenReadings / 1000); // Convert usBetweenReadings to ms
      }

      // If measurement interval is less than minMeasIntervalAll then disable all constellations except GPS
      // If measurement interval is less than minMeasIntervalRawxAll and RAWX is enabled then also disable all constellations except GPS to limit I2C traffic
      if ((measRate < settings.sensor_uBlox.minMeasIntervalAll) || ((measRate < settings.sensor_uBlox.minMeasIntervalRawxAll) && (settings.sensor_uBlox.logUBXRXMRAWX)))
      {
        gnss.newCfgValset8(0x10310021, 0, VAL_LAYER_RAM);    // CFG-SIGNAL-GAL_ENA  : Disable Galileo (in RAM only)
        gnss.addCfgValset8(0x10310022, 0);                   // CFG-SIGNAL-BDS_ENA  : Disable BeiDou
        gnss.addCfgValset8(0x10310024, 0);                   // CFG-SIGNAL-QZSS_ENA : Disable QZSS
        success = gnss.sendCfgValset8(0x10310025, 0, 2100);  // CFG-SIGNAL-GLO_ENA  : Disable GLONASS (maxWait 2100ms)
        if (success == 0)
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset failed when disabling constellations"));
          }
        }
        else
        {
          if (settings.printMinorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset was successful when disabling constellations"));
          }
        }
      }
      else
      {
        gnss.newCfgValset8(0x10310021, 1, VAL_LAYER_RAM);    // CFG-SIGNAL-GAL_ENA  : Enable Galileo (in RAM only)
        gnss.addCfgValset8(0x10310022, 1);                   // CFG-SIGNAL-BDS_ENA  : Enable BeiDou
        gnss.addCfgValset8(0x10310024, 1);                   // CFG-SIGNAL-QZSS_ENA : Enable QZSS
        success = gnss.sendCfgValset8(0x10310025, 1, 2100);  // CFG-SIGNAL-GLO_ENA  : Enable GLONASS (maxWait 2100ms)
        if (success == 0)
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset failed when enabling constellations"));
          }
        }
        else
        {
          if (settings.printMinorDebugMessages)
          {
            Serial.println(F("beginSensors: sendCfgValset was successful when enabling constellations"));
          }
        }
      }

      // Set output rate
      gnss.newCfgValset16(0x30210001, measRate, VAL_LAYER_RAM); // CFG-RATE-MEAS  : Configure measurement period (in RAM only)
      success = gnss.sendCfgValset16(0x30210002, 1, 2100);      // CFG-RATE-NAV   : 1 measurement per navigation solution (maxWait 2100ms)
      if (success == 0)
      {
        if (settings.printMajorDebugMessages)
        {
          Serial.println(F("beginSensors: sendCfgValset failed when setting message interval"));
        }
      }
      else
      {
        if (settings.printMinorDebugMessages)
        {
          Serial.println(F("beginSensors: sendCfgValset was successful when setting message interval"));
        }
      }

      // Enable the selected messages in RAM (MaxWait 2100)
      enableConstellations(2100);

      // Enable the selected messages in RAM (MaxWait 2100)
      enableMessages(2100);

      qwiicOnline.uBlox = true;
    }
  }
  return (true);
}

// Check I2C bus for u-blox module(s)
bool detectQwiicDevices()
{
  bool somethingDetected = false;

  qwiic.setClock(100000); // During detection, go slow

  //qwiic.setPullups(0); // Disable pull-ups as the u-blox modules have their own pull-ups (commented by PaulZC - beginQwiic does this instead)

  // Depending on what hardware is configured, the Qwiic bus may have only been turned on a few ms ago
  // Give sensors, specifically those with a low I2C address, time to turn on
  //delay(100); // SCD30 required >50ms to turn on (commented by PaulZC - we always wait for 250ms after turning on the Qwiic power)

  uint8_t address = settings.sensor_uBlox.ubloxI2Caddress;

  qwiic.beginTransmission(address);
  if (qwiic.endTransmission() == 0)
  {
    if (settings.printMinorDebugMessages)
    {
      Serial.printf("Device found at address 0x%02X\n", address);
    }
    if (gnss.begin(qwiic, address)) // Wire port, address
    {
      qwiicAvailable.uBlox = true;
      somethingDetected = true;
    }
  }
  return (somethingDetected);
}

// Close the current log file and reset the GNSS
void resetGnss()
{
  if (settings.logData && online.microSd && online.dataLogging) // Check if we are logging
  {
    if (qwiicAvailable.uBlox && qwiicOnline.uBlox) // Check if the u-blox is available and logging
    {
      // Disable all messages in RAM (maxWait 0)
      disableMessages(0);
      // Using a maxWait of zero means we don't wait for the ACK/NACK
      // and success will always be false (sendCommand returns SFE_UBLOX_STATUS_SUCCESS not SFE_UBLOX_STATUS_DATA_SENT)

      unsigned long pauseUntil = millis() + 2100UL; // Wait >> 500ms so we can be sure SD data is synced
      while (millis() < pauseUntil) // While we are pausing, keep writing data to SD
      {
        storeData(); // Read I2C data and write to SD
      }

      // Close the current log file
      Serial.print("Closing: "); Serial.println(fileName);
      file.sync();
      updateDataFileAccess(); // Update the file access and write timestamps
      file.close(); // Close the log file

      // Reset the GNSS
      // Note: this method is DEPRECATED. TO DO: replace this with UBX-CFG-VALDEL ?
      gnss.factoryDefault(2100);
      gnss.factoryReset();

      // Wait 5 secs
      Serial.print(F("GNSS has been reset. Waiting 5 seconds."));
      for (int i = 0; i < 5; i++)
      {
        for (int j = 0; j < 1000; j++)
        {
          delay(1);
        }
        Serial.print(F("."));
      }
    }
  }
}

// Disable all logable messages
uint8_t disableMessages(uint16_t maxWait)
{
  uint8_t success = true;
  success &= gnss.newCfgValset8(0x20910065, 0, VAL_LAYER_RAM); // CFG-MSGOUT-UBX_NAV_CLOCK_I2C (in RAM only)
  success &= gnss.addCfgValset8(0x2091002e, 0);                // CFG-MSGOUT-UBX_NAV_HPPOSECEF_I2C
  success &= gnss.addCfgValset8(0x20910033, 0);                // CFG-MSGOUT-UBX_NAV_HPPOSLLH_I2C
  success &= gnss.addCfgValset8(0x2091007e, 0);                // CFG-MSGOUT-UBX_NAV_ODO_I2C
  success &= gnss.addCfgValset8(0x20910024, 0);                // CFG-MSGOUT-UBX_NAV_POSECEF_I2C
  success &= gnss.addCfgValset8(0x20910029, 0);                // CFG-MSGOUT-UBX_NAV_POSLLH_I2C
  success &= gnss.addCfgValset8(0x20910006, 0);                // CFG-MSGOUT-UBX_NAV_PVT_I2C
  success &= gnss.addCfgValset8(0x2091008d, 0);                // CFG-MSGOUT-UBX_NAV_RELPOSNED_I2C
  success &= gnss.addCfgValset8(0x2091001a, 0);                // CFG-MSGOUT-UBX_NAV_STATUS_I2C
  success &= gnss.addCfgValset8(0x2091005b, 0);                // CFG-MSGOUT-UBX_NAV_TIMEUTC_I2C
  success &= gnss.addCfgValset8(0x2091003d, 0);                // CFG-MSGOUT-UBX_NAV_VELECEF_I2C
  success &= gnss.addCfgValset8(0x20910042, 0);                // CFG-MSGOUT-UBX_NAV_VELNED_I2C
  success &= gnss.addCfgValset8(0x209102a4, 0);                // CFG-MSGOUT-UBX_RXM_RAWX_I2C
  success &= gnss.addCfgValset8(0x20910231, 0);                // CFG-MSGOUT-UBX_RXM_SFRBX_I2C
  success &= gnss.sendCfgValset8(0x20910178, 0, maxWait);      // CFG-MSGOUT-UBX_TIM_TM2_I2C
  if (success > 0)
  {
    if (settings.printMinorDebugMessages)
    {
      Serial.println(F("disableMessages: sendCfgValset was successful when disabling messages"));
    }
  }
  else if (maxWait > 0) // If maxWait was zero then we expect success to be false
  {
    if (settings.printMajorDebugMessages)
    {
      Serial.println(F("disableMessages: sendCfgValset failed when disabling messages"));
    }
  }
  return (success);
}

uint8_t enableMessages(uint16_t maxWait)
{
  // Enable the selected messages
  uint8_t success = true;
  success &= gnss.newCfgValset8(0x20910065, settings.sensor_uBlox.logUBXNAVCLOCK, VAL_LAYER_RAM);  // CFG-MSGOUT-UBX_NAV_CLOCK_I2C (RAM only)
  success &= gnss.addCfgValset8(0x2091002e, settings.sensor_uBlox.logUBXNAVHPPOSECEF);             // CFG-MSGOUT-UBX_NAV_HPPOSECEF_I2C
  success &= gnss.addCfgValset8(0x20910033, settings.sensor_uBlox.logUBXNAVHPPOSLLH);              // CFG-MSGOUT-UBX_NAV_HPPOSLLH_I2C
  success &= gnss.addCfgValset8(0x2091007e, settings.sensor_uBlox.logUBXNAVODO);                   // CFG-MSGOUT-UBX_NAV_ODO_I2C
  success &= gnss.addCfgValset8(0x20910024, settings.sensor_uBlox.logUBXNAVPOSECEF);               // CFG-MSGOUT-UBX_NAV_POSECEF_I2C
  success &= gnss.addCfgValset8(0x20910029, settings.sensor_uBlox.logUBXNAVPOSLLH);                // CFG-MSGOUT-UBX_NAV_POSLLH_I2C
  success &= gnss.addCfgValset8(0x20910006, settings.sensor_uBlox.logUBXNAVPVT);                   // CFG-MSGOUT-UBX_NAV_PVT_I2C
  success &= gnss.addCfgValset8(0x2091001a, settings.sensor_uBlox.logUBXNAVSTATUS);                // CFG-MSGOUT-UBX_NAV_STATUS_I2C
  success &= gnss.addCfgValset8(0x2091005b, settings.sensor_uBlox.logUBXNAVTIMEUTC);               // CFG-MSGOUT-UBX_NAV_TIMEUTC_I2C
  success &= gnss.addCfgValset8(0x2091003d, settings.sensor_uBlox.logUBXNAVVELECEF);               // CFG-MSGOUT-UBX_NAV_VELECEF_I2C
  success &= gnss.addCfgValset8(0x20910042, settings.sensor_uBlox.logUBXNAVVELNED);                // CFG-MSGOUT-UBX_NAV_VELNED_I2C
  success &= gnss.addCfgValset8(0x20910231, settings.sensor_uBlox.logUBXRXMSFRBX);                 // CFG-MSGOUT-UBX_RXM_SFRBX_I2C

  if (minfo.HPG || minfo.HDG || minfo.ADR || minfo.LAP)
  {
    success &= gnss.addCfgValset8(0x2091008d, settings.sensor_uBlox.logUBXNAVRELPOSNED); // CFG-MSGOUT-UBX_NAV_RELPOSNED_I2C
  }
  if (minfo.HPG || minfo.TIM || minfo.FTS) // TO DO: I'm guessing that FTS supports RAWX!
  {
    success &= gnss.addCfgValset8(0x209102a4, settings.sensor_uBlox.logUBXRXMRAWX); // CFG-MSGOUT-UBX_RXM_RAWX_I2C
  }
  success &= gnss.sendCfgValset8(0x20910178, settings.sensor_uBlox.logUBXTIMTM2, maxWait); // CFG-MSGOUT-UBX_TIM_TM2_I2C
  if (success > 0)
  {
    if (settings.printMinorDebugMessages)
    {
      Serial.println(F("enableMessages: sendCfgValset was successful when enabling messages"));
    }
  }
  else if (maxWait > 0) // If maxWait was zero then we expect success to be false
  {
    if (settings.printMajorDebugMessages)
    {
      Serial.println(F("enableMessages: sendCfgValset failed when enabling messages"));
    }
  }
  return (success);
}

uint8_t enableConstellations(uint16_t maxWait)
{
  // Enable the selected constellations
  uint8_t success = true;
  success &= gnss.newCfgValset8(0x1031001f, settings.sensor_uBlox.enableGPS, VAL_LAYER_RAM);  // CFG-SIGNAL-GPS_ENA   : Enable GPS (in RAM only)
  success &= gnss.addCfgValset8(0x2091003d, settings.sensor_uBlox.enableGLO);                 // CFG-SIGNAL-GLO_ENA   : Enable GLONASS
  success &= gnss.addCfgValset8(0x20910042, settings.sensor_uBlox.enableGAL);                 // CFG-SIGNAL-GAL_ENA   : Enable Galileo
  success &= gnss.addCfgValset8(0x20910231, settings.sensor_uBlox.enableBDS);                 // CFG-SIGNAL-BDS_ENA   : Enable BeiDou
  success &= gnss.sendCfgValset8(0x20910231, settings.sensor_uBlox.enableQZSS, maxWait);      // CFG-SIGNAL-QZSS_ENA  : Enable QZSS (maxWait 2100 ms)
  if (success > 0)
  {
    if (settings.printMinorDebugMessages)
    {
      Serial.println(F("enableConstellations: sendCfgValset was successful when enabling constellations"));
    }
  }
  else if (maxWait > 0) // If maxWait was zero then we expect success to be false
  {
    if (settings.printMajorDebugMessages)
    {
      Serial.println(F("enableConstellations: sendCfgValset failed when enabling constellations"));
    }
  }
  return (success);
}

// Put the module to sleep for duration ms
uint8_t powerManagementTask(uint32_t duration, uint16_t maxWait)
{
  customCfg.cls = UBX_CLASS_RXM; // Message Class
  customCfg.id = UBX_RXM_PMREQ;  // Message ID
  customCfg.len = 16;            // Length)
  customCfg.startingSpot = 0;    // Set the startingSpot to zero

  //Define the payload
  customPayload[0] = 0x00;                    // Message version (0x00 for this version)
  customPayload[1] = 0x00;                    // Reserved
  customPayload[2] = 0x00;                    // Reserved
  customPayload[3] = 0x00;                    // Reserved
  customPayload[4] = duration & 0xFF;         // Duration of the power management task
  customPayload[5] = (duration >> 8) & 0xFF;
  customPayload[6] = (duration >> 16) & 0xFF;
  customPayload[7] = (duration >> 24) & 0xFF;
  customPayload[8] = 0x02;                    // Flags : set the backup bit (leave the force bit clear - module will stay on if USB is connected)
  customPayload[9] = 0x00;                    // Flags
  customPayload[10] = 0x00;                   // Flags
  customPayload[11] = 0x00;                   // Flags
  customPayload[12] = 0x00;                   // Disable the wakeup sources
  customPayload[13] = 0x00;                   // Wakeup sources
  customPayload[14] = 0x00;                   // Wakeup sources
  customPayload[15] = 0x00;                   // Wakeup sources

  // Send the command
  // UBX_RXM_PMREQ does not ACK so we expect the return value to be false
  return ((gnss.sendCommand(&customCfg, maxWait) == SFE_UBLOX_STATUS_DATA_SENT));
}

// If certain devices are attached, we need to reduce the I2C max speed
void determineMaxI2CSpeed()
{
  uint32_t maxSpeed = 400000; // Assume 400 kHz - but beware! 400 kHz with no pullups can cause issues.

  // If user wants to limit the I2C bus speed, do it here
  if (maxSpeed > settings.qwiicBusMaxSpeed)
  {
    maxSpeed = settings.qwiicBusMaxSpeed;
  }
  qwiic.setClock(maxSpeed);
}
