// Configure the real-time clock (RTC)
void setInitialAlarm()
{
  // Alarm modes:
  // 0: Alarm interrupt disabled
  // 1: Alarm match hundredths, seconds, minutes, hour, day, month  (every year)
  // 2: Alarm match hundredths, seconds, minutes, hours, day        (every month)
  // 3: Alarm match hundredths, seconds, minutes, hours, weekday    (every week)
  // 4: Alarm match hundredths, seconds, minutes, hours             (every day)
  // 5: Alarm match hundredths, seconds, minutes                    (every hour)
  // 6: Alarm match hundredths, seconds                             (every minute)
  // 7: Alarm match hundredths                                      (every second)

  // Debugging only: Manually set the RTC date and time
  //rtc.setTime(23, 55, 50, 0, 23, 6, 23); // hour, minutes, seconds, hundredths, day, month, year

  // Check for operation mode
  if (operationMode == 1) // Daily mode
  {
    // Set alarm mode
    initialAlarmMode = 4; // Alarm match hundredths, seconds, minutes, hours

    // Set initial alarm for specified hour
    rtc.setAlarm(startHour % 24, startMinute % 60, 0, 0, 0, 0);

    // Set the alarm mode
    rtc.setAlarmMode(initialAlarmMode);

    // Clear flag
    alarmFlag = false;
  }
  else if (operationMode == 2) // Rolling mode
  {
    // Set alarm mode
    initialAlarmMode = 5;  // Alarm match hundredths, seconds, minutes

    // Set initial alarm for rollover of next hour
    rtc.setAlarm(0, 0, 0, 0, 0, 0);

    // Set the alarm mode
    rtc.setAlarmMode(initialAlarmMode);

    // Clear flag
    alarmFlag = false;
  }
  else if (operationMode == 3) // Continuous mode
  {
    // Set alarm mode
    initialAlarmMode = 4; // Alarm match hundredths, seconds, minutes, hours

    // Set initial alarm for rollover of next day (00:00:00 UTC)
    rtc.setAlarm(0, 0, 0, 0, 0, 0);

    // Set the alarm mode
    rtc.setAlarmMode(initialAlarmMode);

    // Set flag
    alarmFlag = true;
  }

  // Attach alarm interrupt
  rtc.attachInterrupt();

  // Clear the RTC alarm interrupt
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  DEBUG_PRINT(F("Info - Initial alarm mode: "));  DEBUG_PRINTLN(initialAlarmMode);
}

// Read the RTC
void readRtc()
{
  // Start the loop timer
  unsigned long loopStartTime = micros();

  // Get the RTC's current date and time
  rtc.getTime();

  //DEBUG_PRINT("Info - Unix epoch time "); DEBUG_PRINTLN(rtc.getEpoch());

  // Stop the loop timer
  timer.rtc = micros() - loopStartTime;
}

// Set RTC alarm
void setSleepAlarm()
{
  // Clear RTC alarm interrupt
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Check for logging mode
  if (operationMode == 1) // Daily mode
  {
    DEBUG_PRINTLN(F("Info - Setting daily RTC alarm"));

    // Set alarm mode
    sleepAlarmMode = 4; // Alarm match hundredths, seconds, minutes, hours

    // Set daily alarm
    rtc.setAlarm(startHour, startMinute, 0, 0, 0, 0);

    // Set RTC alarm mode
    rtc.setAlarmMode(sleepAlarmMode); // Alarm match hundredths, seconds, minute, hours

    // Clear alarm flag
    alarmFlag = false;
  }
  else if (operationMode == 2) // Rolling mode
  {
    DEBUG_PRINTLN(F("Info - Setting rolling RTC alarm"));

    // Set rolling RTC alarm
    rtc.setAlarm((rtc.hour + sleepAlarmHours) % 24, (rtc.minute + sleepAlarmMinutes) % 60, 0, 0, rtc.dayOfMonth, rtc.month);

    // Adjust alarm mode based on duration of rolling alarm
    if (sleepAlarmHours > 0)
    {
      sleepAlarmMode = 4; // Alarm match on hundredths, seconds, minutes, hours
    }
    else
    {
      sleepAlarmMode = 5; // Alarm match on hundredths, seconds, minutes
    }

    // Set RTC alarm mode
    rtc.setAlarmMode(sleepAlarmMode);

    // Clear alarm flag
    alarmFlag = false;
  }
  else if (operationMode == 3) // Continuous mode
  {
    DEBUG_PRINTLN(F("Info - Continuous logging enabled"));
    return; // Skip setting alarm
  }
  else
  {
    alarmFlag = true; // Default if no logging mode set
  }

  // Print the next RTC alarm date and time
  DEBUG_PRINT("Info - Current time "); printDateTime();
  DEBUG_PRINT("Info - Sleeping until "); printAlarm();
}

// Set logging duration alarm
void setAwakeAlarm()
{
  // Clear the RTC alarm interrupt
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Check for logging mode
  if (operationMode == 1) // Daily mode
  {
    // Set daily RTC alarm
    rtc.setAlarm(stopHour, stopMinute, 0, 0, 0, 0);

    // Set RTC alarm mode
    rtc.setAlarmMode(4); // Alarm match on hundredths, seconds,  minutes, hours
  }
  else if (operationMode == 2) // Rolling mode
  {
    // Set rolling RTC alarm
    rtc.setAlarm((rtc.hour + awakeAlarmHours) % 24, (rtc.minute + awakeAlarmMinutes) % 60, 0, 0, rtc.dayOfMonth, rtc.month);

    // Set RTC alarm mode
    rtc.setAlarmMode(loggingAlarmMode);
  }
  else if (operationMode == 3) // Continuous mode
  {
    DEBUG_PRINTLN(F("Info - Continuous logging enabled"));
    return; // Skip setting alarm
  }

  // Clear alarm flag
  alarmFlag = false;

  // Print the next RTC alarm date and time
  DEBUG_PRINT("Info - Logging until "); printAlarm();
}

// Get the RTC's date and time and store it in a buffer
void getDateTime()
{
  rtc.getTime(); // Get the RTC's date and time
  sprintf(dateTimeBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds, rtc.hundredths);
}

// Print the RTC's date and time
void printDateTime()
{
  rtc.getTime(); // Get the RTC's date and time
  sprintf(dateTimeBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds, rtc.hundredths);
  DEBUG_PRINTLN(dateTimeBuffer);
}

// Print the RTC's alarm
void printAlarm()
{
  rtc.getAlarm(); // Get the RTC's alarm date and time
  char alarmBuffer[25];
  sprintf(alarmBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.alarmMonth, rtc.alarmDayOfMonth,
          rtc.alarmHour, rtc.alarmMinute, rtc.alarmSeconds, rtc.alarmHundredths);
  DEBUG_PRINTLN(alarmBuffer);
}
