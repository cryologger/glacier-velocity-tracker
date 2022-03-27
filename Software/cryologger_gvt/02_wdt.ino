// Configure the Watchdog Timer (WDT)
void configureWdt()
{
  /*
    Watchdog Timer (WDT) Clock Divider Selections
    See: Apollo3 Blue Datasheet Section 15/Table 1002 for more information
    WDT_OFF     = Low Power Mode. This setting disables the watchdog timer
    WDT_128HZ   = 28 Hz LFRC clock
    WDT_16HZ    = 16 Hz LFRC clock
    WDT_1HZ     = 1 Hz LFRC clock
    WDT_1_16HZ  = 1/16th Hz LFRC clock
  */
  // Set the WDT interrupt and reset periods
  wdt.configure(WDT_16HZ, 128, 240); // 16 Hz clock, 10-second interrupt period, 15-second reset period
  //wdt.configure(WDT_1HZ, 64, 128); // 1 Hz clock, 64-second interrupt period, 128-second reset period

  // Start the WDT
  wdt.start();
}

// Reset the WDT
void petDog()
{
  // Start the loop timer
  unsigned long loopStartTime = micros();

  wdt.restart(); // Restart the WDT
  //DEBUG_PRINT("Watchdog interrupt: "); DEBUG_PRINTLN(wdtCounter);
  wdtFlag = false; // Clear WDT flag
  wdtCounter = 0; // Reset WDT interrupt counter

  // Stop the loop timer
  timer.wdt = micros() - loopStartTime;
}
