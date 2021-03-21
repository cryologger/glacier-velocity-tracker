// Enter deep sleep
void goToSleep()
{
#if DEBUG
  Serial.end(); // Close Serial port
#endif
  Wire.end(); // Disable I2C
  SPI.end(); // Disable SPI
  power_adc_disable(); // Disable ADC (v1.x)
  //powerControlADC(false); // Disable ADC (v2.x)
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

  // Disable power to Flash, SRAM, and cache
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_CACHE); // Turn off CACHE
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_FLASH_512K); // Turn off everything but lower 512k
  //am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_SRAM_64K_DTCM); // Turn off everything but lower 64k

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
  // Enable power to SRAM, turn on entire Flash
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_MAX);

  // Return to using the main clock
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

  // Renable UART0
  am_hal_gpio_pinconfig(48, g_AM_BSP_GPIO_COM_UART_TX);
  am_hal_gpio_pinconfig(49, g_AM_BSP_GPIO_COM_UART_RX);
  am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_UART0);

  ap3_adc_setup();  // Enable ADC (v1.x)
  initializeADC();  // Enable ADC (v2.x)
  Wire.begin();     // Enable I2C
  SPI.begin();      // Enable SPI
#if DEBUG
  Serial.begin(115200); // Open Serial port
#endif
}

// Enable power to Qwiic connector
void qwiicPowerOn()
{
  digitalWrite(PIN_QWIIC_POWER, HIGH);
  delay(2500);
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
  delay(250);
}

// Disable power to microSD and peripherals
void peripheralPowerOff()
{
  digitalWrite(PIN_PWC_POWER, LOW);
  delay(250);
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
