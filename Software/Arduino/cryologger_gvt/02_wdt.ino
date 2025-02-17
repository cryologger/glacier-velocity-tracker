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

// Configure and enable the Watchdog Timer.

void configureWdt() {
  // Configure the Watchdog Timer (WDT)
  // Using a 1 Hz clock, 64-second interrupt period, and 128-second reset period.
  // (Alternative configuration is commented out below)
  // wdt.configure(WDT_16HZ, 128, 240); // 16 Hz clock: 10-sec interrupt, 15-sec reset period
  wdt.configure(WDT_1HZ, 64, 128);

  // Start the Watchdog Timer.
  wdt.start();
}

// Reset ("pet") the Watchdog Timer to prevent an unintended system reset. 
// This function also resets the WDT flag and counter, while recording the 
// service time for profiling purposes.
void petDog() {
  // Record the start time for profiling the WDT service routine.
  unsigned long loopStartTime = micros();

  // Restart the WDT timer.
  wdt.restart();

  // DEBUG_PRINT(F("[WDT] Watchdog interrupt: ")); DEBUG_PRINTLN(wdtCounter);

  // Clear the WDT flag and reset the interrupt counter.
  wdtFlag = false;
  wdtCounter = 0;

  // Record the time taken to service the WDT.
  timer.wdt = micros() - loopStartTime;
}