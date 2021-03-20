/*
    Title:    Cryologger - Glacier Velocity Measurement System (GVMS) v2.0 Prototype
    Date:     March 20, 2021
    Author:   Adam Garbo

    Components:
    - SparkFun Artemis Processor
    - SparkFun MicroMod Data Logging Carrier Board
    - SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic)
    - SparkFun Buck-Boost Converter

    Comments:
    - Code includes new consolidated logGnss() function that eliminates
    createLogFile()

    Test:
    - 20210319_1
    - Deployed 2021-03-19 16:37:00 UTC
    - Sleep duration: 5 minutes
    - Logging duration: 30 minutes
    - SanDisk 64 GB microSD card formatted as ExFat

    Results:
    - Debug file write fails
    - First data logging file did not complete writing
    - First data log file did not have timestamps updated
    - Data logging failed after 3 files
    - Watchdog failure
*/

// ----------------------------------------------------------------------------
// Libraries
// ----------------------------------------------------------------------------

#include <RTC.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
#include <SdFat.h>                                // https://github.com/greiman/SdFat
#include <SPI.h>
#include <TimeLib.h>                              // https://github.com/PaulStoffregen/Time
#include <WDT.h>
#include <Wire.h>                                 // https://www.arduino.cc/en/Reference/Wire

// -----------------------------------------------------------------------------
// Debugging macros
// -----------------------------------------------------------------------------
#define DEBUG           true   // Output debug messages to Serial Monitor
#define DEBUG_GNSS      false   // Output GNSS information to Serial Monitor

#if DEBUG
#define DEBUG_PRINT(x)            Serial.print(x)
#define DEBUG_PRINTLN(x)          Serial.println(x)
#define DEBUG_PRINT_HEX(x)        Serial.print(x, HEX)
#define DEBUG_PRINTLN_HEX(x)      Serial.println(x, HEX)
#define DEBUG_PRINT_DEC(x, y)     Serial.print(x, y)
#define DEBUG_PRINTLN_DEC(x, y)   Serial.println(x, y)
#define DEBUG_WRITE(x)            Serial.write(x)

#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT_HEX(x)
#define DEBUG_PRINTLN_HEX(x)
#define DEBUG_PRINT_DEC(x, y)
#define DEBUG_PRINTLN_DEC(x, y)
#define DEBUG_WRITE(x)
#endif

// ----------------------------------------------------------------------------
// Pin definitions
// ----------------------------------------------------------------------------

#define PIN_VBAT          A0
#define PIN_PWC_POWER     33  // G1
#define PIN_QWIIC_POWER   34  // G2
#define PIN_SD_CS         41  // CS

// ----------------------------------------------------------------------------
// Object instantiations
// ----------------------------------------------------------------------------
APM3_RTC          rtc;
APM3_WDT          wdt;
SdFs              sd;
FsFile            logFile;
FsFile            debugFile;
SFE_UBLOX_GNSS    gnss;

// ----------------------------------------------------------------------------
// User defined global variable declarations
// ----------------------------------------------------------------------------
byte          sleepAlarmMinutes     = 30; // Minutes rolling alarm
byte          sleepAlarmHours       = 0;  // Hours rolling alarm
byte          loggingAlarmMinutes   = 0; // Minutes rolling alarm
byte          loggingAlarmHours     = 1;  // Hours rolling alarm
byte          sleepAlarmMode        = 5;  // Alarm match on hundredths, seconds, minutes
byte          loggingAlarmMode      = 4;  // Alarm match on hundredths, seconds, minutes
byte          initialAlarmMode      = 5;  // Initial alarm mode
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
volatile int  watchdogCounterMax  = 0;      // Max number of Watchdog Timer interrupts seen
bool          firstTimeFlag       = true;   // Flag to determine if the program is running for the first time
bool          gnssConfigFlag      = false;  // Flag to indicate if u-blox GNSS module has been configured
bool          gnssFixFlag         = false;  // Flag to indicate if a valid GNSS fix has been acquired
bool          resetFlag           = 0;      // Flag to force system reset using Watchdog Timer
bool          rtcSyncFlag         = false;  // Flag to indicate if the RTC was synced with the GNSS
char          logFileName[30]     = "";     // Keep a record of this file name so that it can be re-opened upon wakeup from sleep
char          debugFileName[30]   = "debug.csv";
long          rtcDrift            = 0;      // Flag to track drift of the RTC
unsigned int  sdPowerDelay        = 250;    // Delay before enabling/disabling power to microSD (milliseconds)
unsigned int  qwiicPowerDelay     = 2500;   // Delay after enabling power to Qwiic connector (milliseconds)
unsigned int  debugCounter        = 0;      // Counter to track number of recorded debug messages
unsigned int  maxBufferBytes      = 0;      // Maximum value of file buffer
unsigned long previousMillis      = 0;      // Global millis() timer
unsigned long unixtime            = 0;      // Unix epoch timestamp
unsigned long bytesWritten        = 0;      // Counter for tracking bytes written to microSD card
float         voltage             = 0.0;    // Battery voltage

