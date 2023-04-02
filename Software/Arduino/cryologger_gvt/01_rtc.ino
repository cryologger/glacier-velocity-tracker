// Configure the real-time clock (RTC)
void setInitialAlarm()
{
  // Alarm modes:
  // 0: Alarm interrupt disabled
  // 1: Alarm match hundredths, seconds, minutes, hour, day, month  (every year)
  // 2: Alarm match hundredths, seconds, minutes, hours, day        (every month)
  // 3: Alarm match hundredths, seconds, minutes, hours, weekday    (every week)
  // 4: Alarm match hundredths, seconds, minute, hours              (every day)
  // 5: Alarm match hundredths, seconds, minutes                    (every hour)
  // 6: Alarm match hundredths, seconds                             (every minute)
  // 7: Alarm match hundredths                                      (every second)

  // Manually set the RTC date and time
  //rtc.setTime(14, 58, 50, 0, 24, 6, 22); // hour, minutes, seconds, hundredths, day, month, year

  if (loggingMode == 1)
  {
    // Set initial alarm
    rtc.setAlarm(loggingStartTime, 0, 0, 0, 0, 0);
  }
  else
  {
    // Set initial alarm
    rtc.setAlarm(0, 0, 0, 0, 0, 0);
  }

  // Set the alarm mode
  rtc.setAlarmMode(initialAlarmMode);

  // Attach alarm interrupt
  rtc.attachInterrupt();

  // Clear the RTC alarm interrupt
  //rtc.clearInterrupt(); // Apollo3 Core v2.x
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM); // Apollo3 Core v1.x

  // Initial alarm flag is dependent on logging mode
  if (loggingMode == 3)
  {
    alarmFlag = true; // Set flag
  }
  else
  {
    alarmFlag = false; // Clear flag
  }
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
  if (loggingMode == 1) // Daily mode
  {
    DEBUG_PRINTLN(F("Info - Setting daily RTC alarm"));

    // Set daily alarm
    rtc.setAlarm(loggingStartTime, 0, 0, 0, 0, 0);

    // Set RTC alarm mode
    rtc.setAlarmMode(sleepAlarmMode); // Alarm match

    // Clear alarm flag
    alarmFlag = false;
  }
  else if (loggingMode == 2) // Rolling mode
  {
    DEBUG_PRINTLN(F("Info - Setting rolling RTC alarm"));
    
    // Set rolling RTC alarm
    rtc.setAlarm((rtc.hour + sleepAlarmHours) % 24, (rtc.minute + sleepAlarmMinutes) % 60, 0, 0, rtc.dayOfMonth, rtc.month);

    // Set RTC alarm mode
    rtc.setAlarmMode(sleepAlarmMode); // Alarm match on hundredths, seconds, minutes, hours

    // Clear alarm flag
    alarmFlag = false;
  }
  else if (loggingMode == 3) // Continuous mode
  {
    DEBUG_PRINTLN(F("Info - Continuous logging enabled"));
    return; // Skip setting alarm
  }
  else
  {
    alarmFlag = true; // Default if no logging mode set
    return; // Skip setting alarm
  }

  // Print the next RTC alarm date and time
  DEBUG_PRINT("Info - Current time "); printDateTime();
  DEBUG_PRINT("Info - Sleeping until "); printAlarm();
}

// Set logging duration alarm
void setLoggingAlarm()
{
  // Clear the RTC alarm interrupt
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM); // Apollo3 Core v1.x

  // Check for logging mode
  if (loggingMode == 1) // Daily mode
  {
    // Set daily RTC alarm
    rtc.setAlarm(loggingStopTime, 0, 0, 0, 0, 0);

    // Set RTC alarm mode
    rtc.setAlarmMode(loggingAlarmMode); // Alarm match on hundredths, seconds,  minutes, hours
  }
  else if (loggingMode == 2) // Rolling mode
  {
    // Set rolling RTC alarm
    rtc.setAlarm((rtc.hour + loggingAlarmHours) % 24, (rtc.minute + loggingAlarmMinutes) % 60, 0, 0, rtc.dayOfMonth, rtc.month);

    // Set RTC alarm mode
    rtc.setAlarmMode(loggingAlarmMode);
  }
  else if (loggingMode == 3) // Continuous mode
  {
    // Set continuous RTC alarm
    rtc.setAlarm(0, 0, 0, 0, 0, 0); // hours, minutes, seconds, microseconds, day, month

    // Set RTC alarm mode
    rtc.setAlarmMode(4); // Alarm match hundredths, seconds, minute, hours (00:00:00 UTC)
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
