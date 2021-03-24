#if DEBUG_OLED
void configureOled()
{
  if (!oled.begin())
  {
    Serial.println("Warning: OLED failed to initialize!");
  }

  oled.setFont(u8x8_font_amstrad_cpc_extended_f);
  oled.drawString(0, 0, "Init. OLED."); // Write something to the internal memory
  delay(500);
}


void printBuffer()
{
  oled.setCursor(0, 0);
  oled.print(bytesWritten);
  oled.setCursor(0, 1);
  oled.print(maxBufferBytes);
}
#endif
