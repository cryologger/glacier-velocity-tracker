// Power down the entire system but maintain RTC clock
// Function takes 100 us to run including GPIO setting
// Places Artemis into 2.36 uA to 2.6 uA consumption mode
// With leakage across the 3.3V protection diode, ~3.00 uA
void powerDown()
{
  // Prevent voltage supervisor from waking us from sleep
  detachInterrupt(digitalPinToInterrupt(PIN_POWER_LOSS));

  // Prevent stop logging button from waking us from sleep
  if (settings.useGPIO32ForStopLogging)
  {
    detachInterrupt(digitalPinToInterrupt(PIN_STOP_LOGGING)); // Disable the interrupt
    pinMode(PIN_STOP_LOGGING, INPUT); // Remove the pull-up
  }

  // Power down immediately
  qwiic.end();          // Power down I2C
  SPI.end();            // Power down SPI
  power_adc_disable();  // Power down ADC
  Serial.end();         // Power down UART

  // Force the peripherals off
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM1);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM2);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM3);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM4);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM5);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART1);

  // Disable pads
  for (int x = 0; x < 50; x++)
  {
    if ((x != ap3_gpio_pin2pad(PIN_POWER_LOSS)) &&
        (x != ap3_gpio_pin2pad(PIN_MICROSD_POWER)) &&
        (x != ap3_gpio_pin2pad(PIN_QWIIC_POWER)) &&
        (x != ap3_gpio_pin2pad(PIN_IMU_POWER)))
    {
      am_hal_gpio_pinconfig(x, g_AM_HAL_GPIO_DISABLE);
    }
  }

  powerLedOff(); // Turn OFF PWR LED

  // Ensure PIN_POWER_LOSS is configured as an input for the WDT
  pinMode(PIN_POWER_LOSS, INPUT); // BD49K30G-TL has CMOS output and does not need a pull-up

  // No not leave power control pins floating
  imuPowerOff(); // Power down IMU
  microSdPowerOff(); // Power down SD

  // Keep Qwiic bus powered on if user desires it - but only for X04 to avoid a brown-out
#if(HARDWARE_VERSION_MAJOR == 0)
  if (settings.powerDownQwiicBusBetweenReads)
  {
    qwiicPowerOff();
  }
  else
  {
    qwiicPowerOn(); // Ensure pins are set as OUTPUT
  }
#else
  {
    qwiicPowerOff();
  }
#endif

  // Power down Flash, SRAM, cache
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_CACHE); //Turn off CACHE
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_FLASH_512K); //Turn off everything but lower 512k
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_SRAM_64K_DTCM); //Turn off everything but lower 64k //TO DO: check this! GNSSbuffer is 24K!
  //am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL); //Turn off all memory (doesn't recover)

  // Keep the 32kHz clock running for RTC
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);

  // Remain in deep sleep until manual reset
  while (1)
  {
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP); // Deep sleep
  }
}

