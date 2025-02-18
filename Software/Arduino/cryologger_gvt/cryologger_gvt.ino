/*
  Cryologger - Glacier Velocity Tracker (GVT)
  Version: 2.4.0
  Date: February 16, 2025
  Author: Adam Garbo
  License: GPLv3. See license file for more information.

  Components:
  - SparkFun Artemis Processor
  - SparkFun MicroMod Data Logging Carrier Board
  - SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic)
  - SparkFun Qwiic OLED Display
  - Pololu 5V 600mA Step-Down Voltage Regulator D36V6F5

  Description:
  This sketch integrates the RTC, microSD, GNSS, OLED display, and WDT modules to
  implement a glacier velocity measurement and logging system.
*/

// ----------------------------------------------------------------------------
// Libraries                          Version     Comments
// ----------------------------------------------------------------------------
#include <RTC.h>                      // 1.2      Apollo3 Core v1.2.3
#include <SdFat.h>                    // 2.3.0
#include <SparkFun_Qwiic_OLED.h>      // 1.0.13
#include <SparkFun_u-blox_GNSS_v3.h>  // 3.1.8
#include <SPI.h>                      //          Apollo3 Core v1.2.3
#include <WDT.h>                      // 0.1      Apollo3 Core v1.2.3
#include <Wire.h>                     //          Apollo3 Core v1.2.3

// ----------------------------------------------------------------------------
// Software & Hardware Versions
// ----------------------------------------------------------------------------
#define SOFTWARE_VERSION "2.4.0"
#define HARDWARE_VERSION "2.21"

// ----------------------------------------------------------------------------
// Device Identifier
// ----------------------------------------------------------------------------
#define SERIAL "GVT-25-001" // Unique device identifer to distinguish log files

// ----------------------------------------------------------------------------
// Debugging Macros
// ----------------------------------------------------------------------------
#define DEBUG true       // Enable Serial Monitor debug messages
#define DEBUG_GNSS true  // Enable GNSS-related debug messages
#define OLED true        // Enable OLED display messages

#if DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT_DEC(x, y) Serial.print(x, y)
#define DEBUG_PRINTLN_DEC(x, y) Serial.println(x, y)
#define DEBUG_WRITE(x) Serial.write(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT_DEC(x, y)
#define DEBUG_PRINTLN_DEC(x, y)
#define DEBUG_WRITE(x)
#endif

// ----------------------------------------------------------------------------
// Pin Definitions
// ----------------------------------------------------------------------------
#define PIN_MICROSD_POWER 33  // MicroSD power control (G1)
#define PIN_QWIIC_POWER 34    // Qwiic connector power control (G2)
#define PIN_SD_CS 41          // SD card Chip Select

// ----------------------------------------------------------------------------
// Peripheral Object Instantiations
// ----------------------------------------------------------------------------
APM3_RTC rtc;          // Real-Time Clock
APM3_WDT wdt;          // Watchdog Timer
SdFs sd;               // SD card filesystem
FsFile logFile;        // Log file on SD
FsFile debugFile;      // Debug log file on SD
QwiicNarrowOLED oled;  // OLED display (I2C address: 0x3C)
SFE_UBLOX_GNSS gnss;   // GNSS receiver (I2C address: 0x42)

// ----------------------------------------------------------------------------
// Operation Modes (Logging Modes)
// ----------------------------------------------------------------------------
enum OperationMode : uint8_t {
  DAILY = 1,
  ROLLING = 2,
  CONTINUOUS = 3
};

OperationMode operationMode = DAILY;                // Select operation mode
OperationMode normalOperationMode = operationMode;  // Stores initial logging mode

// Summer mode flag (forces continuous logging during summer period)
bool summerMode = true;

// ----------------------------------------------------------------------------
// Alarm & Logging Time Configurations
// ----------------------------------------------------------------------------
// Daily mode: log from 14:00 to 17:00 UTC
byte alarmStartHour = 23;   // Logging start hour (UTC)
byte alarmStartMinute = 55;  // Logging start minute (UTC)
byte alarmStopHour = 23;    // Logging stop hour (UTC)
byte alarmStopMinute = 57;   // Logging stop minute (UTC)

