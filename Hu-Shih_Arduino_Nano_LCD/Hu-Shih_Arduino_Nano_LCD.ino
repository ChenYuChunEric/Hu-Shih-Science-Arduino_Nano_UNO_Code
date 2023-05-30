/*
 * Using Arduino Nano + LCD
 * Display Data
 * A4→SDA  A5→SCL
 * lcd(0x3f) or lcd(0x27)
 */

#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x3f); // set the LCD address to 0x27/0x3f for a 16 chars and 2 line display

int a=0;

void setup(){
  Serial.begin(9600);
  Serial.println("LCD...");
  lcd.begin(16,2);
  lcd.setBacklight(255);
}


void loop(){
  lcd.setCursor(0,0);
  lcd.print("The First Line");

  lcd.setCursor(0,1);
  lcd.print(a);
  a+=1;
  delay(1000);
}
