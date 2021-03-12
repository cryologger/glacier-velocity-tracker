void configureSensors() {}

void readSensors()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Stop the loop timer
  timer.sensors = millis() - loopStartTime;
}
