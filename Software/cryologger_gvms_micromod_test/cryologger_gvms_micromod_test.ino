/*
    Title:    Cryologger - Glacier Velocity Measurement System (GVMS) v2.0 Prototype
    Date:     March 19, 2021
    Author:   Adam Garbo

    Components:
    - SparkFun Artemis Processor
    - SparkFun MicroMod Data Logging Carrier Board
    - SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic)
    - SparkFun Buck-Boost Converter

    Comments:
    - Minimal code example to test system
*/

// ----------------------------------------------------------------------------
// Libraries
// ----------------------------------------------------------------------------

#include <RTC.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
#include <SdFat.h>                                // https://github.com/greiman/SdFat
#include <SPI.h>
#include <WDT.h>

// -----------------------------------------------------------------------------
// Debugging
// -----------------------------------------------------------------------------
#define DEBUG       true   // Output debug messages to Serial Monitor
#define DEBUG_GNSS  true   // Output GNSS information to Serial Monitor

// ----------------------------------------------------------------------------
// Pin definitions
// ----------------------------------------------------------------------------

#define PIN_PWC_POWER     G1
#define PIN_QWIIC_POWER   G2
#define PIN_SD_CS         CS

// ----------------------------------------------------------------------------
// Object instantiations
// ----------------------------------------------------------------------------
APM3_RTC          rtc;
APM3_WDT          wdt;
SdFs              sd;
FsFile            file;
SFE_UBLOX_GNSS    gnss;

// ----------------------------------------------------------------------------
// User defined global variable declarations
// ----------------------------------------------------------------------------
byte          sleepAlarmMinutes     = 30; // Rolling minutes alarm
byte          sleepAlarmHours       = 0;  // Rolling hours alarm
byte          loggingAlarmMinutes   = 30; // Rolling minutes alarm
byte          loggingAlarmHours     = 0;  // Rolling minhoursutes alarm
byte          sleepAlarmMode        = 5;  // Alarm match on hundredths, seconds, minutes
byte          loggingAlarmMode      = 5;  // Alarm match on hundredths, seconds, minutes
unsigned int  gnssTimeout           = 5;  // Timeout for GNSS signal acquisition (minutes)



// ----------------------------------------------------------------------------
// Global variable declarations
// ----------------------------------------------------------------------------
const int     sdWriteSize         = 512;    // Write data to SD in blocks of 512 bytes
const int     fileBufferSize      = 16384;  // Allocate 16 KB RAM for UBX message storage
volatile bool alarmFlag           = false;  // Flag for alarm interrupt service routine
volatile bool loggingFlag         = false;  // Flag to determine if data logging should start/stop
volatile bool watchdogFlag        = false;  // Flag for Watchdog Timer interrupt service routine
volatile int  watchdogCounter     = 0;      // Counter for Watchdog Timer interrupts
bool          resetFlag           = 0;      // Flag to force system reset using Watchdog Timer
bool          rtcSyncFlag         = false;  // Flag to indicate if the RTC was synced with the GNSS
char          fileName[30]        = "";     // Keep a record of this file name so that it can be re-opened upon wakeup from sleep
unsigned int  maxBufferBytes      = 0;      // Maximum value of file buffer
unsigned long previousMillis      = 0;      // Global millis() timer
unsigned long bytesWritten        = 0;      // Counter for tracking number of bytes written to microSD

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------
void setup()
{
  // Pin assignments
  pinMode(PIN_QWIIC_POWER, OUTPUT);
  pinMode(PIN_PWC_POWER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  qwiicPowerOff(); // Disable power to Qwiic connector
  peripheralPowerOn(); // Enable power to peripherials

  SPI.begin(); // Initialize SPI

  Serial.begin(115200); // Open Serial port
  //while (!Serial); // Wait for user to open Serial Monitor
  blinkLed(2, 1000); // Non-blocking delay to allow user to open Serial Monitor

  Serial.println("Cryologger - Glacier Velocity Measurement System");

  printDateTime();      // Print RTC's date and time

  // Configure devices
  configureWdt();       // Configure and start Watchdog Timer (WDT)
  configureGnss();      // Configure GNSS receiver
  syncRtc();            // Acquire GNSS fix and synchronize RTC with GNSS
  configureSd();        // Configure microSD
  configureRtc();       // Configure real-time clock (RTC) alarm

  Serial.println("Info: Datetime is "); printDateTime();
  Serial.println("Info: Initial RTC alarm set for "); printAlarm();

  // Blink LED to indicate completion of setup
  blinkLed(10, 100);
}

// ----------------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------------
void loop()
{
  // Check if alarm flag is set
  if (alarmFlag)
  {
    // Read RTC
    readRtc();

    // Clear alarm flag
    alarmFlag = false;

    DEBUG_PRINT("Info: Alarm trigger "); printDateTime();

    // Toggle logging flag
    loggingFlag = !loggingFlag;

    // Perform measurements
    if (loggingFlag)
    {
      setLoggingAlarm();    // Set logging duration

      peripheralPowerOn();  // Enable power to peripherals
      configureSd();        // Configure microSD

      blinkLed(2, 1000);    // Delay to allow u-blox receiver to boot-up
      configureGnss();      // Configure u-blox receiver
      logGnss();            // Log data
    }

    // Clear logging alarm flag
    alarmFlag = false;

    // Set the next RTC alarm
    setSleepAlarm();
  }

  // Check for watchdog interrupt
  if (watchdogFlag)
  {
    petDog(); // Restart watchdog timer
  }

  // Blink LED
  blinkLed(1, 25);

  // Enter deep sleep
  goToSleep();
}

// ----------------------------------------------------------------------------
// Interupt Service Routines (ISR)
// ----------------------------------------------------------------------------

// Interrupt handler for the RTC
extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt
  //rtc.clearInterrupt();
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Set alarm flag
  alarmFlag = true;
}

// Interrupt handler for the watchdog timer
extern "C" void am_watchdog_isr(void)
{
  // Clear the watchdog interrupt
  wdt.clear();

  // Perform system reset after 10 watchdog interrupts (should not occur)
  if (watchdogCounter < 10 )
  {
    wdt.restart(); // Restart the watchdog timer
  }
  else
  {
    wdt.stop(); // Stop the watchdog timer
    while (1)
    {
      blinkLed(2, 250);
      blinkLed(3, 1000);
    }
  }
  watchdogFlag = true; // Set the watchdog flag
  watchdogCounter++; // Increment watchdog interrupt counter
}
