// Configure the Watchdog Timer
void configureWdt()
{
  /*
    Simplified WDT Clock Divider Selections
    WDT_OFF     = Low Power Mode. This setting disables the watch dog timer
    WDT_128HZ   = 28 Hz LFRC clock
    WDT_16HZ    = 16 Hz LFRC clock
    WDT_1HZ     = 1 Hz LFRC clock
    WDT_1_16HZ  = 1/16th Hz LFRC clock
  */
  // Set the watchdog timer interrupt and reset periods
  //wdt.configure(WDT_16HZ, 128, 240); // 16 Hz clock, 10-second interrupt period, 15-second reset period
  wdt.configure(WDT_1HZ, 64, 128); // 1 Hz clock, 64-second interrupt period, 128-second reset period

  // Start the watchdog timer
  wdt.start();
}

// Reset the watchdog timer
void petDog()
{
  wdt.restart(); // Restart the watchdog timer
  //DEBUG_PRINT("Watchdog interrupt: "); DEBUG_PRINTLN(wdtCounter);
  wdtFlag = false; // Clear watchdog flag
  wdtCounter = 0; // Reset watchdog interrupt counter
}
