/*
    Title:    Cryologger - Glacier Velocity Measurement System (GVMS) v2.0
    Date:     March 20, 2021
    Author:   Adam Garbo

    Components:
    - SparkFun Artemis Processor
    - SparkFun MicroMod Data Logging Carrier Board
    - SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic)

    Comments:
    - Minimal debugging code for testing purposes
*/

// ----------------------------------------------------------------------------
// Libraries
// ----------------------------------------------------------------------------
#include <RTC.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
#include <SdFat.h>                                // https://github.com/greiman/SdFat
#include <SPI.h>
#include <WDT.h>
#include <Wire.h>

// -----------------------------------------------------------------------------
// Debugging
// -----------------------------------------------------------------------------
#define DEBUG       true  // Output debug messages to Serial Monitor
#define DEBUG_GNSS  true  // Output GNSS information to Serial Monitor

// ----------------------------------------------------------------------------
// Pin definitions
// ----------------------------------------------------------------------------
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
byte          loggingStart          = 18;   // Logging start time (hour)
byte          loggingEnd            = 21;   // Logging end time (hour)
byte          sleepAlarmMinutes     = 1;    // Rolling minutes alarm
byte          sleepAlarmHours       = 0;    // Rolling hours alarm
byte          loggingAlarmMinutes   = 2;    // Rolling minutes alarm
byte          loggingAlarmHours     = 0;    // Rolling hours alarm
byte          sleepAlarmMode        = 5;    // Sleep alarm mode
byte          loggingAlarmMode      = 5;    // Logging alarm mode
byte          initialAlarmMode      = 6;    // Initial alarm mode
bool          sleepFlag             = true; // Flag to indicate whether to sleep between new log files
bool          gnssConfigFlag        = true; // Flag to indicate whether to configure the u-blox module
unsigned int  gnssTimeout           = 5;    // Timeout for GNSS signal acquisition (minutes)

// ----------------------------------------------------------------------------
// Global variable declarations
// ----------------------------------------------------------------------------
const int     sdWriteSize         = 512;          // Write data to SD in blocks of 512 bytes
const int     fileBufferSize      = 16384;        // Allocate 16 KB RAM for UBX message storage
volatile bool alarmFlag           = false;        // Flag for alarm interrupt service routine
volatile bool wdtFlag             = false;        // Flag for watchdog timer interrupt service routine
volatile int  wdtCounter          = 0;            // Counter for watchdog timer interrupts
volatile int  wdtCounterMax       = 0;            // Counter for max watchdog timer interrupts
bool          firstTimeFlag       = true;         // Flag to track configuration of u-blox GNSS
bool          loggingFlag         = true;         //
bool          resetFlag           = false;        // Flag to force system reset using watchdog timer
bool          rtcSyncFlag         = false;        // Flag to indicate if the RTC was synced with the GNSS
char          logFileName[30]     = "";           // Log file name
char          debugFileName[10]   = "debug.csv";  // Debug log file name
unsigned int  debugCounter        = 0;            // Counter to track number of recorded debug messages
unsigned int  maxBufferBytes      = 0;            // Maximum value of file buffer
unsigned long previousMillis      = 0;            // Global millis() timer
unsigned long bytesWritten        = 0;            // Counter for tracking bytes written to microSD
long          rtcDrift            = 0;            // Counter for drift of RTC

// ----------------------------------------------------------------------------
// Unions/structures
// ----------------------------------------------------------------------------

// Union to store online/offline states
struct struct_online
{
  bool microSd  = false;
  bool gnss     = false;
  bool sensors  = false;
  bool logGnss  = false;
  bool logDebug = false;
} online;

// Union to store loop timers
struct struct_timer
{
  unsigned long wdt;
  unsigned long rtc;
  unsigned long microSd;
  unsigned long battery;
  unsigned long sensors;
  unsigned long gnss;
  unsigned long syncRtc;
  unsigned long logDebug;
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

  qwiicPowerOn();       // Enable power to Qwiic connector
  peripheralPowerOn();  // Enable power to peripherials

  Wire.begin();         // Initalize I2C
  Wire.setClock(400000); // Set I2C clock speed to 400 kHz
  SPI.begin();          // Initialize SPI

  Serial.begin(115200); // Open Serial port
  //while (!Serial);      // Wait for user to open Serial Monitor
  delay(2000);          // Delay to allow user to open Serial Monitor

  printLine();
  Serial.println("Cryologger - Glacier Velocity Measurement System");
  printLine();

  printDateTime();      // Print RTC's current date and time

  // Configure devices
  configureWdt();       // Configure and start Watchdog Timer (WDT)
  configureGnss();      // Configure u-blox GNSS
  syncRtc();            // Acquire GNSS fix and sync RTC with GNSS
  configureSd();        // Configure microSD
  createDebugFile();    // Create debug log file
  configureRtc();       // Configure real-time clock (RTC) alarm

  Serial.print("Info: Datetime "); printDateTime();
  Serial.print("Info: Initial alarm "); printAlarm();

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

    Serial.print("Info: Alarm trigger "); printDateTime();

    // Check if program is running for first time or if sleep is enabled
    if (firstTimeFlag || sleepFlag)
    {
      firstTimeFlag = false;  // Clear flag
      qwiicPowerOn();         // Enable power to Qwiic connector
      peripheralPowerOn();    // Enable power to peripherals
      configureSd();          // Configure microSD
      configureGnss();        // Configure u-blox GNSS
      syncRtc();              // Synchronize RTC
    }

    // Log data
    setLoggingAlarm();  // Set logging duration alarm
    logGnss();          // Log u-blox GNSS data
    logDebug();         // Log system debug information

    // Check if sleep is enabled
    if (sleepFlag)
    {
      setSleepAlarm();  // Set sleep alarm
    }
    else
    {
      alarmFlag = true; // Force logging
    }
  }
  
  // Check for watchdog interrupt
  if (wdtFlag)
  {
    petDog(); // Restart watchdog timer
  }

  // Blink LED
  blinkLed(1, 25);

  // Check if sleep is enabled
  if (firstTimeFlag || sleepFlag)
  {
    goToSleep(); // Enter deep sleep
  }
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
  if (wdtCounter < 10)
  {
    wdt.restart(); // Restart the watchdog timer
  }
  else
  {
    while (1); // Force reset
  }
  wdtFlag = true; // Set the watchdog flag
  wdtCounter++; // Increment watchdog interrupt counter

  if (wdtCounter > wdtCounterMax)
  {
    wdtCounterMax = wdtCounter;
  }
}
