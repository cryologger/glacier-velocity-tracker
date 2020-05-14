/*
  Title:    Cryologger - Glacier Velocity Measurement System (Adafruit)
  Date:     May 14, 2020
  Author:   Adam Garbo

  Description:
  - u-blox ZED-F9P raw datalogger
  - Logs UBX-RXM-RAWX and UBX-RXM-SFRBX messages to microSD card

  Components:
  - Adafruit Feather M0 Adalogger
  - SparkFun GPS-RTK2 Board ZED-F9P

  Comments:
  - This code is based on Paul Clark's F9P_RAWX_Logger:
  https://github.com/PaulZC/ZED-F9P_FeatherWing_USB
  - Added functionality includes Watchdog Timer, rolling alarms and
  streamlined code for logging UBX messages.
*/

// Libraries
#include <ArduinoLowPower.h>                // https://github.com/arduino-libraries/ArduinoLowPower
#include <RTCZero.h>                        // https://github.com/arduino-libraries/RTCZero
#include <SdFat.h>                          // https://github.com/greiman/SdFat
#include <SparkFun_Ublox_Arduino_Library.h> // https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
#include <SPI.h>                            // https://www.arduino.cc/en/Reference/SPI
#include <Wire.h>                           // https://www.arduino.cc/en/Reference/Wire

// Debugging definitons
#define DEBUG               true  // Output debug messages to Serial Monitor
#define DEBUG_I2C           false  // Output I2C debug messages to Serial Monitor
#define DEBUG_SERIAL_BUFFER false  // Output a message each time SerialBuffer.available reaches a new maximum
#define DEBUG_UBX           false  // Output UBX debug messages to Serial Monitor

// LEDs
#define LED_ON  true  // Enable LEDs
#define LED_PIN 8     // Indicates that a GNSS fix has been established

// Select alarm
#define ALARM_MIN false // Enable rolling-minutes alarm
#define ALARM_HR  false // Enable rolling-hours alarm
#define ALARM_DAY true  // Enable rolling-day alarm

// Object instantiations
RTCZero       rtc;
SdFat         sd;
SdFile        file;
SFE_UBLOX_GPS gnss; // I2C address: 0x42

// User-declared global variables and constants
const byte    alarmMinutes    = 5;     // Create a new log file every alarmMinutes
const byte    alarmHours      = 4;      // Create a new log file every alarmHours
const int     maxFixCounter   = 10;     // Minimum number of valid GNSS fixes to collect before beginning to log data
const int     dwell           = 1100;   // Delay in milliseconds to record residual UBX data before closing log file (e.g. 1Hz = 1000 ms, so 1100 ms is slightly more than one measurement interval)
const float   lowVoltage      = 3.3;    // Low battery voltage threshold

// Global flag variable delcarations
volatile bool alarmFlag       = false;  // RTC alarm interrupt service routine flag
volatile bool sleepFlag       = false;  // Flag to indicate to Watchdog Timer if in deep sleep mode
volatile byte watchdogCounter = 0;      // Watchdog interrupt service routine counter
bool          ledState        = LOW;    // Flag to toggle LED in blinkLed() function
bool          voltageFlag     = false;  // Flag to indiciate low battery voltage

// Global variable and constant declarations
const byte    chipSelect      = 4;      // SD card chip select
const size_t  sdPacket        = 512;    // SD card write packet size
char          fileName[24]    = {0};    // Log file name. Limited to 8.3 characters. Format: YYYYMMDD/HHMMSS.ubx
char          dirName[9]      = {0};    // Log file directory name. Format: YYYYMMDD
int           numBytes        = 0;      // Total number of bytes written to SD card
int           maxSerialBuffer = 0;      // Maximum size of available SerialBuffer
int           fixCounter      = 0;      // GNSS valid fix counter
long          bytesWritten    = 0;      // SD card write byte counter
float         voltage         = 0.0;    // Battery voltage
uint8_t       serBuffer[sdPacket];      // Buffer for SD card writes
unsigned long previousMillis  = 0;      // Global millis() timer variable
size_t        bufferPointer   = 0;      // Size of serBuffer pointer for SD card writes

// Global variables for UBX parsing
int ubxLength, ubxClass, ubxId, ubxChecksumA, ubxChecksumB, ubxExpectedChecksumA, ubxExpectedChecksumB = 0;

// Enumerated switch statements
enum LoopSwitch {
  INIT,
  START_UBX,
  OPEN_FILE,
  WRITE_FILE,
  NEW_FILE,
  CLOSE_FILE,
  RESTART_FILE,
  SLEEP,
  WAKE
};

enum ParseSwitch {
  PARSE_UBX_SYNC_CHAR_1,
  PARSE_UBX_SYNC_CHAR_2,
  PARSE_UBX_CLASS,
  PARSE_UBX_ID,
  PARSE_UBX_LENGTH_LSB,
  PARSE_UBX_LENGTH_MSB,
  PARSE_UBX_PAYLOAD,
  PARSE_UBX_CHECKSUM_A,
  PARSE_UBX_CHECKSUM_B,
  SYNC_LOST
};

// Default switch cases
LoopSwitch loopStep = INIT;
ParseSwitch parseStep = PARSE_UBX_SYNC_CHAR_1;

// Definitions for u-blox F9P UBX-format (binary) messages

