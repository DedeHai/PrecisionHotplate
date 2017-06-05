#include <Wire.h>
#include <LiquidCrystal_I2C.h> //https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads

//#include <TWILiquidCrystal.h>


#include <PID_v1.h>
#include <PID_AutoTune_v0.h>
#include <avr/pgmspace.h>
#include "NTC_lookup.h"


#define RELAY_PIN 2
#define LED_PIN 13
#define BUTTONGNDPIN 3
#define BUTTONPIN 4
#define BUZZERPIN 5
#define BUZZERGNDPIN 6
#define NTCPRIMARY A0
#define NTCSECONDARY A2
#define NTCGNDPIN 15 //Analog pin A1
#define NTCEXTERNAL A3 //for external NTC 100k sensor
#define POTIPIN A6 //0-5V potentiometer 





//Define Variables we'll be connecting to
double Setpoint, Input, Output;
double Filterconstant = 0.01;
float secondarySensor;
//Specify the links and initial tuning parameters
//double Kp = 150, Ki = 1.3, Kd = 1100 ; //good for reflow
//double Kp = 90, Ki = 0.3, Kd = 200; //fast and very accurate!
double Kp = 80, Ki = 0.4, Kd = 166; 



PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int WindowSize = 800;
unsigned long windowStartTime;


//Autotune stuff::
/*
byte ATuneModeRemember = 2;

double outputStart = 5;
double aTuneStep = 40, aTuneNoise = .2, aTuneStartValue = 80;
unsigned int aTuneLookBack = 20;
boolean tuning = false;
*/

double lowpassvalue = 25; //sensor value
double lowpassvalueLCD = 25; //sensor value to display on LCD (smoother)
double lowpassvalueInput = 20; //lowpass value of ADC read from potentiometer

//PID_ATune aTune(&Input, &Output);

uint8_t reflowing = 0;
uint8_t watchdog = 0; //watchdog is counted up in an interrupt, needs to be kicked once in a while (every 500ms minimum) if not, system reboots
long SerialTimestamp = 0;
long DisplayTimestamp = 0;

void setup()
{


  windowStartTime = millis();
  pinMode(NTCGNDPIN, OUTPUT);
  digitalWrite(NTCGNDPIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(BUTTONPIN, INPUT);
  digitalWrite(BUTTONPIN, HIGH);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(BUZZERGNDPIN, OUTPUT);
  digitalWrite(BUZZERGNDPIN, LOW);
  pinMode(BUTTONGNDPIN, OUTPUT);
  digitalWrite(BUTTONGNDPIN, LOW);

  setupWatchtog();
  


  Serial.begin(115200);
   initdisplay();
  //initialize the setpoint
  Setpoint = 20;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetSampleTime(100);

/*
  if (tuning)
  {
    tuning = false;
    changeAutoTune();
    tuning = true;
  }

*/

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  Serial.println(F("Hotplate Controller"));

}


void loop()
{

  watchdog = 0; //kick the watchdog


  //lowpass filter:
  lowpassvalue = ((double)getTemperature(NTCPRIMARY) *  (double)Filterconstant) + (lowpassvalue  * ((double)1.0 - (double)Filterconstant));
  Input = lowpassvalue;
  lowpassvalueLCD = ((double)lowpassvalue *  (double)0.001) + (lowpassvalueLCD  * ((double)1.0 - (double)0.001)); //double filtered value to display
  
if(abs(Input - Setpoint) < 5 ) Filterconstant = 0.0005;
else Filterconstant = 0.02;


  secondarySensor = getTemperature(NTCSECONDARY);

/*
  if (tuning)
  {
    //Serial.println("autotune");
    byte val = (aTune.Runtime());
    if (val != 0)
    {
      tuning = false;
    }
    if (!tuning)
    { //we're done, set the tuning parameters
      Kp = aTune.GetKp();
      Ki = aTune.GetKi();
      Kd = aTune.GetKd();

      myPID.SetTunings(Kp, Ki, Kd);
      AutoTuneHelper(false);

      Serial.print("kp: "); Serial.print(myPID.GetKp()); Serial.print(" ");
      Serial.print("ki: "); Serial.print(myPID.GetKi()); Serial.print(" ");
      Serial.print("kd: "); Serial.print(myPID.GetKd()); Serial.println();
    }
  }
  else*/
 
 


  /************************************************
   * turn the output pin on/off based on pid output
   ************************************************/
  if (millis() - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
    myPID.Compute();
  }
  if (Output > (millis() - windowStartTime)) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  }
  else {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }

  //safety switch:
  if (Input > 280 || secondarySensor > 280)
  {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    BuzzerAlarm();
    //todo: switch off until below 250°C
  }

  if (reflowing > 0) reflowProfile();
  else
  {
    if (analogRead(POTIPIN) < 1020)
    {
      lowpassvalueInput = ((double)analogRead(POTIPIN) *  (double)0.05) + (lowpassvalueInput  * ((double)1.0 - (double)0.05));
      Setpoint = (float)map(lowpassvalueInput, 0, 1020, 200, 2700)/10; //map input from potentiometer t0 20-270°C
    }
    else
    {
      BuzzerAlarm();
      Setpoint = 20;
    }
  }





  runLowprioritytasks();


}
