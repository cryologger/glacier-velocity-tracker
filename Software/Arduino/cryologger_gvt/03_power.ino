/*
  Power Management Module

  This module handles battery voltage measurement, I2C pull-up configuration,
  and deep sleep management. It includes routines to shut down peripherals,
  disable leakage sources, and reinitialize the system upon wake-up.
*/

// ----------------------------------------------------------------------------
// Reads the ADC voltage from a battery voltage divider and applies
// a gain/offset correction. Returns the voltage in volts.
// ----------------------------------------------------------------------------
float readBattery() {
  unsigned long startTime = micros();  // Record function start time.

  // Measure the ADC reading from the battery voltage divider on A0.
  reading = analogRead(A0);

  // Convert ADC reading to voltage using the scaling factor and offset.
  float voltage = reading / 452.89;  // Apply ADC linear gain.
  voltage += -0.13;                  // Apply ADC linear offset.

  //DEBUG_PRINT("[POWER] Info: ADC = "); DEBUG_PRINTLN(reading);
  //DEBUG_PRINT("[POWER] Info: Voltage = "); DEBUG_PRINTLN(voltage);

  timer.voltage = micros() - startTime;  // Record execution time.
  return voltage;
}

// Enable internal I2C pull-ups to maintain stable communication
// with connected I2C devices.
void enablePullups() {
  Wire.setPullups(1);
}

// Disable internal I2C pull-ups to reduce leakage and potential bus errors.
void disablePullups() {
  Wire.setPullups(0);
}

// ----------------------------------------------------------------------------
// Enter deep sleep mode to conserve power.
// Disables peripherals (I2C, SPI, ADC, etc.), turns off unused GPIO pads,
// powers down external devices, and configures the system to wake up on an RTC
// or WDT interrupt.
// ----------------------------------------------------------------------------
void goToSleep() {
  firstTimeFlag = false;  // Clear the first-time flag.

  // Skip deep sleep if operating in continuous logging mode.
  if (operationMode == CONTINUOUS) {
    DEBUG_PRINTLN("[POWER] Info: System is in continuous mode. Skipping sleep...");
    return;
  } else {
    alarmFlag = false;  // Ensure the alarm flag is cleared.
  }

  // Display deep sleep message on OLED.
  displayDeepSleep();

#if DEBUG
  Serial.flush();  // Flush the serial port.
  Serial.end();    // Close Serial port to save power.
#endif

  // Disable peripherals and reduce leakage.
  disablePullups();
  Wire.end();
  SPI.end();
  power_adc_disable();

  // Turn off the built-in LED.
  digitalWrite(LED_BUILTIN, LOW);

  // Disable hardware peripherals.
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM1);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM2);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM3);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM4);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM5);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART1);

  // Disable all GPIO pads except: G1 (33), G2 (34), A0, LED_BUILTIN (19).
  for (int pin = 0; pin < 50; pin++) {
    if ((pin != 33) && (pin != 34) && (pin != A0) && (pin != 19)) {
      am_hal_gpio_pinconfig(pin, g_AM_HAL_GPIO_DISABLE);
    }
  }

  // Power off external devices.
  qwiicPowerOff();
  peripheralPowerOff();

  // Clear all online status flags.
  online.gnss = false;
  online.microSd = false;
  online.oled = false;
  online.logGnss = false;
  online.logDebug = false;

  // Configure memory power settings for deep sleep.
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL);
  am_hal_pwrctrl_memory_deepsleep_retain(AM_HAL_PWRCTRL_MEM_SRAM_384K);

  // Configure system timer for RTC.
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);

  // Enter deep sleep.
  am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

  /*
     Processor now sleeps and awaits an RTC or WDT interrupt.
  */

  wakeUp();  // Reinitialize system upon waking.
}

// ----------------------------------------------------------------------------
// Wake up from deep sleep and reinitialize system components.
// ----------------------------------------------------------------------------
void wakeUp() {
  // Reconfigure system timer to use the high-frequency clock.
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

  // Re-enable ADC, I2C, and SPI.
  ap3_adc_setup();
  Wire.begin();
  Wire.setClock(400000);
  SPI.begin();

#if DEBUG
  Serial.begin(115200);  // Reopen Serial for debugging.
#endif
}

// ----------------------------------------------------------------------------
// Restore power to all necessary peripherals.
// Reinitializes power for I2C devices, GNSS, OLED, and the microSD card.
// ----------------------------------------------------------------------------
void restorePeripherals() {
  DEBUG_PRINTLN("[Power] Info: Restoring power to peripherals.");
  qwiicPowerOn();       // Re-enable power to I2C devices.
  peripheralPowerOn();  // Re-enable power to peripherals.
  resetOled();          // Reset/reconfigure the OLED display.
  configureSd();        // Reinitialize the microSD card.
  configureGnss();      // Reinitialize the GNSS receiver.
}

// ----------------------------------------------------------------------------
// Power control for the Qwiic connector.
// ----------------------------------------------------------------------------
void qwiicPowerOn() {
  digitalWrite(PIN_QWIIC_POWER, HIGH);
  myDelay(2500);  // Non-blocking delay to allow Qwiic devices time to power up.
}

void qwiicPowerOff() {
  digitalWrite(PIN_QWIIC_POWER, LOW);
}

// ----------------------------------------------------------------------------
// Power control for microSD and other peripherals.
// ----------------------------------------------------------------------------
void peripheralPowerOn() {
  digitalWrite(PIN_MICROSD_POWER, HIGH);
  myDelay(250);  // Non-blocking delay to allow peripherals time to power up.
}

void peripheralPowerOff() {
  myDelay(250);
  digitalWrite(PIN_MICROSD_POWER, LOW);
}

// ----------------------------------------------------------------------------
// Non-blocking LED blink routine.
// Flashes the built-in LED a specified number of times with the given delay.
// ----------------------------------------------------------------------------
void blinkLed(byte ledFlashes, unsigned int ledDelay) {
  byte i = 0;
  while (i < ledFlashes * 2) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= ledDelay) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      previousMillis = currentMillis;
      i++;
    }
  }
  digitalWrite(LED_BUILTIN, LOW);  // Ensure LED is off after blinking.
}

// ----------------------------------------------------------------------------
// Non-blocking delay function that continues to reset the Watchdog Timer.
// This function delays for a specified duration (in milliseconds) while
// calling petDog() to prevent unintended WDT resets.
// ----------------------------------------------------------------------------
void myDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    petDog();  // Reset the WDT during the delay.
  }
}
