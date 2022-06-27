/*
    Title:    Cryologger - Glacier Velocity Tracker (GVT) v2.1.0
    Date:     June 27, 2022
    Author:   Adam Garbo

    Components:
    - SparkFun Artemis Processor
    - SparkFun MicroMod Data Logging Carrier Board
    - SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic)

    Dependencies:
    - Apollo3 Core v1.2.3
    - SparkFun u-blox GNSS Arduino Library v2.2.7
    - SparkFun Qwiic OLED Arduino Library v1.0.5
    - SdFat v2.1.2

    Comments:
    - Code is currently configured for long-term measurements of glacier motion
    and is set to be deployed during the 2022 Ellesmere Island field season.
*/

// ----------------------------------------------------------------------------
// Libraries
// ----------------------------------------------------------------------------
#include <RTC.h>
#include <SdFat.h>                                // https://github.com/greiman/SdFat
#include <SparkFun_Qwiic_OLED.h>                  // https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library
#include <SPI.h>
#include <WDT.h>
#include <Wire.h>

// ----------------------------------------------------------------------------
// Define unique identifier
// ----------------------------------------------------------------------------
#define CRYOLOGGER_ID 0

// -----------------------------------------------------------------------------
// Debugging macros
// -----------------------------------------------------------------------------
#define DEBUG       true  // Output debug messages to Serial Monitor
#define DEBUG_GNSS  true  // Output GNSS information to Serial Monitor
#define OLED        true  // Output messages to OLED display

#if DEBUG
#define DEBUG_PRINT(x)            Serial.print(x)
#define DEBUG_PRINTLN(x)          Serial.println(x)
#define DEBUG_PRINT_DEC(x, y)     Serial.print(x, y)
#define DEBUG_PRINTLN_DEC(x, y)   Serial.println(x, y)
#define DEBUG_WRITE(x)            Serial.write(x)

#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT_DEC(x, y)
#define DEBUG_PRINTLN_DEC(x, y)
#define DEBUG_WRITE(x)
#endif

// ----------------------------------------------------------------------------
// Pin definitions
// ----------------------------------------------------------------------------
#define PIN_MICROSD_POWER   33  // G1
#define PIN_QWIIC_POWER     34  // G2 
#define PIN_SD_CS           41  // CS

// ----------------------------------------------------------------------------
// Object instantiations
// ----------------------------------------------------------------------------
APM3_RTC          rtc;
APM3_WDT          wdt;
SdFs              sd;
FsFile            logFile;
FsFile            debugFile;
//QwiicMicroOLED    oled;       // I2C address: 0x3C
QwiicNarrowOLED   oled;       // I2C address: 0x3C
SFE_UBLOX_GNSS    gnss;       // I2C address: 0x42

// ----------------------------------------------------------------------------
// User defined logging/sleeping variables
// ----------------------------------------------------------------------------

// Logging modes
// 1: Daily logging (e.g., 3 hours each day between 19:00-22:00)
// 2: Rolling logging (e.g., 2 hours logging, 2 hours sleep for 3, repeat)
// 3: Continuous logging (e.g., new logfiles created each day at 00:00)
byte          loggingMode           = 3;    // 1: daily, 2: rolling, 3: 24-hour

// Daily alarm
byte          loggingStartTime      = 17;   // Logging start hour (UTC)
byte          loggingStopTime       = 20;   // Logging end hour (UTC)

// Rolling alarm
byte          loggingAlarmMinutes   = 0;    // Rolling minutes alarm
byte          loggingAlarmHours     = 1;    // Rolling hours alarm
byte          sleepAlarmMinutes     = 0;    // Rolling minutes alarm
byte          sleepAlarmHours       = 1;    // Rolling hours alarm

// Manual alarm modes (for debugging only)
byte          loggingAlarmMode      = 4;    // Logging alarm mode
byte          sleepAlarmMode        = 4;    // Sleep alarm mode
byte          initialAlarmMode      = 4;    // Initial alarm mode

