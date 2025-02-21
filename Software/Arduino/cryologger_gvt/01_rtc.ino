/*
  RTC Module

  This module configures the real-time clock (RTC), sets alarms, and manages
  timekeeping. It handles scheduled logging, sleep cycles, and periodic
  synchronization with GNSS timestamps.

  ----------------------------------------------------------------------------
  Alarm Modes:
  ----------------------------------------------------------------------------
  0: Alarm interrupt disabled
  1: Alarm match hundredths, seconds, minutes, hour, day, month  (every year)
  2: Alarm match hundredths, seconds, minutes, hours, day        (every month)
  3: Alarm match hundredths, seconds, minutes, hours, weekday    (every week)
  4: Alarm match hundredths, seconds, minutes, hours             (every day)
  5: Alarm match hundredths, seconds, minutes                    (every hour)
  6: Alarm match hundredths, seconds                             (every minute)
  7: Alarm match hundredths                                      (every second)
*/

// Configure the real-time clock (RTC)
//
// Initializes the RTC and sets the date/time manually for debugging purposes.
void configureRtc() {
  // Set the RTC to a predefined date and time.
  // Format: rtc.setTime(hour, minutes, seconds, hundredths, day, month, year);
  //rtc.setTime(23, 57, 30, 0, 31, 5, 25);  // Example: 23:57:30.00 on August 31, 2025

  // Scenario 1: Power-on before the seasonal logging period
  //rtc.setTime(23, 52, 30, 0, 1, 5, 25);

  // Scenario 2: Power-on the day before to seasonal logging period season prior to daily logging period
  //rtc.setTime(23, 52, 30, 0, 31, 5, 25);

  // Scenario 3: Power-on the day before to seasonal logging period season after daily logging period
  //rtc.setTime(23, 57, 30, 0, 31, 5, 25);

  // Scenario 4: Power-on during the seasonal logging period season
  //rtc.setTime(23, 57, 30, 0, 31, 7, 25);

  // Scenario 5: Power-on after the seasonal logging period season
  //rtc.setTime(23, 57, 30, 0, 31, 10, 25);

  // Store the current operation mode for future reference.
  normalOperationMode = operationMode;

  // Attach an interrupt handler to the RTC.
  rtc.attachInterrupt();
}

// Set the RTC logging alarm.
//
// Ensures normal logging behavior while properly transitioning to seasonal mode if applicable.
void setLoggingAlarm() {
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);  // Clear any pending RTC alarm interrupts

  checkOperationMode();  // Determine the correct operation mode

  switch (operationMode) {
    case DAILY:
      DEBUG_PRINTLN(F("[RTC] Info: Setting daily logging alarm."));
      alarmModeLogging = 4;
      rtc.setAlarm(alarmStopHour, alarmStopMinute, 0, 0, 0, 0);
      break;

    case ROLLING:
      DEBUG_PRINTLN(F("[RTC] Info: Setting rolling logging alarm."));
      alarmModeLogging = (alarmAwakeHours > 0) ? 4 : 5;
      rtc.setAlarm((rtc.hour + alarmAwakeHours + ((rtc.minute + alarmAwakeMinutes) / 60)) % 24,
                   (rtc.minute + alarmAwakeMinutes) % 60,
                   0, 0, rtc.dayOfMonth, rtc.month);
      break;

    case CONTINUOUS:
      DEBUG_PRINTLN(F("[RTC] Info: Continuous logging mode active. New log files at 00:00:00"));
      rtc.setAlarm(0, 0, 0, 0, 0, 0);
      alarmModeLogging = 4;
      break;
  }

  rtc.setAlarmMode(alarmModeLogging);
  alarmFlag = false;
  DEBUG_PRINT(F("[RTC] Info: Logging until "));
  printAlarm();
}

