/*
  Debug Module

  This module provides debugging utilities for printing system settings,
  logging configurations, and GNSS register values. It also includes timing
  diagnostics to measure function execution times.
*/

// ----------------------------------------------------------------------------
// Print a horizontal separator line (80 dashes).
// ----------------------------------------------------------------------------
void printLine() {
  for (byte i = 0; i < 80; i++) {
    DEBUG_PRINT("-");
  }
  DEBUG_PRINTLN();
}

// ----------------------------------------------------------------------------
// Print tab spacing. Each tab is displayed `_times` times.
// ----------------------------------------------------------------------------
void printTab(byte _times) {
  for (byte i = 0; i < _times; i++) {
    DEBUG_PRINT("\t");
  }
}

// ----------------------------------------------------------------------------
// Print logging settings.
// ----------------------------------------------------------------------------
void printLoggingSettings() {
  printLine();
  DEBUG_PRINTLN("Logging Configuration");
  printLine();

  DEBUG_PRINT("Logging mode: ");
  printTab(2);

  if (operationMode == DAILY) {
    DEBUG_PRINTLN("Daily");
    DEBUG_PRINT("Start: ");
    printTab(3);
    DEBUG_PRINT(alarmStartHour);
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(alarmStartMinute);
    DEBUG_PRINT("Stop: ");
    printTab(3);
    DEBUG_PRINT(alarmStopHour);
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(alarmStopMinute);
  }
  if (operationMode == ROLLING) {
    DEBUG_PRINTLN("Rolling");
    DEBUG_PRINT("Logging duration: ");
    printTab(1);
    DEBUG_PRINT(alarmAwakeHours);
    DEBUG_PRINT(" hours ");
    DEBUG_PRINT(alarmAwakeMinutes);
    DEBUG_PRINTLN(" minutes");
    DEBUG_PRINT("Sleep duration: ");
    printTab(1);
    DEBUG_PRINT(alarmSleepHours);
    DEBUG_PRINT(" hours ");
    DEBUG_PRINT(alarmSleepMinutes);
    DEBUG_PRINTLN(" minutes");
  }
  if (operationMode == CONTINUOUS) {
    DEBUG_PRINTLN("Continuous");
  }

  DEBUG_PRINT("Seasonal mode: ");
  printTab(2);
  DEBUG_PRINTLN(seasonalLoggingMode ? "Enabled"
                                    : "Disabled");
  if (seasonalLoggingMode) {
    DEBUG_PRINT("Start (MM/DD): ");
    printTab(2);
    DEBUG_PRINT(alarmSeasonalStartMonth);
    DEBUG_PRINT("/");
    DEBUG_PRINTLN(alarmSeasonalStartDay);
    DEBUG_PRINT("Stop (MM/DD): ");
    printTab(2);
    DEBUG_PRINT(alarmSeasonalEndMonth);
    DEBUG_PRINT("/");
    DEBUG_PRINTLN(alarmSeasonalEndDay);
  }

  printLine();
}

