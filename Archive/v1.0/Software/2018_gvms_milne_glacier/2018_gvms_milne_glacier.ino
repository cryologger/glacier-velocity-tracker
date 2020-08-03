/*

  Title:          Cryologger - Milne Glacier Velocity (MGV)
  Author:         Adam Garbo
  Last modified:  June, 2018
  Description:    Glacier velocity measurement system intended for deployment on Milne Glacier, Ellesmere Island, Nunavut, Canada.

*/

// Libraries
#include <avr/interrupt.h>    // https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
#include <avr/power.h>        // https://www.nongnu.org/avr-libc/user-manual/group__avr__power.html
#include <avr/sleep.h>        // https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html
#include <avr/wdt.h>          // https://www.nongnu.org/avr-libc/user-manual/group__avr__watchdog.html
#include <DS3232RTC.h>        // https://github.com/JChristensen/DS3232RTC

// Defined constants
#define DEBUG                     // Echo data to serial port. Comment line to prevent all Serial.print() commands
#define LED                 13    // LED activity and error indicator
#define RTC_ALARM_PIN       3     // Connect DS3231 SQW/INT pin to Pro Trinket D3 (INT1)
#define STEP_DOWN_EN_PIN    9     // Connect Pololu D24V22F5 EN pin to Pro Trinket D9

// Object instantiations
time_t          t;
tmElements_t    tm;

// User defined constants
const float   resistor1     = 10030000.0;   // Measured resistance (Ohms) of voltage divider R1 (MGV 1 & 2: 10.03 MOhm)
const float   resistor2     = 996000.0;     // Measured resistance (Ohms) of voltage divider R2 (MGV 1: 994 kOhm, MGV 2: 996 kOhm)
const byte    startTime     = 17;           // Hour of day to start recording GNSS observations (24-hour UTC)
const byte    endTime       = 18;           // Hour of day to stop recording GNSS observations (24-hour UTC)
const float   minVoltage    = 11.0;         // Minimum battery voltage required for recording of GNSS observations

// Global variable declarations
bool            flag                = 0;        // Flag for LED blink pattern
volatile bool   alarmIsrWasCalled   = false;    // DS3231 alarm interrupt service routine (ISR) flag
volatile bool   wdtIsrWasCalled     = true;     // Watchdog Timer (WDR) ISR flag
volatile bool   sleeping            = true;     // Sleep flag for WDT ISR
float           batteryVoltage      = 0.0;      // Battery voltage measured from resistor voltage divider
unsigned long   alarmTime           = 2;        // DS3231 alarm interval (for debugging purposes)

// Setup
void setup()
{
#ifdef DEBUG
  Serial.begin(57600);
#endif // DEBUG

  pinMode(STEP_DOWN_EN_PIN, OUTPUT);
  digitalWrite(STEP_DOWN_EN_PIN, LOW);

  // Configure the Watchdog Timer
  configureWatchdog();

  // DS3231 Real-time clock (RTC)
  setSyncProvider(RTC.get); // Synchronizes Time library with external RTC every five minutes
#ifdef DEBUG
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
#endif // DEBUG

  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);    // Initialize alarm 1 to known value
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);    // Initialize alarm 2 to known value
  RTC.alarm(ALARM_1);                           // Clear alarm 1 interrupt flag
  RTC.alarm(ALARM_2);                           // Clear alarm 2 interrupt flag
  RTC.alarmInterrupt(ALARM_1, false);           // Disable interrupt output for alarm 1
  RTC.alarmInterrupt(ALARM_2, false);           // Disable interrupt output for alarm 2
  RTC.squareWave(SQWAVE_NONE);                  // Configure INT/SQW pin for interrupt operation by disabling default square wave output

/*
  // Set RTC to an arbitrary time (for debugging purposes)
  tm.Hour = 16;
  tm.Minute = 59;
  tm.Second = 55;
  tm.Day = 7;
  tm.Month = 6;
  tm.Year = 2017 - 1970;    // tmElements_t.Year is the offset from 1970
  time_t t = makeTime(tm);  // change the tm structure into time_t (seconds since epoch)
  RTC.set(t);
*/

