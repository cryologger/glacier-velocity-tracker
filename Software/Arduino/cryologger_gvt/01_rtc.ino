/*
  RTC Module

  This module configures the real-time clock (RTC), sets alarms, and manages
  timekeeping. It handles scheduled logging, sleep cycles, and periodic
  synchronization with GNSS timestamps.

  -----------------------------------------------------------------------------
  Alarm Modes:
  -----------------------------------------------------------------------------
  0: Alarm interrupt disabled
  1: Alarm match hundredths, seconds, minutes, hour, day, month  (every year)
  2: Alarm match hundredths, seconds, minutes, hours, day        (every month)
  3: Alarm match hundredths, seconds, minutes, hours, weekday    (every week)
  4: Alarm match hundredths, seconds, minutes, hours             (every day)
  5: Alarm match hundredths, seconds, minutes                    (every hour)
  6: Alarm match hundredths, seconds                             (every minute)
  7: Alarm match hundredths                                      (every second)
*/

// ----------------------------------------------------------------------------
// Initializes the RTC and optionally sets the date/time for debugging.
// Attaches an interrupt handler for the RTC alarm ISR.
// ----------------------------------------------------------------------------
void configureRtc() {
  // Example usage:
  // rtc.setTime(hour, minute, second, hundredths, day, month, yearOffset);
  // e.g. rtc.setTime(23, 57, 30, 0, 31, 5, 25);

  // Scenario 1: Power-on before the seasonal logging period
  //rtc.setTime(16, 57, 30, 0, 1, 1, 25);

  // Scenario 2: Power-on day before seasonal logging period prior to daily logging period
  //rtc.setTime(16, 57, 30, 0, 31, 5, 25);

  // Scenario 3: Power-on day before seasonal logging period after daily logging period
  //rtc.setTime(23, 57, 30, 0, 31, 5, 25);

  // Scenario 4: Power-on during seasonal logging period
  //rtc.setTime(12, 0, 0, 0, 1, 7, 25);

  // Scenario 5: Power-on after seasonal logging period
  //rtc.setTime(16, 57, 30, 0, 1, 10, 25);

  // Save the initially selected operation mode (DAILY, ROLLING, etc.).
  normalOperationMode = operationMode;

  // Attach the RTC interrupt handler.
  rtc.attachInterrupt();
}
// ----------------------------------------------------------------------------
// Schedules the next logging period alarm based on the current operation mode.
// If in continuous mode, optionally set an alarm at 00:00:00 to roll log files
// at midnight. Otherwise, use the daily/rolling stop times.
// ----------------------------------------------------------------------------
void setLoggingAlarm() {
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);  // Clear pending RTC alarms

  // Update the operation mode first (DAILY, ROLLING, or CONTINUOUS).
  checkOperationMode();

  switch (operationMode) {
    case DAILY:
      DEBUG_PRINTLN("[RTC] Info: Setting daily logging alarm.");
      alarmModeLogging = 4;  // match hour/minute for daily stop.
      rtc.setAlarm(alarmStopHour, alarmStopMinute, 0, 0, 0, 0);
      break;

    case ROLLING:
      DEBUG_PRINTLN("[RTC] Info: Setting rolling logging alarm.");
      // If alarmAwakeHours > 0, we match daily hours; otherwise, just hourly.
      alarmModeLogging = (alarmAwakeHours > 0) ? 4 : 5;
      rtc.setAlarm((rtc.hour + alarmAwakeHours + ((rtc.minute + alarmAwakeMinutes) / 60)) % 24,
                   (rtc.minute + alarmAwakeMinutes) % 60,
                   0, 0, rtc.dayOfMonth, rtc.month);
      break;

    case CONTINUOUS:
      DEBUG_PRINTLN("[RTC] Info: Continuous logging mode. New file at midnight.");
      rtc.setAlarm(0, 0, 0, 0, 0, 0);  // e.g. Roll files at 00:00:00.
      alarmModeLogging = 4;
      break;
  }

  // Apply the chosen alarm mode in the RTC.
  rtc.setAlarmMode(alarmModeLogging);
  alarmFlag = false;

  DEBUG_PRINT("[RTC] Info: Logging until ");
  printAlarm();
}