// Power down and await interrupt wakeup
void goToSleep()
{
  if (settings.printMajorDebugMessages)
  {
    Serial.println(F("Entering deep sleep..."));
  }

  uint32_t msToSleep = (uint32_t)(settings.usSleepDuration / 1000ULL);

  // Counter/Timer 6 will use the 32kHz clock
  // Calculate how many 32768Hz system ticks we need to sleep for:
  //sysTicksToSleep = msToSleep * 32768L / 1000
  // Be careful with the multiply and overflow of uint32_t if msToSleep is > 131072
  uint32_t sysTicksToSleep;
  if (msToSleep < 131000)
  {
    sysTicksToSleep = msToSleep * 32768L; // Do the multiply first for short intervals
    sysTicksToSleep = sysTicksToSleep / 1000L; // Now do the divide
  }
  else
  {
    sysTicksToSleep = msToSleep / 1000L; // Do the division first for long intervals (to avoid an overflow)
    sysTicksToSleep = sysTicksToSleep * 32768L; // Now do the multiply
  }

  detachInterrupt(digitalPinToInterrupt(PIN_POWER_LOSS)); // Prevent voltage supervisor from waking us from sleep

  // Prevent stop logging button from waking us from sleep
  if (settings.useGPIO32ForStopLogging)
  {
    detachInterrupt(digitalPinToInterrupt(PIN_STOP_LOGGING)); // Disable the interrupt
    pinMode(PIN_STOP_LOGGING, INPUT); // Remove the pull-up
  }

  if (qwiicAvailable.uBlox && qwiicOnline.uBlox) // Check if u-blox is available and logging
  {
    // Disable all messages in RAM otherwise they will fill up the module's I2C buffer while we are asleep
    // (Possibly redundant if using a power management task?)
    disableMessages(0);
    // Using a maxWait of zero means we don't wait for the ACK/NACK
    // and success will always be false (sendCommand returns SFE_UBLOX_STATUS_SUCCESS not SFE_UBLOX_STATUS_DATA_SENT)
  }

  // Save files before going to sleep
  if (online.dataLogging)
  {
    unsigned long pauseUntil = millis() + 550UL; // Wait > 500ms so we can be sure SD data is synced
    while (millis() < pauseUntil) // During pause continue writing data to SD
    {
      storeData(); // Read I2C data and write to SD
    }

    file.sync();
    updateDataFileAccess(); // Update the file access timestamp
    file.close();
    delay(sdPowerDownDelay); // Give the SD card time to finish writing ***CRITICAL***
  }

  // Check if power management task used to put the module to sleep
  if (qwiicAvailable.uBlox && qwiicOnline.uBlox && (settings.sensor_uBlox.powerManagement))
  {
    powerManagementTask((msToSleep - 1000), 0); // Wake the module up 1 s before the Artemis so it is ready to rock
    // UBX_RXM_PMREQ does not ACK so there is no point in checking the return value
    if (settings.printMajorDebugMessages)
    {
      Serial.println(F("goToSleep: powerManagementTask request sent"));
    }
  }

  Serial.flush(); // Finish any prints

  //Wire.end();           // Power down I2C
  qwiic.end();          // Power down I2C
  SPI.end();            // Power down SPI
  power_adc_disable();  // Power down ADC
  Serial.end();         // Power down UART

  // Force the peripherals off
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM1);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM2);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM3);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM4);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM5);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART0);
  am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART1);

  // Disable pads
  for (int x = 0; x < 50; x++)
  {
    if ((x != ap3_gpio_pin2pad(PIN_POWER_LOSS)) &&
        (x != ap3_gpio_pin2pad(PIN_MICROSD_POWER)) &&
        (x != ap3_gpio_pin2pad(PIN_QWIIC_POWER)) &&
        (x != ap3_gpio_pin2pad(PIN_IMU_POWER)))
    {
      am_hal_gpio_pinconfig(x, g_AM_HAL_GPIO_DISABLE);
    }
  }

  // Ensure PIN_POWER_LOSS is configured as an input for the WDT
  pinMode(PIN_POWER_LOSS, INPUT); // BD49K30G-TL has CMOS output and does not need a pull-up

  // Do not leave power control pins floating
  imuPowerOff();
  //microSdPowerOff();

  // Testing file record issues
  microSdPowerOn();

  // Keep Qwiic bus powered on if user desires it
  if (settings.powerDownQwiicBusBetweenReads)
  {
    qwiicPowerOff();
    qwiicOnline.uBlox = false; //Mark as offline so it will be started with new settings
  }
  else
  {
    qwiicPowerOn(); // Ensure pins stays as output
  }

  // Leave the power LED on if the user desires it
  if (settings.enablePwrLedDuringSleep)
  {
    powerLedOn();
  }
  else
  {
    powerLedOff();
  }

  // Power down Flash, SRAM, cache
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_CACHE); //Turn off CACHE
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_FLASH_512K); //Turn off everything but lower 512k
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_SRAM_64K_DTCM); //Turn off everything but lower 64k //TO DO: check this! GNSSbuffer is 24K!
  //am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL); //Turn off all memory (doesn't recover)

  // Use the lower power 32kHz clock. Use it to run CT6 as well.
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ | AM_HAL_STIMER_CFG_COMPARE_G_ENABLE);

  // Adjust sysTicks down by the amount we've be at 48MHz
  uint32_t msBeenAwake = millis();
  uint32_t sysTicksAwake = msBeenAwake * 32768L / 1000L; //Convert to 32kHz systicks
  sysTicksToSleep -= sysTicksAwake;

  // Setup interrupt to trigger when the number of ms have elapsed
  am_hal_stimer_compare_delta_set(6, sysTicksToSleep);

  // We use counter/timer 6 to cause us to wake up from sleep but 0 to 7 are available
  // CT 7 is used for Software Serial. All CTs are used for Servo.
  am_hal_stimer_int_clear(AM_HAL_STIMER_INT_COMPAREG); //Clear CT6
  am_hal_stimer_int_enable(AM_HAL_STIMER_INT_COMPAREG); //Enable C/T G=6

  // Enable the timer interrupt in the NVIC.
  NVIC_EnableIRQ(STIMER_CMPR6_IRQn);

  // Halt the WDT otherwise this will bring us out of deep sleep
  am_hal_wdt_halt();

  // Deep sleep
  am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

  // Re-start the WDT
  am_hal_wdt_start();

  // Turn off interrupt
  NVIC_DisableIRQ(STIMER_CMPR6_IRQn);

  am_hal_stimer_int_disable(AM_HAL_STIMER_INT_COMPAREG); //Enable C/T G=6

  // We're BACK!
  wakeFromSleep();
}