// ----------------------------------------------------------------------------
// Global variable declarations
// ----------------------------------------------------------------------------
const int     sdWriteSize         = 512;          // Write data to SD in blocks of 512 bytes
const int     fileBufferSize      = 16384;        // Allocate 16 KB RAM for UBX message storage
volatile bool alarmFlag           = false;        // Flag for alarm interrupt service routine
volatile bool wdtFlag             = false;        // Flag for WDT interrupt service routine
volatile int  wdtCounter          = 0;            // Counter for WDT interrupts
volatile int  wdtCounterMax       = 0;            // Counter for max WDT interrupts
bool          sleepFlag           = 0;            // Flag to place system into long-duration sleep mode
bool          gnssConfigFlag      = true;         // Flag to indicate whether to configure the u-blox module
bool          rtcSyncFlag         = false;        // Flag to indicate if RTC has been synced with GNSS
char          logFileName[30]     = "";           // Log file name
char          debugFileName[20]   = "";           // Debug log file name
char          dateTimeBuffer[25]  = "";           // Global buffer to store datetime information
unsigned int  debugCounter        = 0;            // Counter to track number of recorded debug messages
unsigned int  gnssTimeout         = 5;            // Timeout for GNSS signal acquisition (minutes)
unsigned int  maxBufferBytes      = 0;            // Maximum value of file buffer
unsigned long previousMillis      = 0;            // Global millis() timer
unsigned long bytesWritten        = 0;            // Counter for tracking bytes written to microSD
unsigned long syncFailCounter     = 0;            // microSD logfile synchronize failure counter
unsigned long writeFailCounter    = 0;            // microSD logfile write failure counter
unsigned long closeFailCounter    = 0;            // microSD logfile close failure counter
unsigned long logStartTime        = 0;            // Global counter to track elapsed logging duration
long          rtcDrift            = 0;            // Counter for drift of RTC
int           reading             = 0;            // Battery voltage analog reading

// ----------------------------------------------------------------------------
// Unions/structures
// ----------------------------------------------------------------------------

// Union to store online/offline states
struct struct_online
{
  bool microSd  = false;
  bool gnss     = false;
  bool oled     = false;
  bool logGnss  = false;
  bool logDebug = false;
} online;

// Union to store loop timers
struct struct_timer
{
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
// Setup
// ----------------------------------------------------------------------------
void setup()
{
  // Pin assignments
  pinMode(PIN_QWIIC_POWER, OUTPUT);
  pinMode(PIN_MICROSD_POWER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Enable power to Qwiic connector
  qwiicPowerOn();

  // Enable power to peripherials
  peripheralPowerOn();

  Wire.begin();             // Initalize I2C
  Wire.setClock(400000);    // Set I2C clock speed to 400 kHz
  SPI.begin();              // Initialize SPI
  analogReadResolution(14); // Set ADC resolution to 14-bits

#if DEBUG
  Serial.begin(115200);   // Open Serial port
  //while (!Serial);        // Wait for user to open Serial Monitor
  blinkLed(2, 1000);      // Delay to allow user to open Serial Monitor
#endif

  // Configure OLED display
  configureOled();

  printLine();
  DEBUG_PRINT("Cryologger Glacier Velocity Tracker #"); DEBUG_PRINTLN(CRYOLOGGER_ID);
  printLine();
  printDateTime(); // Print RTC's current date and time
  DEBUG_PRINT("Voltage: "); DEBUG_PRINTLN(readVoltage()); // Print battery voltage

  // Display OLED message(s)
  displayWelcome();

  // Print logging configuration
  printLoggingSettings();

  // Configure devices
  configureWdt();         // Configure and start Watchdog Timer (WDT)
  configureGnss();        // Configure u-blox GNSS receiver
  syncRtc();              // Acquire GNSS fix and sync RTC with GNSS
  configureSd();          // Configure microSD
  createDebugFile();      // Create debug log file
  setInitialAlarm();      // Configure RTC and set initial alarm

  DEBUG_PRINT("Info: Datetime "); printDateTime();
  DEBUG_PRINT("Info: Initial alarm "); printAlarm();

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
    DEBUG_PRINT("Info: Alarm trigger "); printDateTime();

    // Configure logging
    readRtc();            // Get the RTC's alarm date and time
    setLoggingAlarm();    // Set logging alarm
    getLogFileName();     // Get timestamped log file name

    // Configure devices
    qwiicPowerOn();       // Enable power to Qwiic connector
    peripheralPowerOn();  // Enable power to peripherals
    resetOled();          // Configure OLED display
    configureSd();        // Configure microSD
    configureGnss();      // Configure u-blox GNSS
    syncRtc();            // Synchronize RTC

    // Log data
    logGnss();            // Log u-blox GNSS data
    logDebug();           // Log system debug information
    setSleepAlarm();      // Set sleep alarm
    printTimers();        // Log timers to debug file
  }

  // Check for watchdog interrupt
  if (wdtFlag)
  {
    petDog(); // Restart WDT
  }

  // Blink LED
  blinkLed(1, 100);

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
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Set alarm flag
  alarmFlag = true;
}

// Interrupt handler for the WDT
extern "C" void am_watchdog_isr(void)
{
  // Clear the WDT interrupt
  wdt.clear();

  // Perform system reset after 10 WDT interrupts (should not occur)
  if (wdtCounter < 10)
  {
    wdt.restart(); // Restart the WDT timer
  }

  wdtFlag = true; // Set the WDT flag
  wdtCounter++; // Increment WDT interrupt counter

  if (wdtCounter > wdtCounterMax)
  {
    wdtCounterMax = wdtCounter;
  }
}