#ifdef DEBUG
  printDateTime(RTC.get());                     // Display current date and time
#endif // DEBUG

  pinMode(RTC_ALARM_PIN, INPUT_PULLUP);         // Enable internal pull-up resistor on Pro Trinket external interrupt pin D3
  attachInterrupt(INT1, alarmIsr, FALLING);     // Wake on falling edge of RTC_ALARM_PIN

  // Set initial RTC alarm 1
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, 0, startTime, 1);   // Set alarm 1
  //RTC.setAlarm(ALM1_MATCH_SECONDS, 0, 0, 0, 1);         // For debugging purposes.
  RTC.alarm(ALARM_1);                                   // Ensure alarm 1 interrupt flag is cleared
  RTC.alarmInterrupt(ALARM_1, true);                    // Enable interrupt output for alarm 1

  // Set initial RTC alarm 2
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, 0, endTime, 1);   // Set alarm 2
  //RTC.setAlarm(ALM2_MATCH_MINUTES, 0, 0, 0, 1);       // For debugging purposes
  RTC.alarm(ALARM_2);                                 // Ensure alarm 2 interrupt flag is cleared
  RTC.alarmInterrupt(ALARM_2, true);                  // Enable interrupt output for alarm 2
  blink(5, 50);
}

// Loop
void loop()
{
  if ((alarmIsrWasCalled) || (wdtIsrWasCalled))
  {
    RTC.read(tm);             // Read current date and time from RTC
    if (RTC.alarm(ALARM_1))   // Check alarm 1 and clear flag if set
    {
#ifdef DEBUG
      Serial.print(F("Alarm 1: "));
      printDateTime(RTC.get());
#endif // DEBUG

      readBattery();
      if (batteryVoltage > minVoltage)
      {
        digitalWrite(STEP_DOWN_EN_PIN, HIGH);   // Enable step-down voltage converter
        flag = true;                            // LED blink pattern selection flag
      }
      else
      {
#ifdef DEBUG
        Serial.println(F("Battery voltage too low!"));
#endif // DEBUG
        blink(2, 750);   // Low voltage LED indicator
        flag = false;
      }

      // Set alarm 1 (for debugging purposes)
      //RTC.setAlarm(ALM1_MATCH_MINUTES, 0, (tm.Minute + alarmTime) % 60, 0, 1);
      //RTC.setAlarm(ALM1_MATCH_HOURS, 0, 0, (tm.Hour + alarmTime) % 24, 1);
    }

    if (RTC.alarm(ALARM_2))   // Check alarm flag and clear if set
    {
#ifdef DEBUG
      Serial.print(F("Alarm 2: "));
      printDateTime(RTC.get());
#endif // DEBUG
      digitalWrite(STEP_DOWN_EN_PIN, LOW);    // Disable step-down voltage converter
      flag = false;                           // LED blink pattern selection flag

      // Set alarm 2 (for debugging purposes)
      //RTC.setAlarm(ALM2_MATCH_MINUTES, 0, (tm.Minute + alarmTime) % 60, 0, 1);
      //RTC.setAlarm(ALM1_MATCH_HOURS, 0, 0, (tm.Hour + alarmTime) % 24, 1);

    }
    if (flag == true)
    {
      blink(3, 250);  // LED indicator of GNSS observations being recorded
    }
    else
    {
      blink(1, 5);    // LED indicator of GNSS observations not being recorded
    }
    alarmIsrWasCalled = false;  // Reset RTC ISR flag
    wdtIsrWasCalled = false;    // Reset WDT ISR flag
    goToSleep();                // Sleep until interrupt is detected from RTC INT/SQW pin
  }
}

// Functions

// Real-time clock alarm interrupt service routine
void alarmIsr()
{
  alarmIsrWasCalled = true;
}

