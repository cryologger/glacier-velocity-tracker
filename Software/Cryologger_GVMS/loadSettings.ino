// Load settings
void loadSettings()
{
  // First load any settings from NVM
  // After, we'll load settings from config file if available
  // We'll then re-record settings so that the settings from the file over-rides internal NVM settings

  // Check to see if EEPROM is blank
  uint32_t testRead = 0;
  if (EEPROM.get(0, testRead) == 0xFFFFFFFF)
  {
    recordSettings(); // Record default settings to EEPROM and config file. At power on, settings are in default state
    Serial.println(F("Default settings applied"));
  }

  // Check that the current settings struct size matches what is stored in EEPROM
  // Misalignment happens when we add a new feature or setting
  int tempSize = 0;
  EEPROM.get(0, tempSize); // Load the sizeOfSettings
  if (tempSize != sizeof(settings))
  {
    Serial.println(F("Settings wrong size. Default settings applied"));
    recordSettings(); // Record default settings to EEPROM and config file. At power on, settings are in default state
  }

  // Check that the olaIdentifier is correct
  // (It is possible for two different versions of the code to have the same sizeOfSettings - which causes problems!)
  int tempIdentifier = 0;
  EEPROM.get(sizeof(int), tempIdentifier); // Load the identifier from the EEPROM location after sizeOfSettings (int)
  if (tempIdentifier != OLA_IDENTIFIER)
  {
    Serial.println(F("Settings are not valid for this variant of the OLA. Default settings applied"));
    recordSettings(); // Record default settings to EEPROM and config file. At power on, settings are in default state
  }

  // Read current settings
  EEPROM.get(0, settings);

  loadSettingsFromFile(); // Load any settings from config file. This will over-write any pre-existing EEPROM settings.
  // Record these new settings to EEPROM and config file to be sure they are the same
  // (do this even if loadSettingsFromFile returned false)
  recordSettings();
}

// Record the current settings struct to EEPROM and then to config file
void recordSettings()
{
  settings.sizeOfSettings = sizeof(settings);
  EEPROM.put(0, settings);
  recordSettingsToFile();
}

