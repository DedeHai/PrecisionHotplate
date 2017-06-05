
float getTemperature(uint8_t pin)
{
  int32_t NTCtemperature = analogRead(pin);

  //Serial.print("ADC value: ");
  //Serial.print(NTCtemperature, DEC);
  uint8_t i = 1;

  //use the lookup table with linear interpolation:
  for (i = 1; i < 72; i++) //search through the lookuptable
  {
    //  Serial.print("\t NTCtable= ");
    //Serial.print(NTCtable[i][1], DEC);
    if (NTCtable[i][1] > NTCtemperature) {
      NTCtemperature = (int32_t) NTCtable[i - 1][0] * 1000
                       + (NTCtemperature - NTCtable[i - 1][1])
                       * (((int32_t) NTCtable[i][0]
                           - (int32_t) NTCtable[i - 1][0])
                          * 1000)
                       / ((int32_t) NTCtable[i][1]
                          - (int32_t) NTCtable[i - 1][1]);
      break;
    }
  }

  //Serial.print("\t Temp: ");
  //Serial.println(NTCtemperature, DEC);
  return  ((float)NTCtemperature)/1000;
}