// Power everything up gracefully
void wakeFromSleep()
{
  // Power up SRAM, turn on entire Flash
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_MAX);

  // Go back to using the main clock
  //am_hal_stimer_int_enable(AM_HAL_STIMER_INT_OVERFLOW);
  //NVIC_EnableIRQ(STIMER_IRQn);
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

  // Turn on ADC
  ap3_adc_setup();

  // Run setup again

  // If 3.3V rail drops below 3V, system will enter low power mode and maintain RTC
  pinMode(PIN_POWER_LOSS, INPUT); // BD49K30G-TL has CMOS output and does not need a pull-up

  delay(1); // Let PIN_POWER_LOSS stabilize

  attachInterrupt(digitalPinToInterrupt(PIN_POWER_LOSS), powerDown, FALLING);

  if (digitalRead(PIN_POWER_LOSS) == LOW)
  {
    powerDown(); // Check PIN_POWER_LOSS in case falling edge was missed
  }

  if (settings.useGPIO32ForStopLogging)
  {
    pinMode(PIN_STOP_LOGGING, INPUT_PULLUP);
    delay(1); // Let the pin stabilize
    attachInterrupt(digitalPinToInterrupt(PIN_STOP_LOGGING), stopLoggingIsr, FALLING); // Enable the interrupt
    stopLoggingFlag = false; // Ensure the flag is clear
  }

  pinMode(PIN_STAT_LED, OUTPUT);
  digitalWrite(PIN_STAT_LED, LOW);

  powerLedOn(); // Turn ON PWR LED

  Serial.begin(settings.serialTerminalBaudRate);

  SPI.begin(); // Required if SD is disabled
  beginSd();
  beginQwiic();
  beginDataLogging();
  disableImu(); // Disable IMU

  // Check if module is powered down
  if (!qwiicOnline.uBlox)
  {
    beginSensors(); // Restart the module
  }
  else
  {
    // If module active, enable selected messages
    enableMessages(2100);
  }
}

// Stop data logging
void stopLogging(void)
{
  detachInterrupt(digitalPinToInterrupt(PIN_STOP_LOGGING)); // Disable the interrupt

  if (qwiicAvailable.uBlox && qwiicOnline.uBlox) // Check if the u-blox is available and logging
  {
    // Disable all messages in RAM
    disableMessages(0);
    // Using a maxWait of zero means we don't wait for the ACK/NACK
    // and success will always be false (sendCommand returns SFE_UBLOX_STATUS_SUCCESS not SFE_UBLOX_STATUS_DATA_SENT)
  }

  // Save files before going to sleep
  if (online.dataLogging)
  {
    unsigned long pauseUntil = millis() + 550UL; //Wait > 500ms so we can be sure SD data is synced
    while (millis() < pauseUntil) //While we are pausing, keep writing data to SD
    {
      storeData(); // Read I2C data and write to SD
    }
    file.sync();
    updateDataFileAccess(); // Update the file access time stamp
    file.close();
  }

  Serial.print(F("Logging is stopped. Please reset OpenLog Artemis and open a terminal at "));
  Serial.print((String)settings.serialTerminalBaudRate);
  Serial.println(F("bps..."));
  digitalWrite(PIN_STAT_LED, HIGH); // Turn ON STAT LED to indicate logging has stopped
  delay(sdPowerDownDelay); // Give the SD card time to shut down and for the serial message to send
  powerDown();
}

