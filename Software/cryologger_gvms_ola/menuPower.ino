// Power configuration menu
void menuPower()
{
  while (1)
  {
    Serial.println();
    Serial.println(F("Menu: Configure Power Options"));

#if(HARDWARE_VERSION_MAJOR >= 1) || (HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 6)
    Serial.print(F("1) Turn off Qwiic bus power when sleeping: "));
    if (settings.powerDownQwiicBusBetweenReads)
    {
      Serial.println(F("Yes"));
    }
    else
    {
      Serial.println(F("No"));
    }
#endif

#if(HARDWARE_VERSION_MAJOR >= 1)
    Serial.print(F("2) Power LED During Sleep: "));
    if (settings.enablePwrLedDuringSleep)
    {
      Serial.println(F("Enabled"));
    }
    else
    {
      Serial.println(F("Disabled"));
    }
#endif

    Serial.println(F("x) Exit"));

    byte incoming = getByteChoice(menuTimeout); // Timeout after x seconds

    if (incoming == 'x')
    {
      break;
    }
#if(HARDWARE_VERSION_MAJOR >= 1) || (HARDWARE_VERSION_MAJOR == 0 && HARDWARE_VERSION_MINOR == 6)
    else if (incoming == '1')
    {
      settings.powerDownQwiicBusBetweenReads ^= 1;
    }
#endif
#if(HARDWARE_VERSION_MAJOR >= 1)
    else if (incoming == '2')
    {
      settings.enablePwrLedDuringSleep ^= 1;
    }
#endif
    else if (incoming == STATUS_GETBYTE_TIMEOUT)
    {
      break;
    }
    else
    {
      printUnknown(incoming);
    }
  }
}
