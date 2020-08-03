// Data logging menu
void menuLogging()
{
  while (1)
  {
    Serial.println();
    Serial.println(F("Menu: Configure Logging"));
    Serial.print(F("1) Log to microSD                                         : "));
    if (settings.logData)
    {
      Serial.println(F("Enabled"));
    }
    else
    {
      Serial.println(F("Disabled"));
    }
    Serial.print(F("2) Log to Terminal                                        : "));
    if (settings.enableTerminalOutput)
    {
      Serial.println(F("Enabled"));
    }
    else
    {
      Serial.println(F("Disabled"));
    }
    Serial.print(F("3) Set Serial Baud Rate                                   : "));
    Serial.print(settings.serialTerminalBaudRate);
    Serial.println(F(" bps"));

    Serial.print(F("4) Set Log Rate in Hz                                     : "));
    if (settings.usBetweenReadings < 1000000ULL) // Take more than one measurement per second
    {
      // Display Integer Hertz
      int logRate = (int)(1000000ULL / settings.usBetweenReadings);
      Serial.printf("%d\n", logRate);
    }
    else
    {
      // Display fractional Hertz
      uint32_t logRateSeconds = (uint32_t)(settings.usBetweenReadings / 1000000ULL);
      Serial.printf("%.06lf\n", 1.0 / logRateSeconds);
    }

    Serial.print(F("5) Set Log Rate in seconds between readings               : "));
    if (settings.usBetweenReadings > 1000000ULL) // Take more than one measurement per second
    {
      uint32_t interval = (uint32_t)(settings.usBetweenReadings / 1000000ULL);
      Serial.printf("%d\n", interval);
    }
    else
    {
      float rate = (float)(settings.usBetweenReadings / 1000000.0);
      Serial.printf("%.06f\n", rate);
    }

    Serial.print(F("6) Set logging duration in seconds                        : "));
    Serial.printf("%llu\n", settings.usLoggingDuration / 1000000ULL);

    Serial.print(F("7) Set sleep duration in seconds (0 = continuous logging) : "));
    Serial.printf("%llu\n", settings.usSleepDuration / 1000000ULL);

    Serial.print(F("8) Open new log file after sleep                          : "));
    if (settings.openNewLogFile)
    {
      Serial.println(F("Yes"));
    }
    else
    {
      Serial.println(F("No"));
    }

    Serial.print(F("9) Use pin 32 to Stop Logging                             : "));
    if (settings.useGPIO32ForStopLogging)
    {
      Serial.println("Yes");
    }
    else
    {
      Serial.println("No");
    }
    Serial.println(F("x) Exit"));

    byte incoming = getByteChoice(menuTimeout); // Timeout after x seconds

    if (incoming == '1')
    {
      settings.logData ^= 1;
    }
    else if (incoming == '2')
    {
      settings.enableTerminalOutput ^= 1;
    }
    else if (incoming == '3')
    {
      Serial.print(F("Enter baud rate (1200 to 500000): "));
      int newBaud = getNumber(menuTimeout); // Timeout after x seconds
      if (newBaud < 1200 || newBaud > 500000)
      {
        Serial.println(F("Error: baud rate out of range"));
      }
      else
      {
        settings.serialTerminalBaudRate = newBaud;
        recordSettings();
        Serial.printf("Terminal now set at %dbps. Please reset device and open terminal at new baud rate. Freezing...\n", settings.serialTerminalBaudRate);
        while (1);
      }
    }
    else if (incoming == '4')
    {
      float rateLimit = 1.0 / (((float)settings.sensor_uBlox.minMeasIntervalGps) / 1000.0);
      int maxOutputRate = (int)rateLimit;

      Serial.printf("How many readings per second would you like to log? (Current max is %d): ", maxOutputRate);
      int tempRPS = getNumber(menuTimeout); // Timeout after x seconds
      if (tempRPS < 1 || tempRPS > maxOutputRate)
      {
        Serial.println(F("Error: Readings Per Second out of range"));
      }
      else
      {
        settings.usBetweenReadings = 1000000UL / tempRPS;
      }

      gnssSettingsFlag = true; // Mark GNSS settings as changed so it will be started with new settings
      //qwiicOnline.uBlox = false; // Mark as offline so it will be started with new settings
    }
    else if (incoming == '5')
    {
      Serial.println(F("How many seconds between readings? (1 to 129,600):"));
      uint64_t tempSeconds = getNumber(menuTimeout); //Timeout after x seconds
      if (tempSeconds < 1 || tempSeconds > 129600ULL)
        Serial.println(F("Error: Readings Per Second out of range"));
      else
        //settings.recordPerSecond = tempRPS;
        settings.usBetweenReadings = 1000000ULL * tempSeconds;

      gnssSettingsFlag = true; // Mark GNSS settings as changed so it will be started with new settings
      //qwiicOnline.uBlox = false; // Mark as offline so it will be started with new settings
    }
    else if (incoming == '6')
    {
      uint64_t secsBetweenReads = settings.usBetweenReadings / 1000000ULL;
      if (secsBetweenReads < 5) secsBetweenReads = 5; // Let's be sensible about this. The module will take ~2 secs to do a hot start anyway.
      Serial.printf("How many seconds would you like to log for? (%d to 129,600):", secsBetweenReads);
      uint64_t tempSeconds = getNumber(menuTimeout); // Timeout after x seconds
      if ((tempSeconds < secsBetweenReads) || tempSeconds > 129600ULL)
      {
        Serial.println(F("Error: Logging Duration out of range"));
      }
      else
      {
        settings.usLoggingDuration = 1000000ULL * tempSeconds;
      }
    }
    else if (incoming == '7')
    {
      // The deep sleep duration is set with am_hal_stimer_compare_delta_set, the duration of which is uint32_t
      // The maximum sleep duration is 2^32 / 32768 = 131072 seconds = 36.4 hours
      // Limited to 36 hours = 129600 seconds
      Serial.println(F("How many seconds would you like to sleep for after logging? (0  or  10 to 129,600):"));
      uint64_t tempSeconds = getNumber(menuTimeout); // Timeout after x seconds
      if (((tempSeconds > 0) && (tempSeconds < 10)) || tempSeconds > 129600ULL)
      {
        Serial.println(F("Error: Sleep Duration out of range"));
      }
      else
      {
        settings.usSleepDuration = 1000000ULL * tempSeconds;
      }
    }
    else if (incoming == '8')
    {
      settings.openNewLogFile ^= 1;
    }
    else if (incoming == '9')
    {
      if (settings.useGPIO32ForStopLogging)
      {
        settings.useGPIO32ForStopLogging = false; // Disable stop logging
        detachInterrupt(digitalPinToInterrupt(PIN_STOP_LOGGING)); // Disable the interrupt
        pinMode(PIN_STOP_LOGGING, INPUT); // Remove the pull-up
      }
      else
      {
        settings.useGPIO32ForStopLogging = true; // Enable stop logging
        pinMode(PIN_STOP_LOGGING, INPUT_PULLUP);
        delay(1); // Let the pin stabilize
        attachInterrupt(digitalPinToInterrupt(PIN_STOP_LOGGING), stopLoggingIsr, FALLING); // Enable the interrupt
        stopLoggingFlag = false; // Ensure the flag is clear
      }
    }
    else if (incoming == 'x')
    {
      return;
    }
    else if (incoming == STATUS_GETBYTE_TIMEOUT)
    {
      return;
    }
    else
    {
      printUnknown(incoming);
    }
  }
}