void qwiicPowerOn()
{
  pinMode(PIN_QWIIC_POWER, OUTPUT);
#if(HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 4)
  digitalWrite(PIN_QWIIC_POWER, LOW);
#elif(HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 5)
  digitalWrite(PIN_QWIIC_POWER, LOW);
#elif(HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 6)
  digitalWrite(PIN_QWIIC_POWER, HIGH);
#elif(HARDWARE_VERSION_MAJOR == 1 && HARDWARE_VERSION_MINOR == 0)
  digitalWrite(PIN_QWIIC_POWER, HIGH);
#endif
}
void qwiicPowerOff()
{
  pinMode(PIN_QWIIC_POWER, OUTPUT);
#if(HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 4)
  digitalWrite(PIN_QWIIC_POWER, HIGH);
#elif(HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 5)
  digitalWrite(PIN_QWIIC_POWER, HIGH);
#elif(HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 6)
  digitalWrite(PIN_QWIIC_POWER, LOW);
#elif(HARDWARE_VERSION_MAJOR == 1 && HARDWARE_VERSION_MINOR == 0)
  digitalWrite(PIN_QWIIC_POWER, LOW);
#endif
}

void microSdPowerOn()
{
  pinMode(PIN_MICROSD_POWER, OUTPUT);
  digitalWrite(PIN_MICROSD_POWER, LOW);
}
void microSdPowerOff()
{
  pinMode(PIN_MICROSD_POWER, OUTPUT);
  digitalWrite(PIN_MICROSD_POWER, HIGH);
}

void imuPowerOn()
{
  pinMode(PIN_IMU_POWER, OUTPUT);
  digitalWrite(PIN_IMU_POWER, HIGH);
}
void imuPowerOff()
{
  pinMode(PIN_IMU_POWER, OUTPUT);
  digitalWrite(PIN_IMU_POWER, LOW);
}

void powerLedOn()
{
#if(HARDWARE_VERSION_MAJOR >= 1)
  pinMode(PIN_PWR_LED, OUTPUT);
  digitalWrite(PIN_PWR_LED, HIGH); // Turn the Power LED on
#endif
}
void powerLedOff()
{
#if(HARDWARE_VERSION_MAJOR >= 1)
  pinMode(PIN_PWR_LED, OUTPUT);
  digitalWrite(PIN_PWR_LED, LOW); // Turn the Power LED off
#endif
}

// Return number of milliseconds according to the RTC (10 ms increments)
// Watch out for the year roll-over!
uint64_t rtcMillis()
{
  rtc.getTime();
  uint64_t millisToday = 0;
  int dayOfYear = calculateDayOfYear(rtc.dayOfMonth, rtc.month, rtc.year + 2000);
  millisToday += ((uint64_t)dayOfYear * 86400000ULL);
  millisToday += ((uint64_t)rtc.hour * 3600000ULL);
  millisToday += ((uint64_t)rtc.minute * 60000ULL);
  millisToday += ((uint64_t)rtc.seconds * 1000ULL);
  millisToday += ((uint64_t)rtc.hundredths * 10ULL);

  return (millisToday);
}

// Return day of year
// https://gist.github.com/jrleeman/3b7c10712112e49d8607
int calculateDayOfYear(int day, int month, int year)
{
  // Given a day, month, and year (4 digit), returns
  // the day of year. Errors return 999.

  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Verify 4-digit year
  if (year < 1000)
  {
    return 999;
  }

  // Check for leap year, this is confusing business
  // See: https://support.microsoft.com/en-us/kb/214019
  if (year % 4  == 0)
  {
    if (year % 100 != 0)
    {
      daysInMonth[1] = 29;
    }
    else
    {
      if (year % 400 == 0)
      {
        daysInMonth[1] = 29;
      }
    }
  }

  // Ensure valid day of the month
  if (day < 1)
  {
    return 999;
  }
  else if (day > daysInMonth[month - 1])
  {
    return 999;
  }

  int doy = 0;
  for (int i = 0; i < month - 1; i++)
  {
    doy += daysInMonth[i];
  }

  doy += day;
  return doy;
}