// Export the current settings to a config file
void recordSettingsToFile()
{
  if (online.microSd)
  {
    if (sd.exists("OLA_GNSS_settings.cfg"))
      sd.remove("OLA_GNSS_settings.cfg");

    SdFile settingsFile;
    if (!settingsFile.open("OLA_GNSS_settings.cfg", O_CREAT | O_APPEND | O_WRITE))
    {
      Serial.println(F("Failed to create settings file"));
      return;
    }

    settingsFile.println("sizeOfSettings=" + (String)settings.sizeOfSettings);
    settingsFile.println("olaIdentifier=" + (String)settings.olaIdentifier);
    settingsFile.println("nextDataLogNumber=" + (String)settings.nextDataLogNumber);

    // Convert uint64_t to string
    // Based on printLLNumber by robtillaart
    // https://forum.arduino.cc/index.php?topic=143584.msg1519824#msg1519824
    char tempTimeRev[20]; // Char array to hold to usBetweenReadings (reversed order)
    char tempTime[20]; // Char array to hold to usBetweenReadings (correct order)
    uint64_t usBR = settings.usBetweenReadings;
    unsigned int i = 0;
    if (usBR == 0ULL) // if usBetweenReadings is zero, set tempTime to "0"
    {
      tempTime[0] = '0';
      tempTime[1] = 0;
    }
    else
    {
      while (usBR > 0)
      {
        tempTimeRev[i++] = (usBR % 10) + '0'; // divide by 10, convert the remainder to char
        usBR /= 10; // divide by 10
      }
      unsigned int j = 0;
      while (i > 0)
      {
        tempTime[j++] = tempTimeRev[--i]; // Reverse the order
        tempTime[j] = 0; // Mark the end with a NULL
      }
    }

    settingsFile.println("usBetweenReadings=" + (String)tempTime);

    usBR = settings.usLoggingDuration;
    i = 0;
    if (usBR == 0ULL) // If usLoggingDuration is zero, set tempTime to "0"
    {
      tempTime[0] = '0';
      tempTime[1] = 0;
    }
    else
    {
      while (usBR > 0)
      {
        tempTimeRev[i++] = (usBR % 10) + '0'; // Divide by 10, convert the remainder to char
        usBR /= 10; // divide by 10
      }
      unsigned int j = 0;
      while (i > 0)
      {
        tempTime[j++] = tempTimeRev[--i]; // Reverse the order
        tempTime[j] = 0; // Mark the end with a NULL
      }
    }

    settingsFile.println("usLoggingDuration=" + (String)tempTime);

    usBR = settings.usSleepDuration;
    i = 0;
    if (usBR == 0ULL) // if usSleepDuration is zero, set tempTime to "0"
    {
      tempTime[0] = '0';
      tempTime[1] = 0;
    }
    else
    {
      while (usBR > 0)
      {
        tempTimeRev[i++] = (usBR % 10) + '0'; // divide by 10, convert the remainder to char
        usBR /= 10; // divide by 10
      }
      unsigned int j = 0;
      while (i > 0)
      {
        tempTime[j++] = tempTimeRev[--i]; // reverse the order
        tempTime[j] = 0; // mark the end with a NULL
      }
    }

    settingsFile.println("usSleepDuration=" + (String)tempTime);
    settingsFile.println("openNewLogFile=" + (String)settings.openNewLogFile);
    settingsFile.println("enableSd=" + (String)settings.enableSd);
    settingsFile.println("enableTerminalOutput=" + (String)settings.enableTerminalOutput);
    settingsFile.println("logData=" + (String)settings.logData);
    settingsFile.println("serialTerminalBaudRate=" + (String)settings.serialTerminalBaudRate);
    settingsFile.println("showHelperText=" + (String)settings.showHelperText);
    settingsFile.println("printMajorDebugMessages=" + (String)settings.printMajorDebugMessages);
    settingsFile.println("printMinorDebugMessages=" + (String)settings.printMinorDebugMessages);
    settingsFile.println("powerDownQwiicBusBetweenReads=" + (String)settings.powerDownQwiicBusBetweenReads);
    settingsFile.println("qwiicBusMaxSpeed=" + (String)settings.qwiicBusMaxSpeed);
    settingsFile.println("enablePwrLedDuringSleep=" + (String)settings.enablePwrLedDuringSleep);
    settingsFile.println("useGPIO32ForStopLogging=" + (String)settings.useGPIO32ForStopLogging);
    settingsFile.close();
  }
}

// If a config file exists on the SD card, load them and overwrite the local settings
// Heavily based on ReadCsvFile from SdFat library
// Returns true if some settings were loaded from a file
// Returns false if a file was not opened/loaded
bool loadSettingsFromFile()
{
  if (online.microSd)
  {
    if (sd.exists("OLA_GNSS_settings.cfg"))
    {
      SdFile settingsFile;
      if (!settingsFile.open("OLA_GNSS_settings.cfg", O_READ))
      {
        Serial.println(F("Failed to open settings file"));
        return (false);
      }

      char line[60];
      int lineNumber = 0;

      while (settingsFile.available())
      {
        int n = settingsFile.fgets(line, sizeof(line));
        if (n <= 0)
        {
          Serial.printf("Failed to read line %d from settings file\n", lineNumber);
        }
        else if (line[n - 1] != '\n' && n == (sizeof(line) - 1))
        {
          Serial.printf("Settings line %d too long\n", lineNumber);
          if (lineNumber == 0)
          {
            // If we can't read the first line of the settings file, give up
            Serial.println(F("Giving up on settings file"));
            return (false);
          }
        }
        else if (!parseLine(line))
        {
          Serial.printf("Failed to parse line %d: %s\n", lineNumber, line);
          if (lineNumber == 0)
          {
            // If we can't read the first line of the settings file, give up
            Serial.println(F("Giving up on settings file"));
            return (false);
          }
        }

        lineNumber++;
      }

      Serial.println(F("Config file read complete"));
      settingsFile.close();
      return (true);
    }
    else
    {
      Serial.println(F("No config file found. Using settings from EEPROM."));
      // The defaults of the struct will be recorded to a file later on.
      return (false);
    }
  }

  Serial.println(F("Config file read failed: SD offline"));
  return (false); // SD offline
}

