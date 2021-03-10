// Main configuration menu
void menuMain()
{
  // Disable debug messages while menu is open
  bool prevPrintMajorDebugMessages = settings.printMajorDebugMessages;
  bool prevPrintMinorDebugMessages = settings.printMinorDebugMessages;
  settings.printMajorDebugMessages = false;
  settings.printMinorDebugMessages = false;

  while (1)
  {
    Serial.println();
    Serial.println(F("Menu: Main Menu"));
    Serial.println(F("1) Configure Logging"));
    Serial.println(F("2) Configure RTC"));
    Serial.println(F("3) Configure GNSS"));
    Serial.println(F("4) Configure Qwiic Bus"));
    Serial.println(F("5) Configure Power Options"));

    if (settings.logData && online.microSd && online.dataLogging)
    {
      Serial.println(F("f) Open New Log File"));
    }

    if (qwiicAvailable.uBlox && qwiicOnline.uBlox)
    {
      Serial.println(F("g) Reset GNSS"));
    }

    Serial.println(F("r) Reset all OLA settings to default"));
    Serial.println(F("q) Quit: Close log files and power down"));
    Serial.println(F("d) Debug Menu"));
    Serial.println(F("x) Return to logging"));

    byte incoming = getByteChoice(menuTimeout); // Timeout after x seconds

    if (incoming == '1')
    {
      menuLogging();
    }
    else if (incoming == '2')
    {
      menuRtc();
    }
    else if (incoming == '3')
    {
      menuConfigureGnss();
    }
    else if (incoming == '4')
    {
      menuConfigure_QwiicBus();
    }
    else if (incoming == '5')
    {
      menuPower();
    }
    else if (incoming == 'f')
    {
      openNewLogFile();
    }
    else if (incoming == 'g')
    {
      Serial.println(F("\n\rResetting GNSS to factory defaults. Continue? Press 'y':"));
      byte gContinue = getByteChoice(menuTimeout);
      if (gContinue == 'y')
      {
        resetGnss();
        Serial.print(F("GNSS reset. Please reset OpenLog Artemis and open a terminal at "));
        Serial.print((String)settings.serialTerminalBaudRate);
        Serial.println(F("bps..."));
        while (1);
      }
      else
      {
        Serial.println(F("GNSS reset aborted"));
      }
    }
    else if (incoming == 'd')
    {
      menuDebug(&prevPrintMajorDebugMessages, &prevPrintMinorDebugMessages);
    }
    else if (incoming == 'r')
    {
      Serial.println(F("\n\rResetting to factory defaults. Continue? Press 'y':"));
      byte bContinue = getByteChoice(menuTimeout);
      if (bContinue == 'y')
      {
        EEPROM.erase();
        if (sd.exists("OLA_GNSS_settings.cfg"))
          sd.remove("OLA_GNSS_settings.cfg");

        Serial.print(F("Settings erased. Please reset OpenLog Artemis and open a terminal at "));
        Serial.print((String)settings.serialTerminalBaudRate);
        Serial.println(F("bps..."));
        while (1);
      }
      else
      {
        Serial.println(F("Reset aborted"));
      }
    }
    else if (incoming == 'q')
    {
      Serial.println(F("\n\rQuit? Press 'y' to confirm:"));
      byte bContinue = getByteChoice(menuTimeout);
      if (bContinue == 'y')
      {
        closeLogFile();
        Serial.print(F("Log files are closed. Please reset OpenLog Artemis and open a terminal at "));
        Serial.print((String)settings.serialTerminalBaudRate);
        Serial.println(F("bps..."));
        delay(sdPowerDownDelay); // Give the SD card time to shut down
        powerDown();
      }
      else
      {
        Serial.println(F("Quit aborted"));
      }
    }
    else if (incoming == 'x')
    {
      break;
    }
    else if (incoming == STATUS_GETBYTE_TIMEOUT)
    {
      break;
    }
    else
    {
      printUnknown(incoming);
    }
  }

  Serial.println(F("\nReturning to logging..."));

  // Restore debug messages
  settings.printMajorDebugMessages = prevPrintMajorDebugMessages;
  settings.printMinorDebugMessages = prevPrintMinorDebugMessages;

  recordSettings(); // Once all menus have exited, record the new settings to EEPROM and configuration file

  beginSensors(); // Once all menus have exited, start any sensors that are available, logging, but not yet online/begun.

  while (Serial.available())
  {
    Serial.read(); // Empty buffer of any newline chars
  }

  // If sleeping between readings we cannot rely on millis() as it is powered down. Used RTC instead.
  measurementStartTime = rtcMillis();
}

void menuConfigure_QwiicBus()
{
  while (1)
  {
    Serial.println();
    Serial.println(F("Menu: Configure Qwiic Bus"));

    Serial.print(F("1) Set Max Qwiic Bus Speed          : "));
    Serial.println(settings.qwiicBusMaxSpeed);
#if(HARDWARE_VERSION_MAJOR >= 1) || (HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 6)
    Serial.print(F("2) Turn off bus power when sleeping : "));
    if (settings.powerDownQwiicBusBetweenReads)
    {
      Serial.println(F("Yes"));
    }
    else
    {
      Serial.println(F("No"));
    }
#endif

    Serial.println(F("x) Exit"));

    byte incoming = getByteChoice(menuTimeout); //Timeout after x seconds

    if (incoming == '1')
    {
      Serial.print(F("Enter max frequency to run Qwiic bus: (100000 to 400000): "));
      int amt = getNumber(menuTimeout);
      if (amt >= 100000 && amt <= 400000)
      {
        settings.qwiicBusMaxSpeed = amt;
      }
      else
      {
        Serial.println(F("Error: Out of range"));
      }
    }
#if(HARDWARE_VERSION_MAJOR >= 1) || (HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 6)
    else if (incoming == '2')
    {
      settings.powerDownQwiicBusBetweenReads ^= 1;
    }
#endif
    else if (incoming == 'x')
    {
      break;
    }
    else if (incoming == STATUS_GETBYTE_TIMEOUT)
    {
      break;
    }
    else
    {
      printUnknown(incoming);
    }
  }
}
