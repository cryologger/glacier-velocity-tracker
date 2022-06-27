// Configure OLED display
void configureOled()
{
  // Enable internal I2C pull-ups
  enablePullups();

  // Initalize the OLED device and related graphics system
  if (!oled.begin())
  {
    online.oled = false;
    DEBUG_PRINTLN("Warning: OLED failed to initialize.");
  }
  else
  {
    online.oled = true;
    DEBUG_PRINTLN("Info: OLED initialized");
  }

  // Disable internal I2C pull-ups
  disablePullups();
}

// Reset OLED display after sleep/power cycle
void resetOled()
{
#if OLED
  online.oled = true;
  enablePullups();
  oled.reset(1);
#endif
}

void displayWelcome()
{
  if (online.oled)
  {
    enablePullups(); // Enable internal I2C pull-ups
    oled.erase();
    oled.text(0, 0, "Cryologger GVT #1");
    oled.text(0, 10, dateTimeBuffer);
    oled.setCursor(0, 20);
    oled.print("Voltage:");
    oled.setCursor(54, 20);
    oled.print(readVoltage(), 2);
    oled.display();
    disablePullups();
    myDelay(4000);
  }
}

void displayInitialize(char *device)
{
  if (online.oled)
  {
    enablePullups();
    char displayBuffer[24];
    sprintf(displayBuffer, "Initializing %s...", device);
    oled.erase();
    oled.text(0, 0, displayBuffer);
    oled.display();
    disablePullups();
  }
}

void displaySuccess()
{
  if (online.oled)
  {
    enablePullups();
    oled.text(0, 10, "Success!");
    oled.display();
    disablePullups();
    myDelay(2000);
  }
}

void displayFailure()
{
  if (online.oled)
  {
    enablePullups(); // Enable internal I2C pull-ups
    oled.text(0, 10, "Failed!");
    oled.display();
    disablePullups();
  }
}

void displayReattempt()
{
  if (online.oled)
  {
    enablePullups();
    oled.text(0, 10, "Failed! Reattempting...");
    oled.display();
    disablePullups();
  }
}

void displayRtcSync()
{
  if (online.oled)
  {
    enablePullups();
    oled.erase();
    oled.text(0, 0, "Syncing RTC...");
    oled.display();
    disablePullups();
  }
}

void displayRtcOffset(long drift)
{
  if (online.oled)
  {
    // Get current date and time
    getDateTime();

    oled.erase();
    oled.setCursor(0, 0);
    oled.print(dateTimeBuffer);
    oled.setCursor(0, 10);
    oled.print("RTC drift: ");
    oled.setCursor(66, 10);
    oled.print(drift);
    oled.display();
    disablePullups();
  }
}

void displayScreen1()
{
  if (online.oled)
  {
    enablePullups();
    char displayBuffer1[32];
    char displayBuffer2[32];
    sprintf(displayBuffer1, "File size: %d", (bytesWritten / 1024));
    sprintf(displayBuffer2, "Max buffer: %d", maxBufferBytes);

    oled.erase();
    oled.text(0, 0, logFileName);
    oled.text(0, 10, displayBuffer1);
    oled.text(0, 20, displayBuffer2);
    oled.display();
    disablePullups();
  }
}

void displayScreen2()
{
  if (online.oled)
  {
    // Get current date and time
    getDateTime();

    enablePullups();
    oled.erase();
    oled.setCursor(0, 0);
    oled.print(dateTimeBuffer);
    oled.setCursor(0, 10);
    oled.print("Voltage:");
    oled.setCursor(54, 10);
    oled.print(readVoltage(), 2);
    oled.setCursor(90, 10);
    oled.print(reading);
    oled.setCursor(0, 20);
    oled.print("Duration:");
    oled.setCursor(60, 20);
    oled.print((rtc.getEpoch() - logStartTime));
    oled.display();
    disablePullups();
  }
}

void displayDeepSleep()
{
  if (online.oled)
  {
    enablePullups();
    oled.erase();
    oled.text(0, 0, "Entering deep sleep...");
    oled.display();
    disablePullups();
    myDelay(4000);
  }
}

void displayOff()
{
  if (online.oled)
  {
    oled.displayPower(1);
  }
}

void displayOn()
{
  if (online.oled)
  {
    oled.displayPower(0);
  }
}
