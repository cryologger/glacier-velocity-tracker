/*
  Display Module

  This module manages the OLED display, handling initialization, error messages, 
  and status updates. It provides real-time feedback on system status, logging 
  mode, and RTC synchronization.
*/

// Configure the OLED display.
//
// Initializes the OLED screen and verifies functionality. If the first
// attempt fails, it retries once before disabling the display.
void configureOled() {
  enablePullups();  // Enable internal I2C pull-ups

  // Initialize OLED display
  if (!oled.begin()) {
    online.oled = false;
    DEBUG_PRINTLN(F("[Display] Warning: OLED failed to initialize. Reattempting..."));

    // Delay before retrying
    myDelay(2000);

    if (!oled.begin()) {
      online.oled = false;
      DEBUG_PRINTLN(F("[Display] Warning: OLED initialization failed."));
    } else {
      lineTest();
      online.oled = true;
    }
  } else {
    lineTest();
    online.oled = true;
  }

  disablePullups();  // Disable internal I2C pull-ups
}

// Reset the OLED display after sleep/power cycle.
//
// This function resets the OLED screen after deep sleep or a power cycle
// to ensure proper initialization.
void resetOled() {
#if OLED
  myDelay(4000);
  online.oled = true;
  enablePullups();
  oled.reset(1);
#endif
}

// Display the welcome message.
//
// Shows the Cryologger version and battery voltage.
void displayWelcome() {
  if (!online.oled) return;

  enablePullups();
  oled.erase();
  oled.setCursor(0, 0);
  oled.print("Cryologger GVT ");
  oled.print(SERIAL);
  oled.setCursor(0, 10);
  oled.print(dateTimeBuffer);
  oled.setCursor(0, 20);
  oled.print("Voltage:");
  oled.setCursor(54, 20);
  oled.print(readBattery(), 2);
  oled.display();
  disablePullups();
  myDelay(4000);
}

// Display initialization message.
//
// Shows which device is currently being initialized.
void displayInitialize(char *device) {
  if (!online.oled) return;

  enablePullups();
  char displayBuffer[24];
  sprintf(displayBuffer, "Initialize %s...", device);
  oled.erase();
  oled.text(0, 0, displayBuffer);
  oled.display();
  disablePullups();
}

// Display success message.
void displaySuccess() {
  if (!online.oled) return;

  enablePullups();
  oled.text(0, 10, "Success!");
  oled.display();
  disablePullups();
  myDelay(2000);
}

// Display failure message.
void displayFailure() {
  if (!online.oled) return;

  enablePullups();
  oled.text(0, 10, "Failed!");
  oled.display();
  disablePullups();
}

// Display reattempt message.
void displayReattempt() {
  if (!online.oled) return;

  enablePullups();
  oled.text(0, 10, "Failed! Reattempting...");
  oled.display();
  disablePullups();
}

// Display setup completion message.
void displaySetupComplete() {
  if (!online.oled) return;

  enablePullups();
  oled.erase();
  oled.text(0, 0, "Setup complete!");
  oled.display();
  disablePullups();
  myDelay(2000);
}

// Display logging mode information.
//
// Shows the logging mode and its parameters.
void displayLoggingMode() {
  if (!online.oled) return;

  enablePullups();
  oled.erase();
  oled.setCursor(0, 0);
  oled.print("Log Mode: ");

  char displayBuffer1[32];
  char displayBuffer2[32];

  if (operationMode == 1) {
    sprintf(displayBuffer1, "Start time: %d:%d", alarmStartHour, alarmStartMinute);
    sprintf(displayBuffer2, "End time: %d:%d", alarmStopHour, alarmStopMinute);
    oled.print("Daily");
  } else if (operationMode == 2) {
    sprintf(displayBuffer1, "Log: %02d hrs %02d min", alarmAwakeHours, alarmAwakeMinutes);
    sprintf(displayBuffer2, "Sleep: %02d hrs %02d min", alarmSleepHours, alarmSleepMinutes);
    oled.print("Rolling");
  } else if (operationMode == 3) {
    oled.print("Continuous");
  } else {
    oled.print("Not specified!");
  }

  oled.text(0, 10, displayBuffer1);
  oled.text(0, 20, displayBuffer2);
  oled.display();
  disablePullups();
  myDelay(8000);
}

