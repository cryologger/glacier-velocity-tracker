void printLine()
{
  for (byte i = 0; i < 80; i++)
  {
    Serial.print("-");
  }
  Serial.println();
}

void printTab(byte _times)
{
  for (byte i = 0; i < _times; i++)
  {
    Serial.print("\t");
  }
}
void printGnssSettings()
{
  printLine();
  Serial.println("u-blox Configuration");
  printLine();
  Serial.print("UBLOX_CFG_RATE_MEAS: ");            printTab(2);  Serial.println(gnss.getVal16(UBLOX_CFG_RATE_MEAS));
  Serial.print("UBLOX_CFG_RATE_NAV: ");             printTab(2);  Serial.println(gnss.getVal16(UBLOX_CFG_RATE_NAV));
  Serial.print("UBLOX_CFG_UART1_BAUDRATE: ");       printTab(1);  Serial.println(gnss.getVal32(UBLOX_CFG_UART1_BAUDRATE));
  Serial.print("UBLOX_CFG_UART2_BAUDRATE: ");       printTab(1);  Serial.println(gnss.getVal32(UBLOX_CFG_UART2_BAUDRATE));
  Serial.print("UBLOX_CFG_I2COUTPROT_UBX: ");       printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_I2COUTPROT_UBX));
  Serial.print("UBLOX_CFG_I2COUTPROT_NMEA: ");      printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_I2COUTPROT_NMEA));
  Serial.print("UBLOX_CFG_I2COUTPROT_RTCM3X: ");    printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_I2COUTPROT_RTCM3X));
  Serial.print("UBLOX_CFG_UART1OUTPROT_UBX: ");     printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_UBX));
  Serial.print("UBLOX_CFG_UART1OUTPROT_NMEA: ");    printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_NMEA));
  Serial.print("UBLOX_CFG_UART1OUTPROT_RTCM3X: ");  printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_UART1OUTPROT_RTCM3X));
  Serial.print("UBLOX_CFG_SIGNAL_GPS_ENA: ");       printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_SIGNAL_GPS_ENA));
  Serial.print("UBLOX_CFG_SIGNAL_GLO_ENA: ");       printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_SIGNAL_GLO_ENA));
  Serial.print("UBLOX_CFG_SIGNAL_GAL_ENA: ");       printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_SIGNAL_GAL_ENA));
  Serial.print("UBLOX_CFG_SIGNAL_BDS_ENA: ");       printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_SIGNAL_BDS_ENA));
  Serial.print("UBLOX_CFG_SIGNAL_QZSS_ENA: ");      printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_SIGNAL_QZSS_ENA));
  Serial.print("UBLOX_CFG_SIGNAL_SBAS_ENA: ");      printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_SIGNAL_SBAS_ENA));
  Serial.print("UBLOX_CFG_I2C_ENABLED: ");          printTab(2);  Serial.println(gnss.getVal8(UBLOX_CFG_I2C_ENABLED));
  Serial.print("UBLOX_CFG_SPI_ENABLED: ");          printTab(2);  Serial.println(gnss.getVal8(UBLOX_CFG_SPI_ENABLED));
  Serial.print("UBLOX_CFG_UART1_ENABLED: ");        printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_UART1_ENABLED));
  Serial.print("UBLOX_CFG_UART2_ENABLED: ");        printTab(1);  Serial.println(gnss.getVal8(UBLOX_CFG_UART2_ENABLED));
  Serial.print("UBLOX_CFG_USB_ENABLED: ");          printTab(2);  Serial.println(gnss.getVal8(UBLOX_CFG_USB_ENABLED));

  printLine();
}
