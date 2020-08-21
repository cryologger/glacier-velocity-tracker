// Checks EEPROM for the next available log file name
// Updates EEPROM and appends to the new log file
char* findNextAvailableLog(int &newFileNumber, const char *fileLeader)
{
  SdFile newFile; // Contain file for SD writing

  if (newFileNumber < 2) // If the settings have been reset, warn user of delay
  {
    Serial.println(F("Finding next available log file name"));
  }

  if (newFileNumber > 0)
  {
    newFileNumber--; // Check if last log file was empty
  }

  // Search for next available log spot
  static char newFileName[40];
  while (1)
  {
    sprintf(newFileName, "%s%05u.ubx", fileLeader, newFileNumber); // Splice the new file number into this file name

    if (settings.printMinorDebugMessages)
    {
      Serial.print(F("findNextAvailableLog: trying "));
      Serial.println(newFileName);
    }

    if (sd.exists(newFileName))
    {
      break; // File name not found so we will use it
    }

    // File exists so open and see if it is empty. If so, use it
    newFile.open(newFileName, O_READ);
    if (newFile.fileSize() == 0)
    {
      break; // File is empty so we will use it
    }

    newFile.close(); // Close existing file
    newFileNumber++; // Try next number
  }
  newFile.close(); // Close new file

  newFileNumber++; // Increment so the next power up uses the next file #
  recordSettings(); // Record new file number to EEPROM and to config file (newFileNumber points to settings.nextDataLogNumber)

  Serial.print(F("Created log file: "));
  Serial.println(newFileName);

  return (newFileName);
}

// Create new log file
bool createLogFile()
{
  // Get RTC's current date and time
  rtc.getTime();

  // Create log file
  sprintf(fileName, "20%02d%02d%02d_%02d%02d%02d.ubx",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  if (file.open(fileName, O_CREAT | O_WRITE | O_EXCL))
  {
    Serial.print(F("Logging to file: ")); Serial.println(fileName);
  }
  else
  {
    Serial.println(F("Warning: Unable to create new log file"));
    online.dataLogging = false;
    return (false);
  }

  // Set the log file creation time
  updateDataFileCreate();

  return (true);
}

// Close the current log file and create a new log file
void openNewLogFile()
{
  if (settings.logData && online.microSd && online.dataLogging) // Check if logging
  {
    if (qwiicAvailable.uBlox && qwiicOnline.uBlox) // Check if u-blox is available and logging
    {
      // Disable all messages in RAM (maxWait 0)
      disableMessages(0);
      // A maxWait of zero indicates to not wait for ACK/NACK and success will always be false
      // sendCommand returns SFE_UBLOX_STATUS_SUCCESS not SFE_UBLOX_STATUS_DATA_SENT

      unsigned long pauseUntil = millis() + 2100UL; // Wait > 500 ms so we can be sure SD data is synced
      while (millis() < pauseUntil) // While pausing, continue writing data to SD
      {
        storeData(); // Read I2C data and write to SD
      }

      // Close the current log file
      Serial.print(F("Closing: ")); Serial.println(fileName);
      file.sync();
      updateDataFileAccess(); // Update file access and write timestamps
      file.close();

      createLogFile(); // Create a new log file

      // Re-enable selected messages in RAM (MaxWait 2100)
      enableMessages(2100);
    }
    else
    {
      
    }
  }
}

// Close the current log file
void closeLogFile()
{
  if (settings.logData && online.microSd && online.dataLogging) // Check if logging
  {
    if (qwiicAvailable.uBlox && qwiicOnline.uBlox) // Check if u-blox is available and logging
    {
      // Disable all messages in RAM (maxWait 0)
      disableMessages(0);
      // A maxWait of zero indicates to not wait for ACK/NACK and success will always be false
      // sendCommand returns SFE_UBLOX_STATUS_SUCCESS not SFE_UBLOX_STATUS_DATA_SENT

      unsigned long pauseUntil = millis() + 2100UL; // Wait > 500 ms to ensure SD data is synced
      while (millis() < pauseUntil) // While we are pausing, keep writing data to SD
      {
        storeData(); // Read I2C data and write to SD
      }

      // Close the current log file
      Serial.print(F("Closing: ")); Serial.println(fileName);
      file.sync();
      updateDataFileAccess(); // Update the file access and write timestamps
      file.close();
    }
  }
}

// Update the file create timestamp
void updateDataFileCreate()
{
  if (rtcSyncFlag) // Update the create timestamp if the RTC is valid
  {
    rtc.getTime(); // Get the RTC time
    bool result = file.timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
    if (settings.printMinorDebugMessages)
    {
      Serial.print(F("updateDataFileCreate: file.timestamp T_CREATE returned "));
      Serial.println(result);
    }
  }
}

// Update the log file's last access and write/modification date and time
void updateDataFileAccess()
{
  if (rtcSyncFlag) // Check if RTC date and time are valid
  {
    rtc.getTime();
    bool result = file.timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
    if (settings.printMinorDebugMessages)
    {
      Serial.print(F("updateDataFileAccess: file.timestamp T_ACCESS returned "));
      Serial.println(result);
    }
    result = file.timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth, rtc.hour, rtc.minute, rtc.seconds);
    if (settings.printMinorDebugMessages)
    {
      Serial.print(F("updateDataFileAccess: file.timestamp T_WRITE returned "));
      Serial.println(result);
    }
  }
}
