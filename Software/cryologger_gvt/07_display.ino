// Configure OLED display
void configureOled()
{
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
}

void displayWelcome()
{
  if (online.oled)
  {
    oled.erase();
    oled.text(0, 0, "Cryologger GVT");
    oled.text(0, 10, dateTimeBuffer);
    oled.setCursor(0, 20);
    oled.print("Voltage:");
    oled.setCursor(54, 20);
    oled.print(readVoltage(), 2);
    oled.display();
    myDelay(8000);
  }
}

void displayInitialize(char *device)
{
  if (online.oled)
  {
    char displayBuffer[24];
    sprintf(displayBuffer, "Initializing %s...", device);
    oled.erase();
    oled.text(0, 0, displayBuffer);
    oled.display();
  }
}

void displaySuccess()
{
  if (online.oled)
  {
    oled.text(0, 10, "Success!");
    oled.display();
    myDelay(2000);
  }
}

void displayFailure()
{
  if (online.oled)
  {
    oled.text(0, 10, "Failed!");
    oled.display();
  }
}

void displayReattempt()
{
  if (online.oled)
  {
    oled.text(0, 10, "Failed! Reattempting...");
    oled.display();
  }
}

void displayRtcSync()
{
  if (online.oled)
  {
    oled.erase();
    oled.text(0, 0, "Syncing RTC...");
    oled.display();
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
    myDelay(4000);
  }
}

void displayScreen1()
{
  if (online.oled)
  {
    char displayBuffer1[32];
    char displayBuffer2[32];
    sprintf(displayBuffer1, "File size: %d", (bytesWritten / 1024));
    sprintf(displayBuffer2, "Max buffer: %d", maxBufferBytes);

    oled.erase();
    oled.text(0, 0, logFileName);
    oled.text(0, 10, displayBuffer1);
    oled.text(0, 20, displayBuffer2);
    oled.display();
  }
}

void displayScreen2()
{
  if (online.oled)
  {
    // Get current date and time
    getDateTime();

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
  }
}

void displayDeepSleep()
{
  if (online.oled)
  {
    oled.erase();
    oled.text(0, 0, "Entering deep sleep...");
    oled.display();
    myDelay(2000);
  }
}

void displayOff()
{
  if (online.oled)
  {
    oled.erase();
    oled.display();
  }
}
