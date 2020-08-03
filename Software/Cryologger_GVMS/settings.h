// Sensor settings

// settings.sensor_uBlox.log     : indicates if the user has enabled uBlox logging. Set by menu 2, sub-menu 1, option 1. Possible redundancy with setings.logData as we only have one sensor.
// settings.enableSd             : defines if the SD card should be enabled by beginSd(). Only set in settings.h. Doesn't appear to be able to be changed by the user? Possibly redundant?
// settings.enableTerminalOutput : indicates if the user has enabled Terminal logging - i.e. should the UBX Class and ID be displayed for every frame. Set by menu 1, option 2.
// setings.logData               : indicates if the user has enabled SD card logging. Set by menu 1, option 1.
// online.microSd                : indicates if the SD card is ready for data logging. Set by beginSd().
// online.dataLogging            : indicates if the SD card log file is open and ready to receive data. Set by beginDataLogging().
// qwiicAvailable.uBlox          : indicates if there is a uBlox module connected. Set by detectQwiicDevices().
// qwiicOnline.uBlox             : indicates if the module has been configured, or needs to be configured. Set true by beginSensors().

// Default u-blox I2C address
#define ADR_UBLOX 0x42

// u-blox settings
struct struct_uBlox {
  bool      log                     = true;
  bool      powerManagement         = true;
  bool      logUBXNAVCLOCK          = false;
  bool      logUBXNAVHPPOSECEF      = false;
  bool      logUBXNAVHPPOSLLH       = false;
  bool      logUBXNAVODO            = false;
  bool      logUBXNAVPOSECEF        = false;
  bool      logUBXNAVPOSLLH         = false;
  bool      logUBXNAVPVT            = false;
  bool      logUBXNAVRELPOSNED      = false;
  bool      logUBXNAVSTATUS         = false;
  bool      logUBXNAVTIMEUTC        = true;
  bool      logUBXNAVVELECEF        = false;
  bool      logUBXNAVVELNED         = false;
  bool      logUBXRXMRAWX           = true;
  bool      logUBXRXMSFRBX          = true;
  bool      logUBXTIMTM2            = false;
  bool      enableUSB               = false;
  bool      enableUART1             = false;
  bool      enableUART2             = false;
  bool      enableSPI               = false;
  uint8_t   ubloxI2Caddress         = ADR_UBLOX;  // Store in case we want to change it at some point with CFG-I2C-ADDRESS (0x20510001)
  uint16_t  minMeasIntervalGps      = 50;         // Minimum measurement interval in ms when tracking GPS only (20Hz for ZED-F9P)
  uint16_t  minMeasIntervalAll      = 125;        // Minimum measurement interval in ms when tracking all constallations (8Hz for ZED-F9P)
  uint16_t  minMeasIntervalRawxAll  = 200;        // Minimum measurement interval in ms when tracking all constallations and logging RAWX
  
  // Note: High-rate RAWX logging is tricky. The ZED-F9P seems happy to log RAWX for all constellations slightly above 5Hz but only USB, UARTs and SPI are disabled.
  // Likely more to do with not overloading the I2C bus, rather than not overloading the module core. RAWX frames can be over 2KB in size.
  // At 5Hz we are getting very close to overloading the I2C bus at 100kHz. TO DO: set this according to module type?
};

// OpenLog Artemis settings recorded to NVM and configuration file
struct struct_settings {
  int       sizeOfSettings                = 0;              // sizeOfSettings *must* be the first entry and must be int
  int       olaIdentifier                 = OLA_IDENTIFIER; // olaIdentifier *must* be the second entry
  int       nextDataLogNumber             = 1;
  uint64_t  usBetweenReadings             = 1000000ULL;     // 1000,000 us = 1000 ms = 1 readings per second
  uint64_t  usLoggingDuration             = 10000000ULL;    // 10,000,000 us = 10 s logging duration
  uint64_t  usSleepDuration               = 0ULL;           // 0 us = do not sleep (continuous logging)
  bool      openNewLogFile                = true;
  bool      enableSd                      = true;
  bool      enableTerminalOutput          = true;
  bool      logData                       = true;
  int       serialTerminalBaudRate        = 115200;
  bool      showHelperText                = true;
  bool      printMajorDebugMessages       = false;
  bool      printMinorDebugMessages       = false;
  bool      powerDownQwiicBusBetweenReads = false;          // 29 chars!
  int       qwiicBusMaxSpeed              = 100000;         // 400 kHz with no pullups causes problems, so default to 100kHz. User can select 400 later if required.
  bool      enablePwrLedDuringSleep       = true;
  bool      useGPIO32ForStopLogging       = false;          // If true, use GPIO as a stop logging button
  struct_uBlox sensor_uBlox;
} settings;

// Devices onboard the OpenLog Artemis that may be online or offline
struct struct_online
{
  bool microSd = false;
  bool dataLogging = false;
} online;

// Structures to define supported sensors and if they are available and online (started)
struct struct_QwiicSensors
{
  bool uBlox;
};

struct_QwiicSensors qwiicAvailable =
{
  .uBlox = false,
};

struct_QwiicSensors qwiicOnline =
{
  .uBlox = false,
};
