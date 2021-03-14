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

// Print user-defined beacon settings
void printSettings()
{
  printLine();
  DEBUG_PRINTLN("Settings");
  printLine();
  //DEBUG_PRINT("alarmInterval: ");     printTab(2);  DEBUG_PRINTLN(alarmInterval);
  //DEBUG_PRINT("loggingInterval: ");   printTab(1);  DEBUG_PRINTLN(loggingInterval);
  printLine();
}

void printTimers()
{
  //printLine();
  DEBUG_PRINTLN("Function Execution Timers (ms)");
  printLine();
  DEBUG_PRINT("voltage: "); printTab(1);  DEBUG_PRINTLN(timer.voltage);
  DEBUG_PRINT("rtc: ");     printTab(2);  DEBUG_PRINTLN(timer.rtc);
  DEBUG_PRINT("microsd: "); printTab(1);  DEBUG_PRINTLN(timer.microSd);
  DEBUG_PRINT("sensors: "); printTab(1);  DEBUG_PRINTLN(timer.sensors);
  DEBUG_PRINT("gnss: ");    printTab(2);  DEBUG_PRINTLN(timer.gnss);
  DEBUG_PRINT("logGnss: "); printTab(1);  DEBUG_PRINTLN(timer.logGnss);
  printLine();
}


void printGnssSettings()
{
  printLine();
  DEBUG_PRINTLN("Settings");
  printLine();
  DEBUG_PRINT("UART1 BAUD: ");    printTab(1);  DEBUG_PRINTLN(gnss.getVal32(UBLOX_CFG_UART1_BAUDRATE));
  DEBUG_PRINT("UART2 BAUD: ");    printTab(1);  DEBUG_PRINTLN(gnss.getVal32(UBLOX_CFG_UART2_BAUDRATE));
  DEBUG_PRINT("UART1 UBX: ");     printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1INPROT_UBX));
  DEBUG_PRINT("UART1 NMEA: ");    printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1INPROT_NMEA));
  DEBUG_PRINT("UART1 RTCM3X: ");  printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1INPROT_RTCM3X));
  DEBUG_PRINT("GPS: ");           printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GPS_ENA));
  DEBUG_PRINT("GLO: ");           printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GLO_ENA));
  DEBUG_PRINT("GAL: ");           printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GAL_ENA));
  DEBUG_PRINT("BDS: ");           printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_BDS_ENA));
  DEBUG_PRINT("QZSS: ");          printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_QZSS_ENA));

  printLine();
}