// ----------------------------------------------------------------------------
// Schedules the next sleep period after logging is finished. If it's the last
// day before the seasonal window, we sleep until 00:00, ensuring we switch
// to continuous mode on the following day. Otherwise, we follow the normal
// daily or rolling schedule.
// ----------------------------------------------------------------------------
void setSleepAlarm() {
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);  // Clear pending RTC alarms.

  // Always update the operation mode first.
  checkOperationMode();

  switch (operationMode) {
    case DAILY:
      // On the last day before seasonal logging, sleep until midnight instead
      // of the normal daily start time.
      if (seasonalLoggingMode && isLastDayBeforeSeasonalLogging() && isLastDayLoggingComplete()) {
        rtc.setAlarm(0, 0, 0, 0, 0, 0);
        DEBUG_PRINTLN("[RTC] Info: Last day before seasonal. Sleeping until midnight.");
        alarmModeSleep = 4;
      } else {
        // Normal daily sleep until next day's start hour/minute.
        rtc.setAlarm(alarmStartHour, alarmStartMinute, 0, 0, 0, 0);
        DEBUG_PRINTLN("[RTC] Info: Setting normal daily sleep alarm.");
        alarmModeSleep = 4;
      }
      alarmFlag = false;
      break;

    case ROLLING:
      // Rolling sleep intervals.
      DEBUG_PRINTLN("[RTC] Info: Setting rolling sleep alarm.");
      rtc.setAlarm((rtc.hour + alarmSleepHours + ((rtc.minute + alarmSleepMinutes) / 60)) % 24,
                   (rtc.minute + alarmSleepMinutes) % 60,
                   0, 0, rtc.dayOfMonth, rtc.month);
      alarmModeSleep = (alarmSleepHours > 0) ? 4 : 5;
      alarmFlag = false;
      break;

    case CONTINUOUS:
      // In continuous mode, we don't go to sleep at all.
      DEBUG_PRINTLN("[RTC] Info: Continuous mode. No sleep alarm.");
      alarmFlag = true;  // signals immediate handling
      return;
  }

  // Set the chosen alarm mode and print.
  rtc.setAlarmMode(alarmModeSleep);
  DEBUG_PRINT("[RTC] Info: Sleeping until ");
  printAlarm();
}