// Rolling mode: define awake/sleep durations
byte alarmAwakeHours = 1;    // Awake period (hours)
byte alarmAwakeMinutes = 0;  // Awake period (minutes)
byte alarmSleepHours = 1;    // Sleep period (hours)
byte alarmSleepMinutes = 0;  // Sleep period (minutes)

// Summer logging period (e.g., June 1st to August 31st)
byte alarmSummerStartDay = 1;
byte alarmSummerStartMonth = 6;
byte alarmSummerEndDay = 31;
byte alarmSummerEndMonth = 8;

// ----------------------------------------------------------------------------
// Global Variables for Logging & System State
// ----------------------------------------------------------------------------
volatile bool alarmFlag = false;  // Set by RTC alarm ISR
volatile bool wdtFlag = false;    // Set by Watchdog Timer ISR
volatile int wdtCounter = 0;      // Count of WDT interrupts
volatile int wdtCounterMax = 0;   // Maximum WDT interrupt count observed

bool summerPowerInitFlag = false; // Tracks if peripherals were restored for summer mode
bool gnssConfigFlag = true;       // Indicates if GNSS module needs reconfiguration
bool rtcSyncFlag = false;         // Indicates if RTC is synchronized with GNSS
bool firstTimeFlag = true;        // True during the first run

// Alarm mode settings for various states
byte alarmModeInitial = 4;  // Initial RTC alarm mode (default daily)
byte alarmModeLogging = 4;  // Alarm mode during logging
byte alarmModeSleep = 4;    // Alarm mode during sleep

// Variables to track date changes
byte dateCurrent = 0;
byte dateNew = 0;

// Buffers
char logFileName[30] = "";
char debugFileName[20] = "";
char dateTimeBuffer[30] = "";

// SD card write configuration
const int sdWriteSize = 512;       // Write block size (bytes)
const int fileBufferSize = 16384;  // Buffer size (16 KB) for UBX messages

// Counters and timers
unsigned int debugCounter = 0;    // Count of debug messages logged
unsigned int gnssTimeout = 5;     // GNSS acquisition timeout (seconds)
unsigned int maxBufferBytes = 0;  // Maximum buffer size used
unsigned int reading = 0;         // Battery voltage reading (analog)
unsigned int fixCounter = 0;      // Count of GNSS fixes

unsigned long previousMillis = 0;    // For millis()-based timing
unsigned long bytesWritten = 0;      // Total bytes written to microSD
unsigned long syncFailCounter = 0;   // Count of RTC synchronization failures
unsigned long writeFailCounter = 0;  // Count of SD write failures
unsigned long closeFailCounter = 0;  // Count of SD file close failures
unsigned long logStartTime = 0;      // Logging session start time (millis)
long rtcDrift = 0;                   // RTC drift measurement

// ----------------------------------------------------------------------------
// Structures for System Status and Timers
// ----------------------------------------------------------------------------
struct OnlineStatus {
  bool microSd = false;
  bool gnss = false;
  bool oled = false;
  bool logGnss = false;
  bool logDebug = false;
} online;

struct Timer {
  unsigned long wdt;
  unsigned long rtc;
  unsigned long microSd;
  unsigned long voltage;
  unsigned long sensors;
  unsigned long gnss;
  unsigned long syncRtc;
  unsigned long logDebug;
  unsigned long logGnss;
} timer;

