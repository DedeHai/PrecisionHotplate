
//this function is called at the end of every main loop.
//don't make this too long lasting (i.e. not longer than 10ms on average)

void  runLowprioritytasks(void)
{

  BuzzerHandler();

  if (digitalRead(BUTTONPIN) == 0)
  {
    startReflow();
  }

  if ((millis() - SerialTimestamp) > 500)
  {
    SerialTimestamp = millis();
    Serial.print(F("Temperature: "));
    Serial.print(Input, DEC);
    Serial.print(F("\t Setpoint = "));
    Serial.print(Setpoint, DEC);
    Serial.print(F("\t Secondary = "));
    Serial.print(secondarySensor, DEC);
    Serial.print(F("\t Output = "));
    Serial.println(Output, DEC);
  }  
  
   if ((millis() - DisplayTimestamp) > 200)
   {
     displayTemperatures();
   }

}
