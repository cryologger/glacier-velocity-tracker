// Configure SSD1306 OLED
void configureOled()
{
#if DEBUG_OLED
  if (!u8g2.begin())
  {
    Serial.println("Warning: OLED failed to initialize!");
  }
  else
  {


    if (firstTimeFlag)
    {
      u8g2.clearBuffer();                                     // Clear the internal memory
      u8g2.setFont(u8g2_font_helvR08_tf);                     // Choose font
      u8g2.drawStr(0, 10, "-------------------------------"); // Write to internal memory
      u8g2.drawStr(0, 20, "| Cryologger GVMS v2.0 |");        // Write to internal memory
      u8g2.drawStr(0, 30, "-------------------------------"); // Write to internal memory
      u8g2.sendBuffer();                                      // Transfer internal memory to the display
      delay(1000);
    }
    else
    {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "OLED initialized");
      u8g2.sendBuffer();
      delay(500);
    }
  }
#endif
}