// Set the RTC sleep alarm.
//
// Ensures normal sleep cycles while handling the seasonal logging mode transition correctly.
void setSleepAlarm() {
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);  // Clear any pending RTC alarm interrupts
  checkOperationMode();                      // Determine the correct operation mode

  switch (operationMode) {
    case DAILY:
      // If today is the last day before seasonal logging period mode and logging is finished, prepare for seasonal logging.
      if (isSeasonalLoggingPeriod() && isLastDayBeforeSeasonalLogging()) {
        rtc.setAlarm(0, 0, 0, 0, 0, 0);
        DEBUG_PRINTLN(F("[RTC] Info: Last day before seasonal logging period. Setting alarm to 00:00:00."));
        alarmModeSleep = 4;
      } else {
        // Otherwise, set the normal daily sleep alarm
        rtc.setAlarm(alarmStartHour, alarmStartMinute, 0, 0, 0, 0);
        DEBUG_PRINTLN(F("[RTC] Info: Setting normal daily sleep alarm."));
        alarmModeSleep = 4;
      }
      alarmFlag = false;
      break;

    case ROLLING:
      rtc.setAlarm((rtc.hour + alarmSleepHours + ((rtc.minute + alarmSleepMinutes) / 60)) % 24,
                   (rtc.minute + alarmSleepMinutes) % 60,
                   0, 0, rtc.dayOfMonth, rtc.month);
      alarmModeSleep = (alarmSleepHours > 0) ? 4 : 5;
      DEBUG_PRINTLN(F("[RTC] Info: Setting rolling sleep alarm."));
      alarmFlag = false;
      break;

    case CONTINUOUS:
      DEBUG_PRINTLN(F("[RTC] Info: Continuous logging mode active. No sleep alarm required."));
      alarmFlag = true;
      return;
  }

  rtc.setAlarmMode(alarmModeSleep);
  DEBUG_PRINT(F("[RTC] Info: Sleeping until "));
  printAlarm();
}

// Read the RTC time.
//
// Retrieves the current time from the RTC and records the time taken
// for profiling purposes.
void readRtc() {
  unsigned long loopStartTime = micros();
  rtc.getTime();
  timer.rtc = micros() - loopStartTime;
}

// Get RTC date and time.
//
// Reads the RTC's date and time and stores it in a buffer.
void getDateTime() {
  rtc.getTime();
  sprintf(dateTimeBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds);
}

// Print the RTC date and time.
//
// Formats the RTC's date and time into a readable string for debugging output.
void printDateTime() {
  rtc.getTime();
  sprintf(dateTimeBuffer, "20%02d-%02d-%02d %02d:%02d:%02d.%02d",
          rtc.year, rtc.month, rtc.dayOfMonth,
          rtc.hour, rtc.minute, rtc.seconds, rtc.hundredths);
  DEBUG_PRINTLN(dateTimeBuffer);
}

// Print the scheduled RTC alarm.
//
// Retrieves and prints the configured alarm time in a readable format.
void printAlarm() {
  rtc.getAlarm();
  char alarmBuffer[30];
  sprintf(alarmBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.alarmMonth, rtc.alarmDayOfMonth,
          rtc.alarmHour, rtc.alarmMinute, rtc.alarmSeconds);
  DEBUG_PRINTLN(alarmBuffer);
}

// Check the RTC date.
//
// Compares the stored date with the RTC’s current date to detect daily changes.
// Useful for detecting midnight rollover or other daily boundary events.
bool checkDate() {
  // Read the current RTC time to get the latest day-of-month
  rtc.getTime();
  dateNew = rtc.dayOfMonth;

  // Print debug info
  DEBUG_PRINT(F("[RTC] Info: Current date: "));
  DEBUG_PRINT(dateCurrent);
  DEBUG_PRINT(F(" New date: "));
  DEBUG_PRINTLN(dateNew);

  // If this is the very first usage of checkDate(), 
  // we do not consider it a "change" event yet.
  if (firstTimeFlag) {
    dateCurrent = dateNew; 
    return false;  
  }

  // Once firstTimeFlag is false, we compare new vs. current
  if (dateNew != dateCurrent) {
    dateCurrent = dateNew;
    return true;  // Day changed
  }

  return false;   // No change
}