// Check for extra characters in field or find minus sign.
char* skipSpace(char* str)
{
  while (isspace(*str)) str++;
  return str;
}

// Convert a given line from file into a settingName and value
// Sets the setting if the name is known
bool parseLine(char* str)
{
  char* ptr;

  // Debug
  //Serial.printf("Line contents: %s", str);
  //Serial.flush();

  // Set strtok start of line.
  str = strtok(str, "=");
  if (!str) return false;

  // Store this setting name
  char settingName[40];
  sprintf(settingName, "%s", str);

  // Move pointer to end of line
  str = strtok(nullptr, "\n");
  if (!str) return false;

  //Serial.printf("s = %s\n", str);
  //Serial.flush();

  // Convert string to double.
  double d = strtod(str, &ptr);
  if (str == ptr || *skipSpace(ptr)) return false;

  //Serial.printf("d = %lf\n", d);
  //Serial.flush();

  // Get setting name
  if (strcmp(settingName, "sizeOfSettings") == 0)
  {
    // We may want to cause a factory reset from the settings file rather than the menu
    // If user sets sizeOfSettings to -1 in config file, OLA will factory reset
    if (d == -1)
    {
      EEPROM.erase();
      sd.remove("OLA_GNSS_settings.cfg");
      Serial.println(F("OpenLog Artemis has been factory reset. Freezing. Please restart and open terminal at 115200bps."));
      while (1);
    }

    // Check if setting.h file is compatible with this version of OLA
    if (d != sizeof(settings))
    {
      Serial.printf("Warning: Settings size is %d but current firmware expects %d. Attempting to use settings from file.\n", d, sizeof(settings));
    }
  }
  else if (strcmp(settingName, "olaIdentifier") == 0)
    settings.olaIdentifier = d;
  else if (strcmp(settingName, "nextDataLogNumber") == 0)
    settings.nextDataLogNumber = d;
  else if (strcmp(settingName, "usBetweenReadings") == 0)
    settings.usBetweenReadings = d;
  else if (strcmp(settingName, "usLoggingDuration") == 0)
    settings.usLoggingDuration = d;
  else if (strcmp(settingName, "usSleepDuration") == 0)
    settings.usSleepDuration = d;
  else if (strcmp(settingName, "openNewLogFile") == 0)
    settings.openNewLogFile = d;
  else if (strcmp(settingName, "enableSd") == 0)
    settings.enableSd = d;
  else if (strcmp(settingName, "enableTerminalOutput") == 0)
    settings.enableTerminalOutput = d;
  else if (strcmp(settingName, "logData") == 0)
    settings.logData = d;
  else if (strcmp(settingName, "serialTerminalBaudRate") == 0)
    settings.serialTerminalBaudRate = d;
  else if (strcmp(settingName, "showHelperText") == 0)
    settings.showHelperText = d;
  else if (strcmp(settingName, "printMajorDebugMessages") == 0)
    settings.printMajorDebugMessages = d;
  else if (strcmp(settingName, "printMinorDebugMessages") == 0)
    settings.printMinorDebugMessages = d;
  else if (strcmp(settingName, "powerDownQwiicBusBetweenReads") == 0)
    settings.powerDownQwiicBusBetweenReads = d;
  else if (strcmp(settingName, "qwiicBusMaxSpeed") == 0)
    settings.qwiicBusMaxSpeed = d;
  else if (strcmp(settingName, "enablePwrLedDuringSleep") == 0)
    settings.enablePwrLedDuringSleep = d;
  else if (strcmp(settingName, "useGPIO32ForStopLogging") == 0)
    settings.useGPIO32ForStopLogging = d;
  else
  {
    Serial.print(F("Unknown setting: "));
    Serial.println(settingName);
  }

  return (true);
}