// ----------------------------------------------------------------------------
// Setup Function
// ----------------------------------------------------------------------------
void setup() {
  // Initialize pin modes for peripheral power control and LED indicator.
  pinMode(PIN_QWIIC_POWER, OUTPUT);
  pinMode(PIN_MICROSD_POWER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Power on I2C peripherals and other devices.
  qwiicPowerOn();
  peripheralPowerOn();

  // Initialize communication protocols.
  Wire.begin();              // Start I2C
  Wire.setClock(400000);     // Set I2C clock to 400 kHz
  SPI.begin();               // Start SPI
  analogReadResolution(14);  // Set ADC resolution to 14 bits

#if DEBUG
  Serial.begin(115200);  // Initialize Serial for debugging.
  // while (!Serial);     // Optionally wait for Serial Monitor connection.
  blinkLed(2, 1000);  // Blink LED to signal startup.
#endif

  // Initialize peripherals.
  configureRtc();   // Set up the Real-Time Clock.
  configureOled();  // Set up the OLED display.

  // Output startup information.
  DEBUG_PRINTLN();
  printLine();
  DEBUG_PRINTLN(F("Cryologger - Glacier Velocity Tracker"));
  printLine();
  DEBUG_PRINT(F("Serial:"));
  printTab(3);
  DEBUG_PRINTLN(SERIAL);
  DEBUG_PRINT(F("Software Version:"));
  printTab(1);
  DEBUG_PRINTLN(SOFTWARE_VERSION);
  DEBUG_PRINT(F("Hardware Version:"));
  printTab(1);
  DEBUG_PRINTLN(HARDWARE_VERSION);
  DEBUG_PRINT(F("Datetime:"));
  printTab(2);
  printDateTime();
  DEBUG_PRINT(F("Battery Voltage:"));
  printTab(1);
  DEBUG_PRINTLN(readBattery());

  // Display welcome messages and logging configuration on OLED.
  displayWelcome();
  printLoggingSettings();
  displayLoggingMode();

  // Configure additional devices and logging parameters.
  configureWdt();     // Set up Watchdog Timer.
  configureSd();      // Set up microSD card.
  configureGnss();    // Set up GNSS receiver.
  syncRtc();          // Synchronize RTC with GNSS.
  checkDate();        // Update the current date.
  createDebugFile();  // Create a debug log file on SD.
  setInitialAlarm();  // Set the initial RTC alarm based on operation mode.

  DEBUG_PRINT(F("[Main] Info: Datetime "));
  printDateTime();
  DEBUG_PRINT(F("[Main] Info: Initial alarm "));
  printAlarm();

  // Indicate that setup is complete.
  displaySetupComplete();
}

// ----------------------------------------------------------------------------
// Main Loop
// ----------------------------------------------------------------------------
void loop() {
  // Process RTC alarm events.
  if (alarmFlag) {
    DEBUG_PRINT(F("[Main] Info: Alarm trigger "));
    printDateTime();

    // Update RTC and logging configuration.
    readRtc();         // Refresh current RTC time.
    setAwakeAlarm();   // Schedule the wake-up alarm (end of logging period).
    getLogFileName();  // Generate a new log file name with a timestamp.

    // If we have just transitioned to summer continuous mode, restore power
    if (operationMode == CONTINUOUS && !summerPowerInitFlag) {
      restorePeripherals();
      summerPowerInitFlag = true;
    }

    // If not in continuous mode, reinitialize peripherals
    if (operationMode != CONTINUOUS) {
      restorePeripherals();
    }

    // If the date has changed (daily logging), re-sync the RTC.
    checkDate();
    if (dateCurrent != dateNew) {
      DEBUG_PRINTLN(F("[Main] Info: Daily RTC sync required..."));
      syncRtc();
      dateCurrent = dateNew;
      checkDate();
    }

    // Log GNSS data and system debug information.
    logGnss();
    logDebug();
    setSleepAlarm();  // Schedule the sleep alarm (for low-power mode).
    printTimers();    // Output timing metrics.
    clearTimers();    // Reset timers for the next cycle.
  }

  // Service the Watchdog Timer if needed.
  if (wdtFlag) {
    petDog();  // Reset the WDT timer.
  }

  // Blink LED as a heartbeat indicator.
  blinkLed(1, 100);

  // Enter deep sleep to conserve power until the next event.
  goToSleep();
}

// ----------------------------------------------------------------------------
// Interrupt Service Routines (ISRs)
// ----------------------------------------------------------------------------
// RTC Alarm ISR.
extern "C" void am_rtc_isr(void) {
  // Clear the RTC alarm interrupt flag.
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);
  alarmFlag = true;
}

// Watchdog Timer ISR.
extern "C" void am_watchdog_isr(void) {
  // Clear the Watchdog Timer interrupt flag.
  wdt.clear();

  // Restart the WDT timer if under the threshold.
  if (wdtCounter < 10) {
    wdt.restart();
  }
  wdtFlag = true;  // Signal WDT event to main loop.
  wdtCounter++;    // Increment WDT interrupt counter.

  if (wdtCounter > wdtCounterMax) {
    wdtCounterMax = wdtCounter;
  }
}
