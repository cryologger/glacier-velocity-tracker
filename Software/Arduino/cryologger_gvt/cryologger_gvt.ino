/*
    Title:    Cryologger - Glacier Velocity Tracker (GVT)
    Version:  2.2
    Date:     April 2, 2023
    Author:   Adam Garbo

    Components:
    - SparkFun Artemis Processor
    - SparkFun MicroMod Data Logging Carrier Board
    - SparkFun GPS-RTK-SMA Breakout - ZED-F9P (Qwiic)
    - Pololu 5V 600mA Step-Down Voltage Regulator D36V6F5

    Dependencies:
    - Apollo3 Core v1.2.3
    - SparkFun u-blox GNSS v3 v3.0.6
    - SparkFun Qwiic OLED Arduino Library v1.0.5
    - SdFat v2.2.0

    Comments:
    - 
*/

// ----------------------------------------------------------------------------
// Libraries
// ----------------------------------------------------------------------------
#include <RTC.h>
#include <SdFat.h>                    // http://librarymanager/All#SdFat
#include <SparkFun_Qwiic_OLED.h>      // http://librarymanager/All#SparkFun_Qwiic_OLED_Arduino_Library
#include <SparkFun_u-blox_GNSS_v3.h>  // http://librarymanager/All#SparkFun_u-blox_GNSS_v3
#include <SPI.h>
#include <WDT.h>
#include <Wire.h>

// ----------------------------------------------------------------------------
// Define unique identifier
// ----------------------------------------------------------------------------
char        ID[5] = "TST";
const int   UNIT  = 1;

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
QwiicNarrowOLED   oled;       // I2C address: 0x3C
SFE_UBLOX_GNSS    gnss;       // I2C address: 0x42

// ----------------------------------------------------------------------------
// User defined logging/sleeping variables
// ----------------------------------------------------------------------------

// Logging modes
// 1: Daily logging (e.g., log 3 hours each day between 19:00-22:00)
// 2: Rolling logging (e.g., log for 2 hours, sleep for 3, repeat)
// 3: Continuous logging (e.g., new log file created each day at 00:00)
byte          loggingMode           = 1;    // 1: daily, 2: rolling, 3: 24-hour

// Daily alarm
byte          loggingStartTime      = 16;   // Logging start hour (UTC)
byte          loggingStopTime       = 17;   // Logging end hour (UTC)

// Rolling alarm
byte          loggingAlarmMinutes   = 0;    // Rolling minutes alarm
byte          loggingAlarmHours     = 1;    // Rolling hours alarm
byte          sleepAlarmMinutes     = 0;    // Rolling minutes alarm
byte          sleepAlarmHours       = 1;    // Rolling hours alarm

// Manual alarm modes (debugging only)
byte          loggingAlarmMode      = 4;    // Logging alarm mode
byte          sleepAlarmMode        = 4;    // Sleep alarm mode
byte          initialAlarmMode      = 4;    // Initial alarm mode

// ----------------------------------------------------------------------------
// Global variable declarations
// ----------------------------------------------------------------------------
volatile bool alarmFlag           = false;    // Flag for alarm interrupt service routine
volatile bool wdtFlag             = false;    // Flag for WDT interrupt service routine
volatile int  wdtCounter          = 0;        // Counter for WDT interrupts
volatile int  wdtCounterMax       = 0;        // Counter for max WDT interrupts
bool          gnssConfigFlag      = true;     // Flag to indicate whether to configure the u-blox module
bool          rtcSyncFlag         = false;    // Flag to indicate if RTC has been synced with GNSS
bool          firstTimeFlag       = true;     // Flag to indicate if program running for the first time
char          logFileName[30]     = "";       // Log file name
char          debugFileName[20]   = "";       // Debug log file name
char          dateTimeBuffer[25]  = "";       // Buffer to store datetime information
const int     sdWriteSize         = 512;      // Write data to SD in blocks of 512 bytes
const int     fileBufferSize      = 16384;    // Allocate 16 KB RAM for UBX message storage
unsigned int  debugCounter        = 0;        // Counter to track number of recorded debug messages
unsigned int  gnssTimeout         = 5;        // Timeout for GNSS signal acquisition (in minutes)
unsigned int  maxBufferBytes      = 0;        // Maximum file buffer size
unsigned int  reading             = 0;        // Battery voltage analog reading
unsigned int  fixCounter          = 0;
unsigned long previousMillis      = 0;        // millis() timer
unsigned long bytesWritten        = 0;        // Counter for tracking bytes written to microSD
unsigned long syncFailCounter     = 0;        // microSD logfile synchronize failure counter
unsigned long writeFailCounter    = 0;        // microSD logfile write failure counter
unsigned long closeFailCounter    = 0;        // microSD logfile close failure counter
unsigned long logStartTime        = 0;        // Counter to track elapsed logging duration
long          rtcDrift            = 0;        // Counter for RTC drift


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
  //blinkLed(2, 1000);      // Delay to allow user to open Serial Monitor
#endif

  // Configure OLED display
  configureOled();

  printLine();
  DEBUG_PRINT("Cryologger Glacier Velocity Tracker #"); DEBUG_PRINTLN(UNIT);
  printLine();
  printDateTime(); // Print RTC's current date and time
  DEBUG_PRINT("Voltage: "); DEBUG_PRINTLN(readVoltage()); // Print battery voltage

  // Display OLED messages(s)
  displayWelcome();

  // Display logging configuration
  printLoggingSettings();
  displayLoggingMode();

  // Configure devices
  configureWdt();         // Configure and start Watchdog Timer (WDT)
  configureSd();          // Configure microSD
  configureGnss();        // Configure u-blox GNSS receiver
  syncRtc();              // Acquire GNSS fix and sync RTC with GNSS
  createDebugFile();      // Create debug log file
  setInitialAlarm();      // Configure RTC and set initial alarm

  DEBUG_PRINT("Info - Datetime "); printDateTime();
  DEBUG_PRINT("Info - Initial alarm "); printAlarm();

  // Blink LED to indicate completion of setup
  //blinkLed(10, 100);
  displaySetupComplete();
}

// ----------------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------------
void loop()
{
  // Check if alarm flag is set
  if (alarmFlag)
  {
    DEBUG_PRINT("Info - Alarm trigger "); printDateTime();

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
    logGnss();            // Log raw data from u-blox GNSS receiver
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