// ----------------------------------------------------------------------------
// Retrieves the current RTC time and measures how long it takes for
// profiling/logging purposes.
// ----------------------------------------------------------------------------
void readRtc() {
  unsigned long loopStartTime = micros();
  rtc.getTime();
  timer.rtc = micros() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Retrieves the RTC date/time and stores it in dateTimeBuffer for later usage.
// This does not print; call printDateTime() to output to Serial.
// ----------------------------------------------------------------------------
void getDateTime() {
  rtc.getTime();
  snprintf(dateTimeBuffer, sizeof(dateTimeBuffer),
           "20%02lu-%02lu-%02lu %02lu:%02lu:%02lu",
           rtc.year, rtc.month, rtc.dayOfMonth,
           rtc.hour, rtc.minute, rtc.seconds);
}

// ---------------------------------------------------------------------------
// Reads and prints the current RTC date/time in a human-readable format.
// ---------------------------------------------------------------------------
void printDateTime() {
  rtc.getTime();
  snprintf(dateTimeBuffer, sizeof(dateTimeBuffer),
           "20%02lu-%02lu-%02lu %02lu:%02lu:%02lu",
           rtc.year, rtc.month, rtc.dayOfMonth,
           rtc.hour, rtc.minute, rtc.seconds);
  DEBUG_PRINTLN(dateTimeBuffer);
}

// ----------------------------------------------------------------------------
// Retrieves and prints the RTC's configured alarm time in a readable format.
// ----------------------------------------------------------------------------
void printAlarm() {
  rtc.getAlarm();
  char alarmBuffer[30];
  snprintf(alarmBuffer, sizeof(alarmBuffer),
           "20%02lu-%02lu-%02lu %02lu:%02lu:%02lu",
           rtc.year, rtc.alarmMonth, rtc.alarmDayOfMonth,
           rtc.alarmHour, rtc.alarmMinute, rtc.alarmSeconds);
  DEBUG_PRINTLN(alarmBuffer);
}

// ----------------------------------------------------------------------------
// Checks if the day-of-month has changed since last time. This helps detect
// midnight rollover or other daily boundary events. Returns true if changed.
// ----------------------------------------------------------------------------
bool checkDate() {
  rtc.getTime();
  dateNew = rtc.dayOfMonth;

  DEBUG_PRINT("[RTC] Info: Current date: ");
  DEBUG_PRINT(dateCurrent);
  DEBUG_PRINT(" New date: ");
  DEBUG_PRINTLN(dateNew);

  // If it's the first time, we only initialize and do not consider it a change.
  if (firstTimeFlag) {
    dateCurrent = dateNew;
    return false;
  }

  // If day-of-month has changed, update and return true
  if (dateNew != dateCurrent) {
    dateCurrent = dateNew;
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------------
// Determines if the given RTC year offset from 2000 is a leap year.
// Example: If rtc.year == 24, then it's 2024, which is a leap year.
// ----------------------------------------------------------------------------
bool isLeapYear(int rtcYear) {
  int fullYear = 2000 + rtcYear;
  if ((fullYear % 400) == 0) return true;
  if ((fullYear % 100) == 0) return false;
  if ((fullYear % 4) == 0) return true;
  return false;
}

// ----------------------------------------------------------------------------
// Converts a given (month/day) in the specified RTC year to a day-of-year
// integer (1..365/366). Accounts for leap years if needed.
// ----------------------------------------------------------------------------
int dayOfYear(int rtcYear, int month, int day) {
  static const int daysBeforeMonth[12] = {
    0, 31, 59, 90, 120, 151,
    181, 212, 243, 273, 304, 334
  };

  int doy = daysBeforeMonth[month - 1] + day;
  if (isLeapYear(rtcYear) && (month > 2)) {
    doy += 1;
  }
  return doy;
}

// ----------------------------------------------------------------------------
// Checks if today is the exact day before alarmSeasonalStartMonth/day. This
// allows setting a final 'sleep until midnight' so that the next day will
// be fully within the seasonal logging period.
// ----------------------------------------------------------------------------
bool isLastDayBeforeSeasonalLogging() {
  rtc.getTime();
  int rtcYear = rtc.year;
  int currentDOY = dayOfYear(rtcYear, rtc.month, rtc.dayOfMonth);

  // Calculate the day-of-year for the seasonal start date
  int startDOY = dayOfYear(rtcYear, alarmSeasonalStartMonth, alarmSeasonalStartDay);

  // The "day before" startDOY is simply (startDOY - 1).
  // If startDOY == 1, the "day before" is the last DOY of the PREVIOUS year (365 or 366).
  int dayBefore = startDOY - 1;
  if (dayBefore < 1) {
    // Wrap around to the previous year's last day-of-year
    // If we precisely track multi-year transitions, we'd check (rtcYear - 1).
    // For a repeating annual cycle, we often reuse 'rtcYear' for the day count:
    dayBefore = isLeapYear(rtcYear) ? 366 : 365;
  }

  // Return true if current DOY matches the "day before" value
  return (currentDOY == dayBefore);
}

// ----------------------------------------------------------------------------
// Determines if the current day-of-year is within the defined seasonal window,
// which may wrap around from late in one year to early in the next.
// ----------------------------------------------------------------------------
bool isSeasonalLoggingPeriod() {
  // Retrieve the current RTC date/time.
  rtc.getTime();
  int rtcYear = rtc.year;
  int currentDOY = dayOfYear(rtcYear, rtc.month, rtc.dayOfMonth);

  // Compute day-of-year for start/end of seasonal window
  int startDOY = dayOfYear(rtcYear, alarmSeasonalStartMonth, alarmSeasonalStartDay);
  int endDOY = dayOfYear(rtcYear, alarmSeasonalEndMonth, alarmSeasonalEndDay);

  // If the start DOY <= end DOY, it's a "normal" window within the same year.
  if (startDOY <= endDOY) {
    // Check if currentDOY is between [startDOY .. endDOY].
    return ((currentDOY >= startDOY) && (currentDOY <= endDOY));
  } else {
    // Otherwise, the window wraps around the new year
    // e.g., start=Nov 15 (319), end=Feb 15 (46).
    int maxDOY = isLeapYear(rtcYear) ? 366 : 365;

    // "In window" if currentDOY is from startDOY..Dec 31 (maxDOY)
    // OR from Jan 1..endDOY.
    if (((currentDOY >= startDOY) && (currentDOY <= maxDOY))
        || ((currentDOY >= 1) && (currentDOY <= endDOY))) {
      return true;
    }
    return false;
  }
}

// ----------------------------------------------------------------------------
// Checks if the current time is past the daily logging stop hour/minute.
// Useful if you only want to trigger certain actions after normal logging
// is finished for the day.
// ----------------------------------------------------------------------------
bool isLastDayLoggingComplete() {
  rtc.getTime();
  // e.g., if alarmStopHour=15, alarmStopMinute=0 => we check if hour>15 or
  // hour=15 & minute>=0
  return ((rtc.hour > alarmStopHour) || ((rtc.hour == alarmStopHour) && (rtc.minute >= alarmStopMinute)));
}

// ----------------------------------------------------------------------------
// Checks if the current date/time is within the seasonal logging period.
// If so, sets the operation mode to CONTINUOUS. Otherwise, reverts to the
// normal operation mode (DAILY, ROLLING, etc.). Does not set any alarms.
// ----------------------------------------------------------------------------
void checkOperationMode() {
  DEBUG_PRINTLN("[RTC] Info: Checking operation mode...");
  rtc.getTime();

  // Simple check of seasonal logging mode for debugging purposes.
  if (seasonalLoggingMode == ENABLED) {
    DEBUG_PRINTLN("[RTC] Info: Seasonal mode = ENABLED.");
  } else {
    DEBUG_PRINTLN("[RTC] Info: Seasonal mode = DISABLED.");
  }

  // If seasonal logging is enabled and it's currently the seasonal window,
  // switch to continuous mode. Otherwise, use normalOperationMode.
  if (seasonalLoggingMode == ENABLED && isSeasonalLoggingPeriod()) {
    operationMode = CONTINUOUS;
  } else {
    operationMode = normalOperationMode;
  }

  // Debug output for the chosen mode.
  DEBUG_PRINT("[RTC] Info: Operation mode = ");
  if (operationMode == DAILY) DEBUG_PRINTLN("DAILY.");
  else if (operationMode == ROLLING) DEBUG_PRINTLN("ROLLING.");
  else if (operationMode == CONTINUOUS) DEBUG_PRINTLN("CONTINUOUS.");
  else DEBUG_PRINTLN("UNKNOWN.");
}