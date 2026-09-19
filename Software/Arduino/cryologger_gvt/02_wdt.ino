/*
  Watchdog Timer (WDT) Module

  This module configures the Watchdog Timer to ensure system reliability by 
  resetting the microcontroller if the main loop becomes unresponsive. 
  It sets the WDT's clock divider, interrupt, and reset periods.

  ----------------------------------------------------------------------------
  Watchdog Clock Divider Selections:
  ----------------------------------------------------------------------------
  WDT_OFF     : Disables the watchdog timer (Low Power Mode)
  WDT_128HZ   : 128 Hz LFRC clock
  WDT_16HZ    : 16 Hz LFRC clock
  WDT_1HZ     : 1 Hz LFRC clock
  WDT_1_16HZ  : 1/16 Hz LFRC clock
*/

// ----------------------------------------------------------------------------
// Configure and enable the Watchdog Timer (WDT).
// ----------------------------------------------------------------------------
void configureWdt() {
  // Use a 1/16 Hz clock, 256-second interrupt period, and 512-second reset period
  // Interrupt after 4 minutes 16 seconds and reset after 8 minutes 32 seconds
  wdt.configure(WDT_1_16HZ, 16, 32);

  // Alternative configuration:
  // Use a 1 Hz clock, 64-second interrupt period, and 128-second reset period
  // Interrupt after 1 minute 4 seconds and reset after 2 minutes 8 seconds
  //wdt.configure(WDT_1HZ, 64, 128);

  wdt.start();  // Start the WDT
}

// ----------------------------------------------------------------------------
// Reset ("pet") the Watchdog Timer to prevent an unintended system reset.
// This function also resets the WDT flag and counter, while recording the
// service time for profiling purposes.
// ----------------------------------------------------------------------------
void petDog() {
  // Record the start time for profiling the WDT service routine
  unsigned long loopStartTime = micros();

  // Restart the WDT timer.
  wdt.restart();

  // DEBUG_PRINT("[WDT] Info: Watchdog interrupt = "); DEBUG_PRINTLN(wdtCounter);

  // Clear the WDT flag and reset the interrupt counter
  wdtFlag = false;
  wdtCounter = 0;

  // Record the time taken to service the WDT
  timer.wdt = micros() - loopStartTime;
}