// Display RTC sync status.
void displayRtcSyncStatus() {
  if (!online.oled) return;

  enablePullups();
  oled.erase();
  oled.setCursor(0, 0);
  oled.print("Syncing RTC/GNSS...");
  oled.setCursor(0, 10);
  oled.print("Sat:");
  oled.setCursor(35, 10);
  oled.print(gnss.getSIV());
  oled.setCursor(55, 10);
  oled.print("Fix:");
  oled.setCursor(90, 10);
  oled.print(gnss.getFixType());
  oled.setCursor(0, 20);
  oled.print("Date:");
  oled.setCursor(35, 20);
  oled.print(gnss.getConfirmedDate());
  oled.setCursor(55, 20);
  oled.print("Time:");
  oled.setCursor(90, 20);
  oled.print(gnss.getConfirmedTime());
  oled.display();
  disablePullups();
}

// Display RTC sync failure.
void displayRtcFailure() {
  if (!online.oled) return;

  enablePullups();
  oled.erase();
  oled.text(0, 0, "Warning: RTC sync failed!");
  oled.display();
  disablePullups();
  myDelay(2000);
}

// Display RTC drift offset.
void displayRtcOffset(long drift) {
  if (!online.oled || !firstTimeFlag) return;

  getDateTime();
  enablePullups();
  oled.erase();
  oled.setCursor(0, 0);
  oled.print("RTC sync success!");
  oled.setCursor(0, 10);
  oled.print(dateTimeBuffer);
  oled.display();
  myDelay(4000);
  oled.erase();
  oled.setCursor(0, 0);
  oled.print("RTC drift (s): ");
  oled.setCursor(0, 10);
  oled.print(drift);
  oled.display();
  disablePullups();
  myDelay(2000);
}

// Display microSD initialization error (first attempt).
void displayErrorMicrosd1() {
  enablePullups();
  oled.erase();
  oled.text(0, 0, "Error: microSD");
  oled.text(0, 10, "failed to initialize!");
  oled.text(0, 20, "Reattempting...");
  oled.display();
  disablePullups();
  myDelay(4000);
}

// Display microSD initialization error (second attempt).
void displayErrorMicrosd2() {
  enablePullups();
  oled.erase();
  oled.text(0, 0, "Error: microSD");
  oled.text(0, 10, "second attempt failed!");
  oled.display();
  disablePullups();
  myDelay(4000);
}

// Display log file details.
//
// Shows the current log file name, file size, and buffer usage.
void displayScreen1() {
  if (!online.oled) return;

  enablePullups();
  char displayBuffer1[32];
  char displayBuffer2[32];

  sprintf(displayBuffer1, "File size: %d KB", bytesWritten / 1024);
  sprintf(displayBuffer2, "Max buffer: %d", maxBufferBytes);

  oled.erase();
  oled.text(0, 0, logFileName);
  oled.text(0, 10, displayBuffer1);
  oled.text(0, 20, displayBuffer2);
  oled.display();
  disablePullups();
}

// Display system status.
//
// Shows the current date/time, battery voltage, and logging duration.
void displayScreen2() {
  if (!online.oled) return;

  getDateTime();  // Fetch the current date and time

  enablePullups();
  oled.erase();
  oled.setCursor(0, 0);
  oled.print(dateTimeBuffer);
  oled.setCursor(0, 10);
  oled.print("Voltage:");
  oled.setCursor(54, 10);
  oled.print(readBattery(), 2);
  oled.setCursor(0, 20);
  oled.print("Duration:");
  oled.setCursor(60, 20);
  oled.print(rtc.getEpoch() - logStartTime);
  oled.display();
  disablePullups();
}

// Display deep sleep message.
void displayDeepSleep() {
  if (!online.oled) return;

  enablePullups();
  oled.erase();
  oled.text(0, 0, "Entering deep sleep...");
  oled.display();
  disablePullups();
  myDelay(4000);
}

// Power off the display.
void displayOff() {
  if (online.oled) {
    oled.displayPower(1);
  }
}

// Power on the display.
void displayOn() {
  if (online.oled) {
    oled.displayPower(0);
  }
}

// Run OLED line test.
void lineTest() {
  int width = oled.getWidth();
  int height = oled.getHeight();

  for (int i = 0; i < width; i += 6) {
    oled.line(0, 0, i, height - 1);
    oled.display();
  }
  myDelay(2000);
  oled.erase();
  for (int i = width - 1; i >= 0; i -= 6) {
    oled.line(width - 1, 0, i, height - 1);
    oled.display();
  }
  myDelay(1000);
}
