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
  printLine();
  DEBUG_PRINTLN("Function Execution Timers (ms)");
  printLine();
  DEBUG_PRINT("voltage: "); printTab(1);  DEBUG_PRINTLN(timer.voltage);
  DEBUG_PRINT("wdt: ");     printTab(2);  DEBUG_PRINTLN(timer.wdt);
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
  DEBUG_PRINTLN("u-blox Configuration");
  printLine();
  DEBUG_PRINT("UBLOX_CFG_RATE_MEAS: ");           printTab(2);  DEBUG_PRINTLN(gnss.getVal16(UBLOX_CFG_RATE_MEAS));
  DEBUG_PRINT("UBLOX_CFG_RATE_NAV: ");            printTab(2);  DEBUG_PRINTLN(gnss.getVal16(UBLOX_CFG_RATE_NAV));
  DEBUG_PRINT("UBLOX_CFG_UART1_BAUDRATE: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal32(UBLOX_CFG_UART1_BAUDRATE));
  DEBUG_PRINT("UBLOX_CFG_UART2_BAUDRATE: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal32(UBLOX_CFG_UART2_BAUDRATE));
  DEBUG_PRINT("UBLOX_CFG_UART1OUTPROT_UBX: ");    printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_UBX));
  DEBUG_PRINT("UBLOX_CFG_UART1OUTPROT_NMEA: ");   printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_NMEA));
  DEBUG_PRINT("UBLOX_CFG_UART1OUTPROT_RTCM3X: "); printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_RTCM3X));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_GPS_ENA: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GPS_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_GLO_ENA: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GLO_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_GAL_ENA: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_GAL_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_BDS_ENA: ");      printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_BDS_ENA));
  DEBUG_PRINT("UBLOX_CFG_SIGNAL_QZSS_ENA: ");     printTab(1);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_SIGNAL_QZSS_ENA));
  DEBUG_PRINT("UBLOX_CFG_USB_ENABLED: ");         printTab(2);  DEBUG_PRINTLN(gnss.getVal8(UBLOX_CFG_USB_ENABLED));
  
  printLine();
}

void printDebug()
{
  printLine();
  DEBUG_PRINTLN("Debugging");
  printLine();
  DEBUG_PRINT("unixtime");            printTab(2);  DEBUG_PRINTLN(unixtime);
  DEBUG_PRINT("timer.voltage");       printTab(2);  DEBUG_PRINTLN(timer.voltage);
  DEBUG_PRINT("timer.rtc");           printTab(2);  DEBUG_PRINTLN(timer.rtc);
  DEBUG_PRINT("timer.microSd");       printTab(2);  DEBUG_PRINTLN(timer.microSd);
  DEBUG_PRINT("timer.sensors");       printTab(2);  DEBUG_PRINTLN(timer.sensors);
  DEBUG_PRINT("timer.logGnss");       printTab(2);  DEBUG_PRINTLN(timer.logGnss);
  DEBUG_PRINT("bytesWritten");        printTab(2);  DEBUG_PRINTLN(bytesWritten);
  DEBUG_PRINT("maxBufferBytes");      printTab(2);  DEBUG_PRINTLN(maxBufferBytes);
  DEBUG_PRINT("online.microSd");      printTab(2);  DEBUG_PRINTLN(online.microSd);
  DEBUG_PRINT("online.gnss");         printTab(2);  DEBUG_PRINTLN(online.gnss);
  DEBUG_PRINT("online.dataLogging");  printTab(1);  DEBUG_PRINTLN(online.dataLogging);
  DEBUG_PRINT("online.debugLogging"); printTab(1);  DEBUG_PRINTLN(online.debugLogging);
  DEBUG_PRINT("rtcDrift");            printTab(2);  DEBUG_PRINTLN(rtcDrift);
  DEBUG_PRINT("watchdogCounterMax");  printTab(1);  DEBUG_PRINTLN(watchdogCounterMax);
  DEBUG_PRINT("debugCounter");        printTab(2);  DEBUG_PRINTLN(debugCounter);
  
  printLine();
}
  
