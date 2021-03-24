// Configure the real-time clock (RTC)
void configureRtc()
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
  //rtc.setTime(12, 59, 50, 0, 1, 11, 21); // 2021-11-01 12:59:50.000 (hour, minutes, seconds, hundredths, day, month, year)

  // Clear the RTC alarm interrupt
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Get time before starting rolling alarm
  //rtc.getTime();

  // Set initial rolling alarm
  //rtc.setAlarm((rtc.hour + sleepAlarmHours) % 24, (rtc.minute + sleepAlarmMinutes) % 60, 0, 0, rtc.dayOfMonth, rtc.month);

  // Set initial rollover alarm
  rtc.setAlarm(0, 0, 0, 0, 0, 0);

  // Set the alarm mode
  rtc.setAlarmMode(initialAlarmMode);

  // Attach alarm interrupt
  rtc.attachInterrupt();

  // Clear the RTC alarm interrupt
  //rtc.clearInterrupt();
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);
}

// Read the real-time clock
void readRtc()
{
  // Get the RTC's current date and time
  rtc.getTime();
}

// Set RTC alarm
void setSleepAlarm()
{
  // Clear the RTC alarm interrupt
  //rtc.clearInterrupt();
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Set RTC  alarm
  rtc.setAlarm((rtc.hour + sleepAlarmHours) % 24, (rtc.minute + sleepAlarmMinutes) % 60, 0, 0, rtc.dayOfMonth, rtc.month);

  // Set the RTC alarm mode
  rtc.setAlarmMode(5); // Alarm match on hundredths, seconds, minutes, hours

  // Print the next RTC alarm date and time
  Serial.print("Info: Current time "); printDateTime();
  Serial.print("Info: Sleeping until "); printAlarm();
}

void setLoggingAlarm()
{
  // Clear the RTC alarm interrupt
  //rtc.clearInterrupt();
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  rtc.setAlarm((rtc.hour + loggingAlarmHours) % 24, (rtc.minute + loggingAlarmMinutes) % 60, 0, 0, rtc.dayOfMonth, rtc.month);

  // Set the RTC alarm mode
  rtc.setAlarmMode(loggingAlarmMode); // Alarm match on hundredths, seconds,  minutes, hours

  // Print the next RTC alarm date and time
  //Serial.print("Info: Current time "); printDateTime();
  Serial.print("Info: Logging until "); printAlarm();
}

// Print the RTC's date and time
void printDateTime()
{
  rtc.getTime(); // Get the RTC's date and time
  char dateTimeBuffer[25];
  sprintf(dateTimeBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds, rtc.hundredths);
  Serial.println(dateTimeBuffer);
}

// Print the RTC's alarm
void printAlarm()
{
  rtc.getAlarm(); // Get the RTC's alarm date and time
  char alarmBuffer[25];
  sprintf(alarmBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.alarmMonth, rtc.alarmDayOfMonth,
          rtc.alarmHour, rtc.alarmMinute, rtc.alarmSeconds, rtc.alarmHundredths);
  Serial.println(alarmBuffer);
}
