void readBattery()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Stop the loop timer
  timer.battery = millis() - loopStartTime;
}

// Enter deep sleep
void goToSleep()
{
#if DEBUG
  Serial.end();         // Close Serial port
#endif
  Wire.end();           // Disable I2C
  SPI.end();            // Disable SPI
  power_adc_disable();  // Disable ADC

  digitalWrite(LED_BUILTIN, LOW); // Turn off LED

  // Force peripherals off
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM1);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM2);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM3);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM4);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM5);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART1);

  // Disable all pads except G1 (33), G2 (34) and LED_BUILTIN (19)
  for (int x = 0; x < 50; x++)
  {
    if ((x != 33) && (x != 34) && (x != 19))
    {
      am_hal_gpio_pinconfig(x, g_AM_HAL_GPIO_DISABLE);
    }
  }

  // Disable power to Qwiic connector
  qwiicPowerOff();

  // Disable power to peripherals
  peripheralPowerOff();

  // Clear online/offline flags
  online.gnss = false;
  online.microSd = false;
  online.logGnss = false;
  online.logDebug = false;

  // Power down flash, SRAM, cache
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL); // Power down all memory during deepsleep
  am_hal_pwrctrl_memory_deepsleep_retain(AM_HAL_PWRCTRL_MEM_SRAM_384K); // Retain all SRAM

  // Keep the 32kHz clock running for RTC
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);

  // Enter deep sleep
  am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

  /*
     Processor sleeps and awaits RTC or WDT ISR
  */

  // Wake up
  wakeUp();
}

// Wake from deep sleep
void wakeUp()
{
  // Return to using the main clock
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

  ap3_adc_setup();        // Enable ADC
  Wire.begin();           // Enable I2C
  Wire.setPullups(0);     // Disable internal I2C pull-ups to help reduce bus errors
  Wire.setClock(400000);  // Set I2C clock speed to 400 kHz
  SPI.begin();            // Enable SPI
#if DEBUG
  Serial.begin(115200);   // Open Serial port
#endif
}

// Enable power to Qwiic connector
void qwiicPowerOn()
{
  digitalWrite(PIN_QWIIC_POWER, HIGH);
  myDelay(2500); // Non-blocking delay to allow Qwiic devices time to power up
}

// Disable power to Qwiic connector
void qwiicPowerOff()
{
  digitalWrite(PIN_QWIIC_POWER, LOW);
}

// Enable power to microSD and peripherals
void peripheralPowerOn()
{
  digitalWrite(PIN_PWC_POWER, HIGH);
  myDelay(250); // Non-blocking delay
}

// Disable power to microSD and peripherals
void peripheralPowerOff()
{
  myDelay(250); // Non-blocking delay
  digitalWrite(PIN_PWC_POWER, LOW);
}

// Non-blocking blink LED (https://forum.arduino.cc/index.php?topic=503368.0)
void blinkLed(byte ledFlashes, unsigned int ledDelay)
{
  byte i = 0;
  while (i < ledFlashes * 2)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= ledDelay)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      previousMillis = currentMillis;
      i++;
    }
  }
  // Turn off LED
  digitalWrite(LED_BUILTIN, LOW);
}

// Non-blocking delay (ms: duration)
// https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
void myDelay(unsigned long ms)
{
  unsigned long start = millis();         // Start: timestamp
  for (;;)
  {
    petDog();                             // Reset watchdog timer
    unsigned long now = millis();         // Now: timestamp
    unsigned long elapsed = now - start;  // Elapsed: duration
    if (elapsed >= ms)                    // Comparing durations: OK
      return;
  }
}