// Configure Watchdog Timer
void configureWatchdog()
{
  cli();                                // Disable all interrupts
  wdt_reset();                          // Reset the watchdog timer
  MCUSR = 0;                            // Clear Reset Flags of MCU Status Register (MCUSR)
  WDTCSR |= (1 << WDCE) | (1 << WDE);   // Start timed sequence allowing alterations to Watchdog Timer Control Register (WDTCSR)
  // Set Watchdog Interrupt Enable (WDIE), clear Watchdog System Reset Enable (WDE) and set Watchdog Timer Prescaler WDP3 and WDP0 to select an 8 s timeout period
  WDTCSR = (1 << WDIE) | (0 << WDE) | (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (1 << WDP0);
  sei();                                // Enable all interrupts
}

// Watchdog interrupt service routine (ISR)
ISR(WDT_vect)
{
  wdtIsrWasCalled = true;
  // Check if asleep to determine if system reset is required
  if (sleeping == true)
  {
    wdt_reset();
    sleeping = false;
  }
  else
  {
    // Enable WDT System Reset Mode
    MCUSR = 0;                          // Clear Reset Flags of MCU Status Register (MCUSR)
    WDTCSR |= (1 << WDCE) | (1 << WDE); // Start timed sequence allowing changes to Watchdog Timer Control Register (WDTCSR)
    // Clear Watchdog Interrupt Enable (WDIE)
    // Set Watchdog System Reset Enable (WDE)
    // Clear Watchdog Timer Prescaler WDP3, WDP2, WDP1 and WDP0 to select a 16 ms timeout period
    WDTCSR = (0 << WDIE) | (1 << WDE) | (0 << WDP3) | (0 << WDP2) | (0 << WDP1) | (0 << WDP0);
    while (1);                          // System reset will occur after 16 ms
  }
}

// Enable sleep and await RTC alarm interrupt
void goToSleep()
{
#ifdef DEBUG
  Serial.println(F("goToSleep();"));
  Serial.flush();
#endif // DEBUG
  byte adcsra = ADCSRA;     // Save ADC Control and Status Register A (ADCSRA)
  ADCSRA = 0;               // Disable the ADC
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();                    // Disable all interrupts
  sleep_enable();           // Set the SE (sleep enable) bit
  wdt_reset();              // Pat the dog
  sleeping = true;          // Set sleep flag
  sleep_bod_disable();      // Disable BOD before going to sleep
  sei();                    // Enable all interrupts
  sleep_cpu();              // Put the device into sleep mode. Must be executed within 3 clock cycles in order to disable BOD
  sleep_disable();          // Clear the SE (sleep enable) bit. Program will execute from this point once awake
  ADCSRA = adcsra;          // Restore ADCSRA
}

// Print current time and date
void printDateTime(time_t t)
{
  Serial.print((day(t) < 10) ? "0" : ""); Serial.print(day(t), DEC); Serial.print('/');
  Serial.print((month(t) < 10) ? "0" : ""); Serial.print(month(t), DEC); Serial.print('/');
  Serial.print(year(t), DEC); Serial.print(' ');
  Serial.print((hour(t) < 10) ? "0" : ""); Serial.print(hour(t), DEC); Serial.print(':');
  Serial.print((minute(t) < 10) ? "0" : ""); Serial.print(minute(t), DEC); Serial.print(':');
  Serial.print((second(t) < 10) ? "0" : ""); Serial.println(second(t), DEC);
}

// Measures battery voltage from 1 MOhm / 10 MOhm resistor voltage divider
void readBattery()
{
  // Allow capacitor to settle before recording analog measurement
  for (int i = 0; i < 5; i++)
  {
    analogRead(A0);
    delay(1);
  }
  batteryVoltage = ((float)analogRead(A0) + 0.5) / (resistor2 / (resistor1 + resistor2) * 1024.0) * 3.3; // For more information see Nick Gammon's voltage divider formula: https://www.gammon.com.au/adc
#ifdef DEBUG
  Serial.print(F("batteryVoltage: ")); Serial.println(batteryVoltage);
#endif // DEBUG
}

// LED activity indicator
void blink(byte flashes, int duration)
{
  pinMode(LED, OUTPUT);
  for (int i = 0; i < flashes; i++)
  {
    digitalWrite(LED, HIGH);
    delay(duration);
    digitalWrite(LED, LOW);
    delay(duration);
  }
  pinMode(LED, INPUT);
}