// Satellite systems (GNSS) signal configuration. Enable GPS/GLO and disable GAL/BDS/QZSS.
uint8_t configureGnss() {
  gnss.newCfgValset8(0x1031001f, 0x01, VAL_LAYER_RAM); // CFG-SIGNAL-GPS_ENA
  gnss.addCfgValset8(0x10310001, 0x01);                // CFG-SIGNAL-GPS_L1CA_ENA
  gnss.addCfgValset8(0x10310003, 0x01);                // CFG-SIGNAL-GPS_L2C_ENA
  gnss.addCfgValset8(0x10310021, 0x00);                // CFG-SIGNAL-GAL_ENA
  gnss.addCfgValset8(0x10310007, 0x00);                // CFG-SIGNAL-GAL_E1_ENA
  gnss.addCfgValset8(0x1031000a, 0x00);                // CFG-SIGNAL-GAL_E5B_ENA
  gnss.addCfgValset8(0x10310022, 0x00);                // CFG-SIGNAL-BDS_ENA
  gnss.addCfgValset8(0x1031000d, 0x00);                // CFG-SIGNAL-BDS_B1_ENA
  gnss.addCfgValset8(0x1031000e, 0x00);                // CFG-SIGNAL-BDS_B2_ENA
  gnss.addCfgValset8(0x10310024, 0x00);                // CFG-SIGNAL-QZSS_ENA
  gnss.addCfgValset8(0x10310012, 0x00);                // CFG-SIGNAL-QZSS_L1CA_ENA
  gnss.addCfgValset8(0x10310015, 0x00);                // CFG-SIGNAL-QZSS_L2C_ENA
  gnss.addCfgValset8(0x10310025, 0x01);                // CFG-SIGNAL-GLO_ENA
  gnss.addCfgValset8(0x10310018, 0x01);                // CFG-SIGNAL-GLO_L1_ENA
  return gnss.sendCfgValset8(0x1031001a, 0x01);        // CFG-SIGNAL-GLO_L2_ENA
}

// Set UART1 to 230400 baud
uint8_t setUart1Baud() {
  return gnss.setVal32(0x40520001, 0x00038400, VAL_LAYER_RAM); // CFG-UART1-BAUDRATE - 0x00038400 = 230400 decimal
}

// Enable UBX output protocol on I2C
uint8_t enableI2cUbx() {
  return gnss.setVal8(0x10720001, 0x01, VAL_LAYER_RAM);
}

// Disable NMEA output protocol on I2C
uint8_t disableI2cNmea() {
  return gnss.setVal8(0x10720002, 0x00, VAL_LAYER_RAM);
}

// Enable UBX output protocol on UART1
uint8_t enableUart1Ubx() {
  return gnss.setVal8(0x10740001, 0x01, VAL_LAYER_RAM);
}

// Enable UBX-RXM-RAWX and UBX-RXM-SFRBX messages on UART1
uint8_t enableUbx() {
  gnss.newCfgValset8(0x209102a5, 0x01, VAL_LAYER_RAM); // CFG-MSGOUT-UBX_RXM_RAWX_UART1
  return gnss.sendCfgValset8(0x20910232, 0x01);        // CFG-MSGOUT-UBX_RXM_SFRBX_UART1
}

// Disable UBX-RXM-RAWX and UBX-RXM-SFRBX messages on UART1
uint8_t disableUbx() {
  gnss.newCfgValset8(0x209102a5, 0x00, VAL_LAYER_RAM); // CFG-MSGOUT-UBX_RXM_RAWX_UART1
  return gnss.sendCfgValset8(0x20910232, 0x00);        // CFG-MSGOUT-UBX_RXM_SFRBX_UART1
}

// Set time between GNSS measurements (CFG-RATE-MEAS)
uint8_t setRate5Hz() {
  return gnss.setVal16(0x30210001, 0x00c8, VAL_LAYER_RAM);
}
uint8_t setRate4Hz() {
  return gnss.setVal16(0x30210001, 0x00fa, VAL_LAYER_RAM);
}
uint8_t setRate2Hz() {
  return gnss.setVal16(0x30210001, 0x01f4, VAL_LAYER_RAM);
}
uint8_t setRate1Hz() {
  return gnss.setVal16(0x30210001, 0x03e8, VAL_LAYER_RAM);
}

// Set the dynamic platform model
uint8_t setNavPortable() {
  return gnss.setVal8(0x20110021, 0x00, VAL_LAYER_RAM); // CFG-NAVSPG-DYNMODEL
};
uint8_t setNavStationary() {
  return gnss.setVal8(0x20110021, 0x02, VAL_LAYER_RAM); // CFG-NAVSPG-DYNMODEL
};

// 'Disable' timepulse TP1 by setting LEN_LOCK_TP1 to zero
// (This doesn't actually disable the timepulse, it just sets its length to zero!)
uint8_t disableTp1() {
  return gnss.setVal32(0x40050005, 0x00, VAL_LAYER_RAM); // CFG-TP-LEN_LOCK_TP1
}

// Define SerialBuffer as a large RingBuffer to store Serial1 data using the TC3 timer interrupt
// https://gist.github.com/jdneo/43be30d85080b175cb5aed3500d3f989
// This prevents needing to increase the size of Serial1 receive buffer by editing RingBuffer.h
// Use DEBUG_SERIAL_BUFFER to determine the appropriate size of the buffer.
RingBufferN<16384> SerialBuffer; // Define SerialBuffer as a RingBuffer of size 16k bytes