// ----------------------------------------------------------------------------
// Unions/structures
// ----------------------------------------------------------------------------

// Union to store device online/offline states
struct struct_online
{
  bool gnss         = false;
  bool microSd      = false;
  bool sensors      = false;
  bool dataLogging  = false;
  bool debugLogging = false;
} online;

// Union to store loop timers
struct struct_timer
{
  unsigned long voltage;
  unsigned long wdt;
  unsigned long rtc;
  unsigned long microSd;
  unsigned long sensors;
  unsigned long gnss;
  unsigned long logGnss;
} timer;

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------
void setup()
{
  // Pin assignments
  pinMode(PIN_QWIIC_POWER, OUTPUT);
  pinMode(PIN_PWC_POWER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Disable power to Qwiic connector
  qwiicPowerOff();

  // Enable power to peripherials
  peripheralPowerOn();

  // Set analog resolution to 14-bits
  analogReadResolution(14);

  Wire.begin(); // Initialize I2C
  //Wire.setClock(400000); // Set I2C clock speed to 400 kHz
  SPI.begin(); // Initialize SPI

#if DEBUG
  Serial.begin(115200);
  //while (!Serial); // Wait for user to open Serial Monitor
  blinkLed(2, 1000); // Non-blocking delay to allow user to open Serial Monitor
#endif

  DEBUG_PRINTLN();
  printLine();
  DEBUG_PRINTLN("Cryologger - Glacier Velocity Measurement System v2.0");
  printLine();
  DEBUG_PRINT("Info: "); printDateTime(); // Print current RTC time at boot

  // Configure devices
  configureWdt();       // Configure and start Watchdog Timer (WDT)

  configureGnss();      // Configure GNSS receiver
  syncRtc();            // Acquire GNSS fix and synchronize RTC with GNSS
  configureSd();        // Configure microSD
  createDebugFile();    // Create debug log file
  configureSensors();   // Configure attached sensors
  configureRtc();       // Configure initial real-time clock (RTC) alarm

  DEBUG_PRINT("Info: Datetime is "); printDateTime();
  DEBUG_PRINT("Info: Initial alarm set for "); printAlarm();

  // Blink LED to indicate completion of setup
  blinkLed(5, 500);
  blinkLed(5, 100);
  blinkLed(5, 500);
}

// ----------------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------------
void loop()
{
  // Check if alarm flag is set or if program is running for the first time
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
      setLoggingAlarm();    // Set duration of data logging

      peripheralPowerOn();  // Enable power to peripherals
      configureSd();        // Configure microSD

      blinkLed(2, 1000);    // Delay to allow u-blox receiver to boot-up
      configureGnss();      // Configure u-blox receiver
      //syncRtc();            // Synchronize RTC (for long-term deployments)
      blinkLed(1, 1000);
      logData();            // Log UBX RAWX/SFRBX data

      printTimers();      // Print function execution timers
    }

    // Log system diagnostic information
    logDebugData();

    // Clear alarm flag
    alarmFlag = false;

    // Set the next RTC alarm
    setSleepAlarm();
  }

  // Check for watchdog interrupt
  if (watchdogFlag)
  {
    petDog(); // Restart the watchdog timer
  }

  // Blink LED during sleep
  blinkLed(1, 25);

  // Enter deep sleep and await WDT or RTC alarm interrupt
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

  // Perform system reset after 10 watchdog interrupts (should never occur)
  if (watchdogCounter < 10 )
  {
    wdt.restart(); // Restart the watchdog timer
  }
  else
  {
    wdt.stop(); // Stop watchdog timer (debugging only)
    peripheralPowerOff(); // Disable power to peripherals
    // Blink LED
    while (1)
    {
      blinkLed(2, 250);
      blinkLed(3, 1000);
    }
  }
  watchdogFlag = true; // Set the watchdog flag
  watchdogCounter++; // Increment watchdog interrupt counter

  if (watchdogCounter > watchdogCounterMax)
  {
    watchdogCounterMax = watchdogCounter;
  }
}
