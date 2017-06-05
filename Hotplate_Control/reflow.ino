/*
Reflow profile:
http://www.dfrsolutions.com/wp-content/uploads/2012/11/Typical-JEDEC-Reflow-Profile.jpg

I choose:

preheat to 190°C: set to 160°C but when 150°C is reached, start a ramp of 60 seconds to 200°C
After time has expired, set to 260°C. We want to reach 235, the overshoot at current settings is 6-8°C when in full speed ramp.
So when reaching 230°C set to 10°C.
The rampdown time should be no faster than 6°C/s but the plate is much slower than that. need to find a good method for cooldown, like adding a PTFE spacer under the PCB for cooldown.

*/


#define PREHEATTHRESHOLD 150
#define DWELLTIME 70 //ramp time from 150 to 200 (200°C)
#define DWELLTEMPERATURE 210 //ramp up to this temperature during dwell time
#define PEAKTEMPERATURE 265 //desired reflow peak temperature
#define OVERSHOOT 6 //measured overshoot in reflow ramp (needed to switch heater off before reaching PEAKTEMPERATURE)


long reflowtime;


void startReflow(void)
{
  reflowing = 1;
  reflowtime = 0;
  BuzzerShortbeep();
  Serial.println(F("Reflow started, preheating..."));
  delay(20);
}

void reflowProfile(void)
{

  if (reflowing == 1) //preheat period (ramp to PREHEATTHRESHOLD)
  {
    Setpoint = 160;
    if (Input >= PREHEATTHRESHOLD)
    {
      reflowing = 2;
      reflowtime = millis(); //set timestamp
      Serial.println(F("preheat temperature reached. dwelling..."));
      BuzzerShortbeep();
    }
  }
  else if (reflowing == 2) //dwell period (ramp slowly to 200°C)
  {
    float dwelltime = (float)(millis() - reflowtime) / 1000;
    Setpoint = ((float)dwelltime / (float)DWELLTIME) * (DWELLTEMPERATURE - PREHEATTHRESHOLD) + PREHEATTHRESHOLD;

    if (dwelltime > DWELLTIME) {

      reflowing = 3; //go to full speed ramp
      Serial.println(F("ramping to peak temperature now"));
      BuzzerShortbeep();
    }

  }
  else if (reflowing == 3) //rampup to peak temperature
  {
    //set the temperature high, to make a fast ramp:

    Setpoint = 300;
    if (Input >= (PEAKTEMPERATURE - OVERSHOOT))
    {
      Setpoint = 10;
      reflowing = 4;
      reflowtime = millis(); //set timestamp
      Serial.println(F("reaching peak reflow shortly"));
      BuzzerShortbeep();
    }
  }
  else if (reflowing == 4) //wait until peak is reached, notify
  {
      if(millis() - reflowtime > 30000)
      {
          BuzzerLongbeep();    
          reflowing = 0;
      }           
  }



}