// TimerCounter3 functions to copy Serial1 receive data into SerialBuffer
// https://gist.github.com/jdneo/43be30d85080b175cb5aed3500d3f989
#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 16

// Set TC3 Interval (sec)
void setTimerInterval(float intervalS) {
  int compareValue = intervalS * CPU_HZ / TIMER_PRESCALER_DIV;
  if (compareValue > 65535) compareValue = 65535;
  TcCount16* TC = (TcCount16*) TC3;
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

// Start TC3 with a specified interval
void startTimerInterval(float intervalS) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // Wait for sync

  TcCount16* TC = (TcCount16*) TC3;

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // Wait for sync

  // Use the 16-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  while (TC->STATUS.bit.SYNCBUSY == 1); // Wait for sync

  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  while (TC->STATUS.bit.SYNCBUSY == 1); // Wait for sync

  // Set prescaler to 16
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV16;
  while (TC->STATUS.bit.SYNCBUSY == 1); // Wait for sync

  setTimerInterval(intervalS);

  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_SetPriority(TC3_IRQn, 3); // Set the TC3 interrupt priority to 3 (lowest)
  NVIC_EnableIRQ(TC3_IRQn);

  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // Wait for sync
}

// TC3 Interrupt Handler
void TC3_Handler() {
  TcCount16* TC = (TcCount16*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  // copy any available Serial1 data into SerialBuffer
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    int available1 = Serial1.available(); // Check if there is any data waiting in the Serial1 RX buffer
    while (available1 > 0) {
      SerialBuffer.store_char(Serial1.read()); // If there is, copy it into our RingBuffer
      available1--;
    }
  }
}

// Setup
void setup() {

  // Pin configuration
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Blink LEDs on reset
  for (byte i = 0; i < 20; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
  }

  // Configure Watchdog Timer
  configureWatchdog();

  // Start Serial at 115200 baud
  Serial.begin(115200);
  //while (!Serial);    // Wait for user to open Serial Monitor
  delay(5000);       // Delay to allow user to open Serial Monitor
 
  Serial.println("-----------------------------");
  Serial.println("u-blox ZED-F9P Raw Datalogger");
  Serial.println("-----------------------------");

  // Initialize I2C
  Wire.begin();
  Wire.setClock(400000); // Increase clock frequency for I2C communications to 400 kHz

  // Configure ADC
  analogReadResolution(12); // Set ADC resolution to 12-bits

  // Initialize the RTC
  rtc.begin();

  // Initialize the SD card
  if (sd.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("SD card initialized.");
  }
  else {
    Serial.println("Warning: Unable to initialize SD card. Halting!");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    while (1); // Halt the program
  }

  // Initialize the u-blox ZED-F9P
  if (gnss.begin() == true) {
    Serial.println("u-blox ZED-F9P initialized.");
#if DEBUG_I2C
    gnss.enableDebugging(); // Enable I2C debugging output to Serial Monitor
#endif
  }
  else {
    Serial.println("Warning: u-blox ZED-F9P not detected at default I2C address. Please check wiring.");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    while (1); // Halt the program
  }

  // u-blox ZED-F9P Configuration
  // Acknowledged:      "Received: CLS:5 ID:1 Payload: 6 8A" (UBX-ACK-ACK (0x05 0x01)
  // Not-Acknowledged:  UBX-ACK-NAK (0x05 0x00)
  // Payload:           UBX-CFG-VALSET (0x06 0x8A)
  boolean setValueSuccess = true;
  setValueSuccess &= enableUart1Ubx();    // Enable UBX output protocol on UART1
  setValueSuccess &= enableI2cUbx();      // Enable UBX output protocol on I2C
  setValueSuccess &= disableI2cNmea();    // Disable NMEA output protocol on I2C leaving it clear for UBX messages
  setValueSuccess &= setUart1Baud();      // Set UART1 baud rate to 230400
  setValueSuccess &= setRate1Hz();        // Set measurement rate to 1Hz
  setValueSuccess &= disableUbx();        // Disable UBX messages
  setValueSuccess &= setNavStationary();  // Set Static Navigation Mode
  setValueSuccess &= configureGnss();     // Configure GNSS constellations
  //setValueSuccess &= setNavPortable();    // Set Portable Navigation Mode

  if (setValueSuccess == true) {
    Serial.println("u-blox ZED-F9P configured.");
  }
  else {
    Serial.println("Warning: Unable to configure u-blox ZED-F9P. Halting!");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    while (1); // Halt the program
  }

#if !LED_ON
  disableTp1(); // Disable the timepulse to prevent the LED from flashing
#endif

  // Start Serial1 at 230400 baud
  Serial1.begin(230400);

#if DEBUG
  // Print RTC's current date and time
  Serial.print("Current RTC date and time: "); printDateTime();

  // Wait for GNSS fix
  Serial.println("Waiting for GNSS fix...");
#endif
}