// ----------------------------------------------------------------------------
// Print GNSS configuration settings.
// Queries and prints various u-blox register values.
// ----------------------------------------------------------------------------
void printGnssSettings() {
  //printLine();
  DEBUG_PRINTLN("u-blox Configuration");
  printLine();

  DEBUG_PRINTLN("Firmware Settings");
  printLine();

  if (gnss.getModuleInfo()) {
    DEBUG_PRINT("FWVER: ");
    DEBUG_PRINT(gnss.getFirmwareVersionHigh());  // Returns uint8_t
    DEBUG_PRINT(".");
    DEBUG_PRINTLN(gnss.getFirmwareVersionLow());  // Returns uint8_t

    DEBUG_PRINT("Firmware: ");
    DEBUG_PRINTLN(gnss.getFirmwareType());  // Returns HPG, SPG etc. as (const char *)

    DEBUG_PRINT("PROTVER: ");
    DEBUG_PRINT(gnss.getProtocolVersionHigh());  // Returns uint8_t
    DEBUG_PRINT(".");
    DEBUG_PRINTLN(gnss.getProtocolVersionLow());  // Returns uint8_t

    DEBUG_PRINT("MOD: ");
    DEBUG_PRINTLN(gnss.getModuleName());  // Returns ZED-F9P, MAX-M10S etc. as (const char *)
  } else
    DEBUG_PRINTLN("[GNSS] Error: Could not read module info!");

  printLine();
  DEBUG_PRINTLN("Receiver Configuration Settings");
  printLine();

  // Create custom packet for retrieving GNSS settings.
  uint8_t customPayload[MAX_PAYLOAD_SIZE];
  ubxPacket customCfg = { 0, 0, 0, 0, 0, customPayload, 0, 0,
                          SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED,
                          SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED };

  gnss.newCfgValget(&customCfg, MAX_PAYLOAD_SIZE, VAL_LAYER_RAM);  // Create a new VALGET construct
  gnss.addCfgValget(&customCfg, UBLOX_CFG_I2C_ENABLED);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_SPI_ENABLED);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART1_ENABLED);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART2_ENABLED);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_USB_ENABLED);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART1_BAUDRATE);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART2_BAUDRATE);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_I2COUTPROT_UBX);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_I2COUTPROT_NMEA);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_I2COUTPROT_RTCM3X);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART2OUTPROT_UBX);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART2OUTPROT_NMEA);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART2OUTPROT_RTCM3X);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART2INPROT_UBX);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART2INPROT_NMEA);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_UART2INPROT_RTCM3X);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_RATE_MEAS);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_RATE_NAV);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_SIGNAL_GPS_ENA);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_SIGNAL_GLO_ENA);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_SIGNAL_GAL_ENA);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_SIGNAL_BDS_ENA);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_SIGNAL_QZSS_ENA);
  gnss.addCfgValget(&customCfg, UBLOX_CFG_SIGNAL_SBAS_ENA);

  if (gnss.sendCfgValget(&customCfg)) {
    // Extract and print GNSS configuration values.
    struct {
      const char *name;
      uint32_t key;
      uint8_t tabCount;
    } gnssSettings[] = {
      { "UBLOX_CFG_I2C_ENABLED", UBLOX_CFG_I2C_ENABLED, 2 },
      { "UBLOX_CFG_SPI_ENABLED", UBLOX_CFG_SPI_ENABLED, 2 },
      { "UBLOX_CFG_UART1_ENABLED", UBLOX_CFG_UART1_ENABLED, 2 },
      { "UBLOX_CFG_UART2_ENABLED", UBLOX_CFG_UART2_ENABLED, 2 },
      { "UBLOX_CFG_USB_ENABLED", UBLOX_CFG_USB_ENABLED, 2 },
      { "UBLOX_CFG_UART1_BAUDRATE", UBLOX_CFG_UART1_BAUDRATE, 1 },
      { "UBLOX_CFG_UART2_BAUDRATE", UBLOX_CFG_UART2_BAUDRATE, 1 },
      { "UBLOX_CFG_I2COUTPROT_UBX", UBLOX_CFG_I2COUTPROT_UBX, 1 },
      { "UBLOX_CFG_I2COUTPROT_NMEA", UBLOX_CFG_I2COUTPROT_NMEA, 1 },
      { "UBLOX_CFG_I2COUTPROT_RTCM3X", UBLOX_CFG_I2COUTPROT_RTCM3X, 1 },
      { "UBLOX_CFG_UART2OUTPROT_UBX", UBLOX_CFG_UART2OUTPROT_UBX, 1 },
      { "UBLOX_CFG_UART2OUTPROT_NMEA", UBLOX_CFG_UART2OUTPROT_NMEA, 1 },
      { "UBLOX_CFG_UART2OUTPROT_RTCM3X", UBLOX_CFG_UART2OUTPROT_RTCM3X, 1 },
      { "UBLOX_CFG_UART2INPROT_UBX", UBLOX_CFG_UART2INPROT_UBX, 1 },
      { "UBLOX_CFG_UART2INPROT_NMEA", UBLOX_CFG_UART2INPROT_NMEA, 1 },
      { "UBLOX_CFG_UART2INPROT_RTCM3X", UBLOX_CFG_UART2INPROT_RTCM3X, 1 },
      { "UBLOX_CFG_RATE_MEAS", UBLOX_CFG_RATE_MEAS, 2 },
      { "UBLOX_CFG_RATE_NAV", UBLOX_CFG_RATE_NAV, 2 },
      { "UBLOX_CFG_SIGNAL_GPS_ENA", UBLOX_CFG_SIGNAL_GPS_ENA, 1 },
      { "UBLOX_CFG_SIGNAL_GLO_ENA", UBLOX_CFG_SIGNAL_GLO_ENA, 1 },
      { "UBLOX_CFG_SIGNAL_GAL_ENA", UBLOX_CFG_SIGNAL_GAL_ENA, 1 },
      { "UBLOX_CFG_SIGNAL_BDS_ENA", UBLOX_CFG_SIGNAL_BDS_ENA, 1 },
      { "UBLOX_CFG_SIGNAL_QZSS_ENA", UBLOX_CFG_SIGNAL_QZSS_ENA, 1 },
      { "UBLOX_CFG_SIGNAL_SBAS_ENA", UBLOX_CFG_SIGNAL_SBAS_ENA, 1 }
    };

    for (const auto &setting : gnssSettings) {
      uint32_t value;
      if (gnss.extractConfigValueByKey(&customCfg, setting.key, &value, sizeof(value))) {
        DEBUG_PRINT(setting.name);
        printTab(setting.tabCount);
        DEBUG_PRINTLN(value);
      } else {
        DEBUG_PRINTLN("extractConfigValueByKey failed!");
      }
    }
  } else {
    DEBUG_PRINTLN("Warning - VALGET failed!");
  }

  printLine();
}

// ----------------------------------------------------------------------------
// Clear all execution timers.
// Resets the timer structure to zero for performance diagnostics.
// ----------------------------------------------------------------------------
void clearTimers() {
  memset(&timer, 0x00, sizeof(timer));
}

// ----------------------------------------------------------------------------
// Print function execution times.
// Displays recorded execution times for various system functions.
// ----------------------------------------------------------------------------
void printTimers() {
  printLine();
  DEBUG_PRINTLN("Function Execution Timers");
  printLine();

  struct {
    const char *name;
    uint32_t value;
    uint8_t tabCount;
  } timers[] = {
    { "readWdt", timer.wdt, 2 },
    { "readRtc", timer.rtc, 2 },
    { "microSD", timer.microSd, 2 },
    { "voltage", timer.voltage, 2 },
    { "gnss", timer.gnss, 2 },
    { "syncRtc", timer.syncRtc, 2 },
    { "logDebug", timer.logDebug, 1 },
    { "logGnss", timer.logGnss, 2 }
  };

  for (const auto &t : timers) {
    DEBUG_PRINT(t.name);
    printTab(t.tabCount);
    DEBUG_PRINTLN(t.value);
  }

  printLine();
}
