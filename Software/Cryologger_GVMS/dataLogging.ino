// Create new log file
bool createLogFile()
{
  // Get RTC's current date and time
  rtc.getTime();

  // Create a new folder
  sprintf(dirName, "20%02d%02d%02d", rtc.year, rtc.month, rtc.dayOfMonth);
  if (sd.mkdir(dirName))
  {
    Serial.print("Created folder: "); Serial.println(dirName);
  }
  else
  {
    Serial.println("Warning: Unable to create new folder");
  }

  // Create log file
  sprintf(fileName, "20%02d%02d%02d/%02d%02d%02d.ubx",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);

  if (file.open(fileName, O_CREAT | O_WRITE | O_EXCL))
  {
    Serial.print("Logging to file: "); Serial.println(fileName);
  }
  else
  {
    Serial.println("Warning: Unable to create new log file");
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
      Serial.print("Closing: "); Serial.println(fileName);
      file.sync();
      updateDataFileAccess(); // Update file access and write timestamps
      file.close();

      createLogFile(); // Create a new log file

      // Re-enable selected messages in RAM (MaxWait 2100)
      enableMessages(2100);
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
      Serial.print("Closing: "); Serial.println(fileName);
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