// Determines whether the specified RTC year (offset from 2000) is a leap year.
//  Example: If rtc.year == 25, then the full year is 2025.
bool isLeapYear(int rtcYear) {
  int fullYear = 2000 + rtcYear;
  if ((fullYear % 400) == 0) return true;
  if ((fullYear % 100) == 0) return false;
  if ((fullYear % 4) == 0) return true;
  return false;
}

// Converts the given (month/day) in the specified RTC year to a
//  "day-of-year" integer (1..365 or 1..366).
int dayOfYear(int rtcYear, int month, int day) {
  static const int daysBeforeMonth[12] = {
    0, 31, 59, 90, 120, 151,
    181, 212, 243, 273, 304, 334
  };

  int doy = daysBeforeMonth[month - 1] + day;
  if (isLeapYear(rtcYear) && month > 2) {
    doy += 1;
  }
  return doy;
}

// Checks if the current date is exactly the calendar day
// preceding the start of the seasonal window.
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

// Determines whether the current date/time falls within a seasonal
// logging window that may wrap around from one year to the next
// (e.g., from Nov 15 to Feb 15).
bool isSeasonalLoggingPeriod() {
  // Retrieve the current RTC date/time
  rtc.getTime();
  int rtcYear = rtc.year;
  int currentDOY = dayOfYear(rtcYear, rtc.month, rtc.dayOfMonth);

  // Compute day-of-year for start/end of seasonal window
  int startDOY = dayOfYear(rtcYear, alarmSeasonalStartMonth, alarmSeasonalStartDay);
  int endDOY = dayOfYear(rtcYear, alarmSeasonalEndMonth, alarmSeasonalEndDay);

  // If the start DOY <= end DOY, it's a "normal" window within the same year
  if (startDOY <= endDOY) {
    // Check if currentDOY is between [startDOY .. endDOY]
    return ((currentDOY >= startDOY) && (currentDOY <= endDOY));
  } else {
    // Otherwise, the window wraps around the new year
    // e.g., start=Nov 15 (319), end=Feb 15 (46).
    int maxDOY = isLeapYear(rtcYear) ? 366 : 365;

    // "In window" if currentDOY is from startDOY..Dec 31 (maxDOY)
    // OR from Jan 1..endDOY
    if (((currentDOY >= startDOY) && (currentDOY <= maxDOY)) || ((currentDOY >= 1) && (currentDOY <= endDOY))) {
      return true;
    }
    return false;
  }
}

// Check if daily logging is complete
bool isLastDayLoggingComplete() {
  rtc.getTime();
  return ((rtc.hour > alarmStopHour) || (rtc.hour == alarmStopHour && rtc.minute >= alarmStopMinute));
}

// Check and update the operation mode.
//
// This function determines the appropriate operation mode but does not set any alarms.
// Alarm scheduling is handled separately in `setLoggingAlarm()`.
void checkOperationMode() {

  DEBUG_PRINTLN(F("[RTC] Info: Checking operation mode..."));
  rtc.getTime();

  // If seasonal logging mode is enabled and the current date is within the seasonal
  // period, activate continuous logging mode.
  if (seasonalLoggingMode && isSeasonalLoggingPeriod()) {
    // If last day of logging has not yet complete, keep normal operation mode
    if (!isLastDayLoggingComplete()) {
      operationMode = normalOperationMode;
    } else {
      operationMode = CONTINUOUS;
    }
    DEBUG_PRINTLN(F("[RTC] Info: Operation mode = SEASONAL."));
  } else {
    // Otherwise, keep the normal operation mode.
    operationMode = normalOperationMode;
  }

  DEBUG_PRINT(F("[RTC] Info: Operation mode = "));
  if (operationMode == DAILY) DEBUG_PRINTLN(F("DAILY."));
  else if (operationMode == ROLLING) DEBUG_PRINTLN(F("ROLLING."));
  else if (operationMode == CONTINUOUS) DEBUG_PRINTLN(F("CONTINUOUS."));
  else DEBUG_PRINTLN(F("UNKNOWN."));
}