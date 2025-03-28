/*
  Cryologger - Glacier Velocity Tracker (GVT)
  Version: 3.0.1
  Date: March 21, 2025
  Date: March 28, 2025
  Author: Adam Garbo
  License: GPLv3. See license file for more information.

  Components:
  - SparkFun Artemis Processor
  - SparkFun MicroMod Data Logging Carrier Board
  - SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic)
  - SparkFun Qwiic OLED Display
  - Pololu 5V 600mA Step-Down Voltage Regulator D36V6F5

  Description:
  This sketch integrates the RTC, microSD, GNSS, OLED display, and WDT modules 
  to implement a glacier velocity measurement and logging system.
*/

// ----------------------------------------------------------------------------
// USER CONFIGURATION
// ----------------------------------------------------------------------------

// Device Identifier
char uid[20] = "GVT_25_XXX";  // Default unique identifier (UID)

// Select the default operation mode (for normal periods when NOT in seasonal)
#define OPERATION_MODE DAILY  // Options: DAILY, ROLLING, CONTINUOUS

// Daily mode parameters (only used if OPERATION_MODE == DAILY)
#define DAILY_START_HOUR 17   // Logging start hour (UTC)
#define DAILY_START_MINUTE 0  // Logging start minute (UTC)
#define DAILY_STOP_HOUR 20    // Logging stop hour (UTC)
#define DAILY_STOP_MINUTE 0   // Logging stop minute (UTC)

// Rolling mode parameters (only used if OPERATION_MODE == ROLLING)
#define ROLLING_AWAKE_HOURS 1    // Awake period (hours)
#define ROLLING_AWAKE_MINUTES 0  // Awake period (minutes)
#define ROLLING_SLEEP_HOURS 1    // Sleep period (hours)
#define ROLLING_SLEEP_MINUTES 0  // Sleep period (minutes)

// Seasonal logging override
// If ENABLED and the current date is within the seasonal window,
// we switch to CONTINUOUS mode automatically.
#define SEASONAL_LOGGING_MODE ENABLED  // ENABLED or DISABLED
#define SEASONAL_START_DAY 1           // Seasonal logging start day
#define SEASONAL_START_MONTH 6         // Seasonal logging start month
#define SEASONAL_END_DAY 30            // Seasonal logging stop day
#define SEASONAL_END_MONTH 9           // Seasonal logging stop month

// GNSS Satellite Signal configuration (0=DISABLE, 1=ENABLE)
#define GNSS_MEASUREMENT_RATE 15000
#define GNSS_GPS_ENABLED 1
#define GNSS_GLO_ENABLED 1
#define GNSS_GAL_ENABLED 1
#define GNSS_BDS_ENABLED 0
#define GNSS_SBAS_ENABLED 0
#define GNSS_QZSS_ENABLED 0

// ----------------------------------------------------------------------------
//  END OF USER CONFIGURATION
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Libraries                          Version     Comments
// ----------------------------------------------------------------------------
#include <ArduinoJson.h>              // 7.3.1
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
#define SOFTWARE_VERSION "3.0.1"
#define HARDWARE_VERSION "2.21"

// ----------------------------------------------------------------------------
// Debugging Macros
// ----------------------------------------------------------------------------
#define DEBUG true       // Enable Serial Monitor debug messages
#define DEBUG_GNSS true  // Enable Serial Monitor GNSS positional output
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
FsFile configFile;     // Configuration file on SD
FsFile logFile;        // Log file on SD
FsFile debugFile;      // Debug log file on SD
QwiicNarrowOLED oled;  // OLED display (I2C address: 0x3C)
SFE_UBLOX_GNSS gnss;   // GNSS receiver (I2C address: 0x42)

// ---------------------------------------------------------------------------
// Operation Mode Enums
// ---------------------------------------------------------------------------
enum OperationMode : uint8_t {
  DAILY = 1,
  ROLLING = 2,
  CONTINUOUS = 3
};

enum SeasonalMode : bool {
  DISABLED = 0,
  ENABLED = 1,
};

// ---------------------------------------------------------------------------
// Global Variables for Logging Modes & Times
// ---------------------------------------------------------------------------

// The user-chosen “normal” operation mode (DAILY/ROLLING/CONTINUOUS)
OperationMode operationMode = (OperationMode)OPERATION_MODE;

// We store a copy of that mode so we can revert after seasonal
OperationMode normalOperationMode = operationMode;

