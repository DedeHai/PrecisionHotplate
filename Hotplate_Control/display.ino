
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // Set the LCD I2C address and pinmapping (for banggood modules, address is 0x27 or 0x3F)

void initdisplay()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Hotplate Control");
  lcd.setCursor(0, 1);
  DisplayTimestamp = 0; //set next update time (works if called in setup only) this is called within 500ms of bootup.
}


void displayTemperatures() { //display setpoint and measured temperature
  uint8_t padding = 0; //pad blanks in front of the number
  lcd.leftToRight();
  lcd.setCursor(0, 0);
  lcd.print("Setpoint");
  lcd.setCursor(0, 1);
  lcd.print("Sensor");
  if (Setpoint >= 100)
    padding = 2;
  else if (Setpoint >= 10)
  {
    padding = 1;
    //delete the 3rd-digit character:
    lcd.setCursor(9, 0);
    lcd.print(" ");
  }
  lcd.setCursor((11 - padding), 0);
  lcd.print((float)Setpoint, 2);


  if (lowpassvalueLCD >= 100) padding = 2;
  else if (lowpassvalueLCD >= 10)
  { padding = 1;
    //delete the 3rd-digit character:
    lcd.setCursor(9, 1);
    lcd.print(" ");
  }

  lcd.setCursor((11 - padding), 1);
  lcd.print((float)lowpassvalueLCD, 2);
  lcd.setCursor(15, 0);
  lcd.print("C");
  lcd.setCursor(15, 1);
  lcd.print("C");

  DisplayTimestamp = millis();
}

