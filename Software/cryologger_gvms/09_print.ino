void printLine()
{
  for (byte i = 0; i < 80; i++)
  {
    DEBUG_PRINT("-");
  }
  DEBUG_PRINTLN();
}

void printTab(byte _times)
{
  for (byte i = 0; i < _times; i++)
  {
    DEBUG_PRINT("\t");
  }
}

// Print logging settings
void printLoggingSettings()
{
  printLine();
  DEBUG_PRINTLN("Logging Configuration");
  printLine();
  
  DEBUG_PRINT("Logging mode: ");          printTab(3);  DEBUG_PRINTLN(loggingMode);
  DEBUG_PRINT("Logging start: ");         printTab(3);  DEBUG_PRINTLN(loggingStartTime);
  DEBUG_PRINT("Logging stop: ");          printTab(3);  DEBUG_PRINTLN(loggingStopTime);
  DEBUG_PRINT("Rolling logging alarm: "); printTab(2);  DEBUG_PRINT(loggingAlarmHours); DEBUG_PRINT(" hours "); DEBUG_PRINT(loggingAlarmMinutes); DEBUG_PRINTLN(" minutes ");
  DEBUG_PRINT("Rolling sleep alarm: ");   printTab(2);  DEBUG_PRINT(sleepAlarmHours); DEBUG_PRINT(" hours "); DEBUG_PRINT(sleepAlarmMinutes); DEBUG_PRINTLN(" minutes ");
  DEBUG_PRINT("Logging alarm mode: ");    printTab(2);  DEBUG_PRINTLN(loggingAlarmMode);
  DEBUG_PRINT("Sleep alarm mode: ");      printTab(2);  DEBUG_PRINTLN(sleepAlarmMode);
  DEBUG_PRINT("Initial alarm mode: ");    printTab(2);  DEBUG_PRINTLN(initialAlarmMode);
}

// Print values of u-blox registers
void printGnssSettings()
{
  printLine();
  DEBUG_PRINTLN("u-blox Configuration");
  printLine();
  DEBUG_PRINT("UBLOX_CFG_RATE_MEAS: ");            printTab(2);  DEBUG_PRINTLN(gnss.getVal16(UBLOX_CFG_RATE_MEAS));
  DEBUG_PRINT("UBLOX_CFG_RATE_NAV: ");             printTab(2);  DEBUG_PRINTLN(gnss.getVal16(UBLOX_CFG_RATE_NAV));
  DEBUG_PRINT("UBLOX_CFG_UART1_BAUDRATE: ");       printTab(1);  DEBUG_PRINTLN(gnss.getVal32(UBLOX_CFG_UART1_BAUDRATE));
  //DEBUG_PRINT("UBLOX_CFG_UART2_BAUDRATE: ");       printTab(1);  DEBUG_PRINTLN(gnss.getVal32(UBLOX_CFG_UART2_BAUDRATE));
  DEBUG_PRINT("UBLOX_CFG_I2COUTPROT_UBX: ");       printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_I2COUTPROT_UBX));
  DEBUG_PRINT("UBLOX_CFG_I2COUTPROT_NMEA: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_I2COUTPROT_NMEA));
  DEBUG_PRINT("UBLOX_CFG_I2COUTPROT_RTCM3X: ");    printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_I2COUTPROT_RTCM3X));
  DEBUG_PRINT("UBLOX_CFG_UART1OUTPROT_UBX: ");     printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_UBX));
  DEBUG_PRINT("UBLOX_CFG_UART1OUTPROT_NMEA: ");    printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_NMEA));
  DEBUG_PRINT("UBLOX_CFG_UART1OUTPROT_RTCM3X: ");  printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_RTCM3X));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_GPS_ENA: ");       printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GPS_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_GLO_ENA: ");       printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GLO_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_GAL_ENA: ");       printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GAL_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_BDS_ENA: ");       printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_BDS_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_QZSS_ENA: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_QZSS_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_SBAS_ENA: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_SBAS_ENA));
  DEBUG_PRINT("UBLOX_CFG_I2C_ENABLED: ");          printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_I2C_ENABLED));
  //DEBUG_PRINT("UBLOX_CFG_SPI_ENABLED: ");          printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SPI_ENABLED));
  DEBUG_PRINT("UBLOX_CFG_UART1_ENABLED: ");        printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1_ENABLED));
  //DEBUG_PRINT("UBLOX_CFG_UART2_ENABLED: ");        printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART2_ENABLED));
  DEBUG_PRINT("UBLOX_CFG_USB_ENABLED: ");          printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_USB_ENABLED));

  printLine();
}