// The seasonal override (ENABLED or DISABLED)
SeasonalMode seasonalLoggingMode = (SeasonalMode)SEASONAL_LOGGING_MODE;

// Daily mode times
byte alarmStartHour = DAILY_START_HOUR;
byte alarmStartMinute = DAILY_START_MINUTE;
byte alarmStopHour = DAILY_STOP_HOUR;
byte alarmStopMinute = DAILY_STOP_MINUTE;

// Rolling mode times
byte alarmAwakeHours = ROLLING_AWAKE_HOURS;
byte alarmAwakeMinutes = ROLLING_AWAKE_MINUTES;
byte alarmSleepHours = ROLLING_SLEEP_HOURS;
byte alarmSleepMinutes = ROLLING_SLEEP_MINUTES;

// Seasonal window
byte alarmSeasonalStartDay = SEASONAL_START_DAY;
byte alarmSeasonalStartMonth = SEASONAL_START_MONTH;
byte alarmSeasonalEndDay = SEASONAL_END_DAY;
byte alarmSeasonalEndMonth = SEASONAL_END_MONTH;

// ----------------------------------------------------------------------------
// GNSS Measurement Rate & Satellite Signal Enables
// ----------------------------------------------------------------------------
unsigned int gnssMeasurementRate = GNSS_MEASUREMENT_RATE;
byte gnssGpsEnabled = GNSS_GPS_ENABLED;
byte gnssGloEnabled = GNSS_GLO_ENABLED;
byte gnssGalEnabled = GNSS_GAL_ENABLED;
byte gnssBdsEnabled = GNSS_BDS_ENABLED;
byte gnssSbasEnabled = GNSS_SBAS_ENABLED;
byte gnssQzssEnabled = GNSS_QZSS_ENABLED;

// ----------------------------------------------------------------------------
// Global Variables for Logging & System State
// ----------------------------------------------------------------------------
volatile bool alarmFlag = false;  // Set by RTC alarm ISR
volatile bool wdtFlag = false;    // Set by Watchdog Timer ISR
volatile int wdtCounter = 0;      // Count of WDT interrupts
volatile int wdtCounterMax = 0;   // Maximum WDT interrupt count observed

bool seasonalPowerInitFlag = false;  // Tracks if peripherals were restored for seasonal mode
bool gnssConfigFlag = true;          // Indicates if GNSS module needs reconfiguration
bool rtcSyncFlag = false;            // Indicates if RTC is synchronized with GNSS
bool firstTimeFlag = true;           // True during the first run

// Alarm mode settings for various states
byte alarmModeInitial = 4;  // Initial RTC alarm mode (default daily)
byte alarmModeLogging = 4;  // Alarm mode during logging
byte alarmModeSleep = 4;    // Alarm mode during sleep

// Variables to track date changes
byte dateCurrent = 0;
byte dateNew = 0;

// Buffers
char logFileName[100] = "";
char debugFileName[30] = "";
char dateTimeBuffer[30] = "";

// SD card write configuration
const int sdWriteSize = 512;       // Write block size (bytes)
const int fileBufferSize = 16384;  // Buffer size (16 KB) for UBX messages
float sdTotalMB = 0.0;
float sdFreeMB = 0.0;
float sdUsedMB = 0.0;
int sdFileCount = 0;

// Global variables to store GNSS firmware info
uint8_t gnssFirmwareVersionHigh = 0;
uint8_t gnssFirmwareVersionLow = 0;
const char* gnssFirmwareType = "";
uint8_t gnssProtocolVersionHigh = 0;
uint8_t gnssProtocolVersionLow = 0;
const char* gnssModuleName = "";
bool gnssInfoAvailable = false;  // Flag to track if retrieval was successful

// Counters and timers
unsigned long debugCounter = 0;    // Count of debug messages logged
unsigned int gnssTimeout = 300;    // GNSS acquisition timeout (default = 300 seconds)
unsigned long maxBufferBytes = 0;  // Maximum buffer size used
unsigned int reading = 0;          // Battery voltage reading (analog)
unsigned int fixCounter = 0;       // Count of GNSS fixes

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

#if DEBUG
  Serial.begin(115200);  // Initialize Serial for debugging.
  // while (!Serial);     // Optionally wait for Serial Monitor connection.
  blinkLed(2, 1000);  // Blink LED to signal startup.