// Loop
void loop() {

  switch (loopStep) {
    case INIT: {

        // Pet the dog
        resetWatchdog();

#if DEBUG
        char gnssDateTime[24];
        snprintf(gnssDateTime, sizeof(gnssDateTime), "%04u-%02d-%02d %02d:%02d:%02d",
                 gnss.getYear(), gnss.getMonth(), gnss.getDay(),
                 gnss.getHour(), gnss.getMinute(), gnss.getSecond());

        long latitude = gnss.getLatitude();
        long longitude = gnss.getLongitude();
        unsigned int pdop = gnss.getPDOP();
        byte fix = gnss.getFixType();
        byte satellites = gnss.getSIV();

        Serial.print(gnssDateTime);
        Serial.print(" Latitude: "); Serial.print(latitude);
        Serial.print(" Longitude: "); Serial.print(longitude);
        Serial.print(" Satellites: "); Serial.print(satellites);
        Serial.print(" Fix: "); Serial.print(fix);
        Serial.print(" PDOP: "); Serial.println(pdop);
#endif

        // Read battery voltage
        readBattery();

        // Has a GNSS fix been acquired?
        if (gnss.getFixType() > 0) {
#if LED_ON
          digitalWrite(LED_PIN, HIGH); // Turn LED ON to indicate GNSS fix
#endif
          fixCounter++; // Increment counter
        }
        else {
#if LED_ON
          digitalWrite(LED_PIN, LOW); // Turn LED OFF to indicate loss of GNSS fix
#endif
        }

        // Have enough valid GNSS fixes been collected?
        if (fixCounter == maxFixCounter) {
          fixCounter = 0; // Reset counter

          // Set the RTC's date and time
          rtc.setTime(gnss.getHour(), gnss.getMinute(), gnss.getSecond());    // Set the time
          rtc.setDate(gnss.getDay(), gnss.getMonth(), gnss.getYear() - 2000); // Set the date
          Serial.print("RTC set: "); printDateTime();

          // Set the RTC's alarm
          alarmFlag = false; // Clear alarm flag
#if ALARM_MIN
          // Rolling-minutes alarm
          rtc.setAlarmTime(0, (rtc.getMinutes() + alarmMinutes) % 60, 0); // Set alarm time(hour, minute, second)
          rtc.setAlarmDate(rtc.getDay(), rtc.getMonth(), rtc.getYear());  // Set alarm date (day, month, year)
          rtc.enableAlarm(rtc.MATCH_MMSS);                                // Alarm match on minutes and seconds
#endif
#if ALARM_HR
          // Rolling-hour alarm
          rtc.setAlarmTime((rtc.getHours() + alarmHours) % 24, 0, 0);     // Set alarm time (hours, minutes, seconds)
          rtc.setAlarmDate(rtc.getDay(), rtc.getMonth(), rtc.getYear());  // Set alarm date (day, month, year)
          rtc.enableAlarm(rtc.MATCH_HHMMSS);                              // Alarm match on hours, minutes and seconds
#endif
#if ALARM_DAY
          // Rolling-day alarm
          rtc.setAlarmTime(0, 0, 0);                                      // Set alarm time (hours, minutes, seconds)
          rtc.setAlarmDate(rtc.getDay(), rtc.getMonth(), rtc.getYear());  // Set alarm date (day, month, year)
          rtc.enableAlarm(rtc.MATCH_HHMMSS);                              // Alarm match on hours, minutes and seconds
#endif
          rtc.attachInterrupt(alarmMatch);  // Attach alarm interrupt
          Serial.print("Next alarm: "); printAlarm();

          // Check if battery voltage is below lowVoltage threshold
          if (voltage < lowVoltage) {
            Serial.println("Warning: Low battery!");
            loopStep = SLEEP;
            break;
          }

          // Flush RX buffer to clear any old data
          while (Serial1.available()) {
            Serial1.read();
          }

          // Once Serial1 is idle and the buffer is empty, start TC3 interrupts to copy all new data into SerialBuffer
          // Set the timer interval to 10 * 10 / 230400 = 0.000434 secs (10 bytes * 10 bits (1 start, 8 data, 1 stop) at 230400 baud)
          startTimerInterval(0.000434);

          // Start UBX messages
          loopStep = START_UBX;
        }
      }
      break;

    // Start UBX messages
    case START_UBX: {
#if DEBUG
        Serial.println("Case: START_UBX");
#endif
        // Start UBX messages
        enableUbx();

        // Initialize bufferPointer
        bufferPointer = 0;

        // Create a new log file
        loopStep = OPEN_FILE;
      }
      break;

    // Open the log file
    case OPEN_FILE: {
#if DEBUG
        Serial.println("Case: OPEN_FILE");
#endif
        // Pet the dog
        resetWatchdog();

        // Create a new folder
        snprintf(dirName, sizeof(dirName), "%04u%02d%02d", (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay());
        if (sd.mkdir(dirName)) {
          Serial.print("Created folder: ");
          Serial.println(dirName);
        }
        else {
          Serial.println("Warning: Unable to create new folder!");
        }

        // Create log file
        snprintf(fileName, sizeof(fileName), "%04u%02d%02d/%02d%02d%02d.ubx",
                 (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
        if (file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
          Serial.print("Logging to: ");
          Serial.println(fileName);
        }
        else {
          Serial.println("Warning: Unable to open new log file. Halting!");
          while (1); // Halt program
        }

        // Set the log file creation time
        if (!file.timestamp(T_CREATE, rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds())) {
          Serial.println("Warning: Unable to set file create timestamp!");
        }

        bytesWritten = 0;                   // Clear bytesWritten
        ubxLength = 0;                      // Set ubxLength to zero
        parseStep = PARSE_UBX_SYNC_CHAR_1;  // Set parseStep to expect 0xB5

        // Begin logging data
        loopStep = WRITE_FILE;

      } // End case OPEN_FILE
      break;

    // Write bytes to serBuffer and write to file when sdPacket is reached
    case WRITE_FILE: {

        int bufAvail = SerialBuffer.available();
        if (bufAvail > 0) {
#if DEBUG_SERIAL_BUFFER
          if (bufAvail > maxSerialBuffer) {
            maxSerialBuffer = bufAvail;
            Serial.print("Max bufAvail: ");
            Serial.println(maxSerialBuffer);
          }
#endif
          // Read bytes on Serial1
          uint8_t c = SerialBuffer.read_char();

          // Write bytes to serBuffer
          serBuffer[bufferPointer] = c;

#if DEBUG_UBX
          // Echo UBX hex data to Serial Monitor
          Serial.printf("%02X ", c);
#endif
          bufferPointer++;
          if (bufferPointer == sdPacket) {
            bufferPointer = 0;
            numBytes = file.write(&serBuffer, sdPacket); // Write a full packet
            file.sync(); // Sync the file system
            bytesWritten += sdPacket;
            blinkLed(1, 50); // Blink LED to indicate SD write
#if DEBUG
            if (numBytes != sdPacket) {
              Serial.print("Warning: SD write error. Only ");
              Serial.print(sdPacket); Serial.print("/");
              Serial.print(numBytes); Serial.println(" bytes were written.");
            }
#endif
            digitalWrite(LED_BUILTIN, LOW);
          }

          // UBX Frame Structure:
          // Sync Char 1: 1-byte  0xB5
          // Sync Char 2: 1-byte  0x62
          // Class:       1-byte  Group of related messages
          // ID byte:     1-byte  Defines message to follow
          // Length:      2-byte  Payload only length. Little-Endian unsigned 16-bit integer
          // Payload:     Variable number of bytes
          // CK_A:        1-byte  16-bit checksum
          // CK_B:        1-byte
          // Example:     B5 62 02 15 0010 4E621058395C5C40000012000101C6BC 06 00

          // Process data bytes according to parseStep switch statement
          switch (parseStep) {
            case (PARSE_UBX_SYNC_CHAR_1): {
                if (c == 0xB5) {
                  parseStep = PARSE_UBX_SYNC_CHAR_2; // Look for Sync Char 2 (0x62)
                }
                else {
                  Serial.println("Warning: Did not receive Sync Char 1 (0xB5)!");
                  parseStep = SYNC_LOST;
                }
              }
              break;
            case (PARSE_UBX_SYNC_CHAR_2): {
                if (c == 0x62) {
                  ubxExpectedChecksumA = 0; // Reset expected checksum
                  ubxExpectedChecksumB = 0;
                  parseStep = PARSE_UBX_CLASS; // Look for Class
                }
                else {
                  Serial.println("Warning: Did not receive Sync Char 2 (0x62)!");
                  parseStep = SYNC_LOST;
                }
              }
              break;
            // Message      Class   ID
            // RXM_RAWX     0x02    0x15
            // RXM_SFRBF    0x02    0x13
            case (PARSE_UBX_CLASS): {
                ubxClass = c;
                ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
                ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
                parseStep = PARSE_UBX_ID; // Look for ID

                // Class syntax checking
                if (ubxClass != 0x02) {
                  Serial.println("Warning: Did not receive Class 0x02!");
                  parseStep = SYNC_LOST;
                }
              }
              break;
            case (PARSE_UBX_ID): {
                ubxId = c;
                ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
                ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
                parseStep = PARSE_UBX_LENGTH_LSB; // Look for length LSB

                // ID syntax checking
                if ((ubxId != 0x15) && (ubxId != 0x13)) {
                  Serial.println("Warning: Did not receive ID 0x15 or 0x13!");
                  parseStep = SYNC_LOST;
                }
              }
              break;
            case (PARSE_UBX_LENGTH_LSB): {
                ubxLength = c; // Store length LSB
                ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
                ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
                parseStep = PARSE_UBX_LENGTH_MSB; // Look for length MSB
              }
              break;
            case (PARSE_UBX_LENGTH_MSB): {
                ubxLength = ubxLength + (c * 256); // Add length MSB
                ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
                ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
                parseStep = PARSE_UBX_PAYLOAD; // Look for payload bytes (length: ubxLength)
              }
              break;
            case (PARSE_UBX_PAYLOAD): {
                ubxLength = ubxLength - 1; // Decrement length by one
                ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
                ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
                if (ubxLength == 0) {
                  ubxExpectedChecksumA = ubxExpectedChecksumA & 0xff; // Limit checksums to 8-bits
                  ubxExpectedChecksumB = ubxExpectedChecksumB & 0xff;
                  parseStep = PARSE_UBX_CHECKSUM_A; // If we have received length payload bytes, look for checksum bytes
                }
              }
              break;
            case (PARSE_UBX_CHECKSUM_A): {
                ubxChecksumA = c;
                parseStep = PARSE_UBX_CHECKSUM_B;
              }
              break;
            case (PARSE_UBX_CHECKSUM_B): {
                ubxChecksumB = c;
#if DEBUG_UBX
                Serial.println(); // Insert line break between UBX frames in Serial Monitor
#endif
                parseStep = PARSE_UBX_SYNC_CHAR_1; // All bytes received so go back to looking for a new Sync Char 1 unless there is a checksum error
                if ((ubxExpectedChecksumA != ubxChecksumA) or (ubxExpectedChecksumB != ubxChecksumB)) {
                  Serial.println("Warning: UBX checksum error!");
                  parseStep = SYNC_LOST;
                }
              }
              break;
          }
        }
        else {
          readBattery(); // Read battery voltage
        }

        // Check if RTC alarm was triggered
        if ((alarmFlag == true) && (parseStep == PARSE_UBX_SYNC_CHAR_1)) {
          Serial.print("RTC alarm: "); printDateTime();
          loopStep = NEW_FILE; // Close the file and open a new one
          break;
        }
        // Check if sync was lost
        else if (parseStep == SYNC_LOST) {
          Serial.println("Warning: Sync lost! Restarting file...");
          loopStep = RESTART_FILE; // Disable UBX messages, create new log file and reenable UBX messages
        }
        // Check for low battery voltage
        else if (voltage < lowVoltage) {
          voltageFlag = true;
          loopStep = CLOSE_FILE; // Close the file
          break;
        }
      }
      break;

    // Close the current log file and open a new one without stopping UBX messages
    case NEW_FILE: {
#if DEBUG
        Serial.println("Case: NEW_FILE");
#endif
        // Write any remaining data in serBuffer to file
        if (bufferPointer > 0) {
          numBytes = file.write(&serBuffer, bufferPointer); // Write remaining data to file
          file.sync(); // Sync the file system
          bytesWritten += bufferPointer;
          blinkLed(1, 50); // Blink LED to indicate SD write
#if DEBUG
          if (numBytes != sdPacket) {
            Serial.print("Warning: Incomplete SD write. ");
            Serial.print(numBytes); Serial.print("/");
            Serial.print(sdPacket); Serial.println(" bytes were written.");
          }
#endif
          bufferPointer = 0; // Reset bufferPointer
        }

        // Set the log file's last write/modification date and time
        if (!file.timestamp(T_WRITE, rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds())) {
          Serial.println("Warning: Unable to set file write timestamp!");
        }
        // Set log file's last access date and time
        if (!file.timestamp(T_ACCESS, rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds())) {
          Serial.println("Warning: Unable to set file access timestamp!");
        }

        // Close the file
        file.close();

#if DEBUG
        Serial.print("File closed!");
        unsigned long filesize = file.fileSize(); // Get the file size
        Serial.print(" File size: "); Serial.print(filesize);
        Serial.print(". Expected file size: "); Serial.println(bytesWritten);
#endif
        // Synchronize the RTC with GPS time
        if (gnss.getFixType() > 0) {
          // Set the RTC's date and time
          rtc.setTime(gnss.getHour(), gnss.getMinute(), gnss.getSecond());    // Set the time
          rtc.setDate(gnss.getDay(), gnss.getMonth(), gnss.getYear() - 2000); // Set the date
          Serial.print("RTC set: "); printDateTime();
        }

        // An RTC alarm was detected, so set the RTC alarm time to the next alarmInterval and loop back to OPEN_FILE.
        // Set the RTC's alarm
        alarmFlag = false; // Clear the RTC alarm flag
#if ALARM_MIN
        // Rolling-minutes alarm
        rtc.setAlarmTime(0, (rtc.getMinutes() + alarmMinutes) % 60, 0); // Set alarm time(hour, minute, second)
        rtc.setAlarmDate(rtc.getDay(), rtc.getMonth(), rtc.getYear());  // Set alarm date (day, month, year)
        rtc.enableAlarm(rtc.MATCH_MMSS);                                // Alarm match on minutes and seconds
#endif
#if ALARM_HR
        // Rolling-hour alarm
        rtc.setAlarmTime((rtc.getHours() + alarmHours) % 24, 0, 0);     // Set alarm time (hours, minutes, seconds)
        rtc.setAlarmDate(rtc.getDay(), rtc.getMonth(), rtc.getYear());  // Set alarm date (day, month, year)
        rtc.enableAlarm(rtc.MATCH_HHMMSS);                              // Alarm match on hours, minutes and seconds
#endif
#if ALARM_DAY
        // Rolling-day alarm
        rtc.setAlarmTime(0, 0, 0);                                      // Set alarm time (hours, minutes, seconds)
        rtc.setAlarmDate(rtc.getDay(), rtc.getMonth(), rtc.getYear());  // Set alarm date (day, month, year)
        rtc.enableAlarm(rtc.MATCH_HHMMSS);                              // Alarm match on hours, minutes and seconds
#endif
        rtc.attachInterrupt(alarmMatch); // Attach alarm interrupt
#if DEBUG
        Serial.print("Next alarm: "); printAlarm();
#endif

        loopStep = OPEN_FILE; // Loop to open a new file
        bytesWritten = 0;     // Clear bytesWritten
      }
      break;

    // Disable UBX messages, save any residual data and close the file
    case CLOSE_FILE: {
#if DEBUG
        Serial.println("Case: CLOSE_FILE");
#endif
        disableUbx(); // Disable UBX messages
        int waitcount = 0;
        // Wait for residual data
        while (waitcount < dwell) {
          while (SerialBuffer.available()) {
            serBuffer[bufferPointer] = SerialBuffer.read_char(); // Place extra bytes in serBuffer
            bufferPointer++;
            if (bufferPointer == sdPacket) {
              bufferPointer = 0;
              numBytes = file.write(&serBuffer, sdPacket); // Write a full packet
              file.sync(); // Sync the file system
              bytesWritten += sdPacket;
              blinkLed(1, 50); // Blink LED to indicate SD write
#if DEBUG
              if (numBytes != sdPacket) {
                Serial.print("Warning: SD write error. Only ");
                Serial.print(numBytes); Serial.print("/");
                Serial.print(sdPacket); Serial.println(" bytes were written.");
              }
#endif
            }
          }
          waitcount++;
          delay(1);
        }
        // Write any remaining data in serBuffer to file
        if (bufferPointer > 0) {
          numBytes = file.write(&serBuffer, bufferPointer); // Write remaining data
          file.sync(); // Sync the file system
          bytesWritten += bufferPointer;
          blinkLed(1, 50); // Blink LED to indicate SD write

#if DEBUG
          if (numBytes != sdPacket) {
            Serial.print("Warning: Incomplete SD write. ");
            Serial.print(numBytes); Serial.print("/");
            Serial.print(sdPacket); Serial.println(" bytes were written.");
          }
          Serial.print("Final SD write: "); Serial.print(bufferPointer);
          Serial.print(" bytes. Total bytes written: "); Serial.println(bytesWritten);
#endif
          bufferPointer = 0; // Reset bufferPointer
        }

        // Set the log file's last write/modification date and time
        if (!file.timestamp(T_WRITE, rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds())) {
          Serial.println("Warning: Unable to set file write timestamp!");
        }
        // Set log file's last access date and time
        if (!file.timestamp(T_ACCESS, rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds())) {
          Serial.println("Warning: Unable to set file access timestamp!");
        }

        // Close the file
        file.close();

#if DEBUG
        Serial.print("File closed!");
        unsigned long filesize = file.fileSize(); // Get the file size
        Serial.print(" File size: "); Serial.print(filesize);
        Serial.print(". Expected file size: "); Serial.println(bytesWritten);
#endif

        // Loop to restart UBX messages before opening a new file
        loopStep = START_UBX;
      }
      break;

    // If UBX sync is lost, disable UBX messages, save any residual data, close the file, create another and restart UBX messages. Do not update RTC alarm
    case RESTART_FILE: {
#if DEBUG
        Serial.println("RESTART_FILE");
#endif
        resetWatchdog(); // Pet the dog

        disableUbx(); // Disable UBX messages

        int waitcount = 0;
        // Wait for residual data
        while (waitcount < dwell) {
          while (SerialBuffer.available()) {
            serBuffer[bufferPointer] = SerialBuffer.read_char(); // Place extra bytes into serBuffer
            bufferPointer++;
            // Write a full packet
            if (bufferPointer == sdPacket) {
              bufferPointer = 0;
              numBytes = file.write(&serBuffer, sdPacket);
              file.sync(); // Sync the file system
              bytesWritten += sdPacket;
              blinkLed(1, 50); // Blink LED to indicate SD write
#if DEBUG
              if (numBytes != sdPacket) {
                Serial.print("Warning: Incomplete SD write. ");
                Serial.print(numBytes); Serial.print("/");
                Serial.print(sdPacket); Serial.println(" bytes were written.");
              }
#endif
            }
          }
          waitcount++;
          delay(1);
        }
        // Write any remaining data in serBuffer to file
        if (bufferPointer > 0) {
          numBytes = file.write(&serBuffer, bufferPointer);
          file.sync(); // Sync the file system
          bytesWritten += bufferPointer;
          blinkLed(1, 50); // Blink LED to indicate SD card write
#if DEBUG
          if (numBytes != sdPacket) {
            Serial.print("Warning: Incomplete SD write. ");
            Serial.print(numBytes); Serial.print("/");
            Serial.print(sdPacket); Serial.println(" bytes were written.");
          }
          Serial.print("Final SD write: "); Serial.print(bufferPointer);
          Serial.print(" bytes. Total bytes written: "); Serial.println(bytesWritten);
#endif
          // Reset bufferPointer
          bufferPointer = 0;
        }

        // Set the log file's last write/modification date and time
        if (!file.timestamp(T_WRITE, rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds())) {
          Serial.println("Warning: Unable to set file write timestamp!");
        }
        // Set log file's last access date and time
        if (!file.timestamp(T_ACCESS, rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds())) {
          Serial.println("Warning: Unable to set file access timestamp!");
        }

        // Close the file
        file.close();

#if DEBUG
        Serial.print("File closed!");
        unsigned long filesize = file.fileSize(); // Get the file size
        Serial.print(" File size: "); Serial.print(filesize);
        Serial.print(". Expected file size: "); Serial.println(bytesWritten);
#endif
        // Loop to restart UBX messages before opening a new file
        loopStep = START_UBX;
      } // case RESTART_FILE
      break;

    case SLEEP: {
#if DEBUG
        Serial.println("Case: SLEEP");
#endif

        // Pet the dog
        resetWatchdog();

        // Go to deep sleep
        LowPower.deepSleep();

        // Check if RTC alarm was triggered
        if (alarmFlag == true) {
          loopStep = WAKE;
        }
        else {
          loopStep = SLEEP;
        }

        // Blink LED to indicate Watchdog Timer trigger
        blinkLed(1, 10);

      } // case SLEEP
      break;

    case WAKE: {
#if DEBUG
        Serial.println("Case: WAKE");
#endif
        // Check battery voltage
        if (readBattery() > lowVoltage) {
          loopStep = INIT;
        }
        else {
          // Go to deep sleep
          loopStep = SLEEP;
        }
      } // case WAKE
      break;
  }
}

// RTC alarm interrupt
void alarmMatch() {
  alarmFlag = true; // Set alarm flag
}

// Print the RTC's current time and date
void printDateTime() {
  char dateTimeBuffer[25];
  snprintf(dateTimeBuffer, sizeof(dateTimeBuffer), "%04u-%02d-%02d %02d:%02d:%02d",
           (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay(),
           rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  Serial.println(dateTimeBuffer);
}

// Print the RTC's current time and date
void printAlarm() {
  char alarmBuffer[25];
  snprintf(alarmBuffer, sizeof(alarmBuffer), "%04u-%02d-%02d %02d:%02d:%02d",
           (rtc.getAlarmYear() + 2000), rtc.getAlarmMonth(), rtc.getAlarmDay(),
           rtc.getAlarmHours(), rtc.getAlarmMinutes(), rtc.getAlarmSeconds());
  Serial.println(alarmBuffer);
}

// Read the battery voltageu
float readBattery() {
  voltage = analogRead(A7) * (2.0 * 3.3 / 4096.0);
#if DEBUG
  //Serial.print("Voltage: "); Serial.print(voltage, 2); Serial.println("V");
#endif
  return voltage;
}

// Blink LED
void blinkLed(byte flashes, unsigned long interval) {

  // Pet the dog
  resetWatchdog();

  byte i = 0;
  while (i < flashes * 2) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      }
      else {
        ledState = LOW;
      }
#if LED_ON
      digitalWrite(LED_BUILTIN, ledState);
#endif
      i++;
    }
  }
}

// Configure the WDT to perform a system reset if loop() blocks for more than 8-16 seconds
void configureWatchdog() {

  // Set up the generic clock (GCLK2) used to clock the watchdog timer at 1.024kHz
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(4) |          // Divide the 32.768kHz clock source by divisor 32, where 2^(4 + 1): 32.768kHz/32=1.024kHz
                    GCLK_GENDIV_ID(2);            // Select Generic Clock (GCLK) 2
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_DIVSEL |        // Set to divide by 2^(GCLK_GENDIV_DIV(4) + 1)
                     GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK2
                     GCLK_GENCTRL_SRC_OSCULP32K | // Set the clock source to the ultra low power oscillator (OSCULP32K)
                     GCLK_GENCTRL_ID(2);          // Select GCLK2
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Feed GCLK2 to WDT (Watchdog Timer)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK2 to the WDT
                     GCLK_CLKCTRL_GEN_GCLK2 |     // Select GCLK2
                     GCLK_CLKCTRL_ID_WDT;         // Feed the GCLK2 to the WDT
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  WDT->EWCTRL.bit.EWOFFSET = 0xA;                 // Set the Early Warning Interrupt Time Offset to 8 seconds // REG_WDT_EWCTRL = WDT_EWCTRL_EWOFFSET_8K;
  WDT->INTENSET.bit.EW = 1;                       // Enable the Early Warning interrupt                       // REG_WDT_INTENSET = WDT_INTENSET_EW;
  WDT->CONFIG.bit.PER = 0xB;                      // Set the WDT reset timeout to 16 seconds                  // REG_WDT_CONFIG = WDT_CONFIG_PER_16K;
  WDT->CTRL.bit.ENABLE = 1;                       // Enable the WDT in normal mode                            // REG_WDT_CTRL = WDT_CTRL_ENABLE;
  while (WDT->STATUS.bit.SYNCBUSY);               // Await synchronization of registers between clock domains

  // Configure and enable WDT interrupt
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);  // Top priority
  NVIC_EnableIRQ(WDT_IRQn);
}

// Pet the Watchdog Timer
void resetWatchdog() {
  WDT->CLEAR.bit.CLEAR = 0xA5;        // Clear the Watchdog Timer and restart time-out period //REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
  while (WDT->STATUS.bit.SYNCBUSY);   // Await synchronization of registers between clock domains
  watchdogCounter = 0;
}

// Disable the Watchdog Timer
void disableWatchdog() {
  WDT->CTRL.bit.ENABLE = 0;         // Disable Watchdog
  while (WDT->STATUS.bit.SYNCBUSY); // Await synchronization of registers between clock domains
}

// Watchdog Timer interrupt service routine
void WDT_Handler() {
  // Permit a number of Watchdog Timer interrupts before resetting the system
  if (watchdogCounter < 10) {
    WDT->INTFLAG.bit.EW = 1;          // Clear the Early Warning interrupt flag //REG_WDT_INTFLAG = WDT_INTFLAG_EW;
    WDT->CLEAR.bit.CLEAR = 0xA5;      // Clear the Watchdog Timer and restart time-out period //REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
    while (WDT->STATUS.bit.SYNCBUSY); // Await synchronization of registers between clock domains
  }
  else {
#if DEBUG
    WDT->CTRL.bit.ENABLE = 0;         // Disable Watchdog
    digitalWrite(LED_BUILTIN, HIGH);  // Turn on LEDs to indicate Watchdog Timer reset has triggered
    digitalWrite(LED_PIN, HIGH);
#endif
    while (1); // Force Watchdog Timer to reset the system
  }

  // Increment number of Watchdog Timer interrupts
  watchdogCounter++;
#if DEBUG
  Serial.print("watchdogCounter: "); Serial.println(watchdogCounter);
#endif
}
