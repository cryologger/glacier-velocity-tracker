/*
  microSD Module

  This module handles the initialization and management of the microSD card.
  It ensures proper initialization, manages file timestamps, and logs errors
  when failures occur.
*/

// Configure and initialize the microSD card.
//
// This function attempts to initialize the microSD card and sets the appropriate
// online flag. If initialization fails, it retries before shutting down power
// to conserve energy.
void configureSd() {
  unsigned long loopStartTime = millis();  // Start loop timer.

  // Check if microSD is already initialized.
  if (online.microSd) {
    DEBUG_PRINTLN(F("[microSD] Info: Already initialized."));
    return;
  }

  displayInitialize("microSD");  // Display OLED message.

  // Attempt microSD initialization with a maximum of 2 retries.
  for (int attempt = 1; attempt <= 2; attempt++) {
    if (sd.begin(PIN_SD_CS, SD_SCK_MHZ(24))) {
      online.microSd = true;  // Set flag.
      DEBUG_PRINTLN(F("[microSD] Info: Initialized successfully."));
      displaySuccess();  // Display OLED success message.
      break;             // Exit loop on success.
    }

    DEBUG_PRINTLN(F("[microSD] Warning: Initialization failed. Retrying..."));
    displayErrorMicrosd1();  // Display OLED error message.
    myDelay(2000);           // Non-blocking delay before retry.

    // On second failure, log error and disable peripherals.
    if (attempt == 2) {
      DEBUG_PRINTLN(F("[microSD] Error: Failed to initialize."));
      online.microSd = false;  // Set flag.

      displayErrorMicrosd2();  // Display OLED failure message.
      qwiicPowerOff();         // Disable power to Qwiic connector.
      peripheralPowerOff();    // Disable power to peripherals.
    }
  }

  timer.microSd = millis() - loopStartTime;  // Stop loop timer.
}

// Update the file creation timestamp.
//
// This function updates the file's creation timestamp using the RTC. If
// the update fails, a warning message is logged.
void updateFileCreate(FsFile *dataFile) {
  rtc.getTime();  // Get current RTC date and time.

  if (!dataFile->timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN(F("[microSD] Warning: Could not update file create timestamp."));
  }
}

// Update file access and write timestamps.
//
// This function updates the file's last access and last write timestamps
// using the RTC. If the update fails, a warning message is logged.
void updateFileAccess(FsFile *dataFile) {
  rtc.getTime();  // Get current RTC date and time.

  if (!dataFile->timestamp(T_ACCESS, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN(F("[microSD] Warning: Could not update file access timestamp."));
  }

  if (!dataFile->timestamp(T_WRITE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                           rtc.hour, rtc.minute, rtc.seconds)) {
    DEBUG_PRINTLN(F("[microSD] Warning: Could not update file write timestamp."));
  }
}