#endif

  // Initialize pin modes for peripheral power control and LED indicator.
  pinMode(PIN_QWIIC_POWER, OUTPUT);
  pinMode(PIN_MICROSD_POWER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Power on I2C peripherals and other devices.
  qwiicPowerOn();
  peripheralPowerOn();

  // Initialize communication protocols.
  Wire.begin();              // Start I2C communications.
  Wire.setClock(400000);     // Set I2C clock to 400 kHz.
  SPI.begin();               // Start SPI communications.
  analogReadResolution(14);  // Set ADC resolution to 14 bits.

  // Output startup information.

  DEBUG_PRINTLN();
  printLine();
  DEBUG_PRINTLN("Cryologger - Glacier Velocity Tracker");
  printLine();
  DEBUG_PRINTLN("[Setup] Info: Initializing peripherals...");

  // Initialize peripherals.
  configureRtc();   // Set up the Real-Time Clock.
  configureOled();  // Set up the OLED display.
  configureWdt();   // Set up Watchdog Timer.
  configureSd();    // Set up microSD card.
  displaySdInfo();  //

  // Load configuration from microSD card.
  if (loadConfigFromSd()) {
    DEBUG_PRINTLN("[Setup] Info: Configuration loaded successfully.");
  } else {
    DEBUG_PRINTLN("[Setup] Info: Using fallback defaults.");
  }
  configureGnss();  // Set up GNSS receiver.
  displayGnssModuleInfo();

  printLine();
  DEBUG_PRINTLN("System Information");
  printLine();
  DEBUG_PRINT("Serial:");
  printTab(3);
  DEBUG_PRINTLN(uid);
  DEBUG_PRINT("Software Version:");
  printTab(1);
  DEBUG_PRINTLN(SOFTWARE_VERSION);
  DEBUG_PRINT("Hardware Version:");
  printTab(1);
  DEBUG_PRINTLN(HARDWARE_VERSION);
  DEBUG_PRINT("Datetime:");
  printTab(2);
  printDateTime();
  DEBUG_PRINT("Battery Voltage:");
  printTab(1);
  DEBUG_PRINTLN(readBattery());
  DEBUG_PRINT("Storage:");
  printTab(2);
  DEBUG_PRINT_DEC(sdUsedMB, 1);
  DEBUG_PRINT(" / ");
  DEBUG_PRINT_DEC(sdTotalMB, 1);
  DEBUG_PRINTLN(" MB");
  DEBUG_PRINT("File Count:");
  printTab(2);
  DEBUG_PRINTLN(sdFileCount);

  // Display welcome messages and logging configuration on OLED.
  //printSystemSettings();
  displayWelcome();
  printLoggingSettings();
  displayLoggingMode();
  displaySeasonalMode();
  printGnssSettings();

  // Configure additional devices and logging parameters.
  syncRtc();          // Synchronize RTC with GNSS.
  checkDate();        // Update the current date.
  createDebugFile();  // Create a debug log file on SD.
  setSleepAlarm();    // Set the RTC alarm based on operation mode.

  DEBUG_PRINT("[Setup] Info: Datetime ");
  printDateTime();

  // Indicate setup is complete.
  displaySetupComplete();
}

// ----------------------------------------------------------------------------
// Main Loop
// ----------------------------------------------------------------------------
void loop() {
  // Process RTC alarm events.
  if (alarmFlag) {
    DEBUG_PRINT("[Main] Info: Alarm trigger ");
    printDateTime();

    // Update RTC and logging configuration.
    readRtc();          // Refresh current RTC time.
    setLoggingAlarm();  // Schedule the wake-up alarm (end of logging period).
    getLogFileName();   // Generate a new log file name with a timestamp.

    // If we have just transitioned to seasonal mode, restore power
    if (operationMode == CONTINUOUS && !seasonalPowerInitFlag) {
      restorePeripherals();
      seasonalPowerInitFlag = true;
    }

    // If not in continuous mode, reinitialize peripherals
    if (operationMode != CONTINUOUS) {
      restorePeripherals();
    }

    // If the date has changed (daily logging), re-sync the RTC.
    if (checkDate()) {
      DEBUG_PRINTLN("[Main] Info: Daily RTC sync required...");
      syncRtc();
    };

    logGnss();        // Log GNSS data.
    logDebug();       // Log system debug information.
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
// Interrupt Service Routines (ISRs).
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