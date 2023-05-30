/*
   Using Arduino Nano + PMS5003T
   Detect PM1.0 PM2.5 PM10 Temp Humi
   Pin4 RX  Pin5 TX
   Maybe wind and temp will have affect
*/

#include <PMS.h>
#include <SoftwareSerial.h>

long pmat10 = 0;
int pmat25 = 0;
long pmat100 = 0;
unsigned int temperature = 0;
unsigned int humandity = 0;
int pmat25_value = 0;

SoftwareSerial pmsSerial(4, 5); // 將Arduino Pin4設定為RX, Pin5設定為TX

void pms5003t_spec() {
  int count = 0;
  unsigned char c;
  unsigned char high;
  while (pmsSerial.available()) {
    c = pmsSerial.read();
    if ((count == 0 && c != 0x42) || (count == 1 && c != 0x4d)) {
      Serial.println("check failed");
      break;
    }
    if (count > 27) {
      Serial.println("Done!!");
      break;
    }
    else if (count == 10 || count == 12 || count == 14 || count == 24 || count == 26) {
      high = c;
    }
    else if (count == 11) {
      pmat10 = 256 * high + c;
      Serial.print("PM1.0=");
      Serial.print(pmat10);
      Serial.println(" ug/m3");
    }
    else if (count == 13) {
      pmat25 = 256 * high + c;
      Serial.print("PM2.5=");
      Serial.print(pmat25);
      Serial.println(" ug/m3");
    }
    else if (count == 15) {
      pmat100 = 256 * high + c;
      Serial.print("PM10=");
      Serial.print(pmat100);
      Serial.println(" ug/m3");
    }
    else if (count == 25) {
      temperature = (256 * high + c) / 10;
      Serial.print("Temp=");
      Serial.print(temperature);
      Serial.println(" (C)");
    }
    else if (count == 27) {
      humandity = (256 * high + c) / 10;
      Serial.print("Humidity=");
      Serial.print(humandity);
      Serial.println(" (%)");
    }
    count++;
  }
  while (pmsSerial.available())
    pmsSerial.read();
  Serial.println();
}

void setup()
{
  pmsSerial.begin(9600);
  Serial.begin(9600);
}

void loop()
{
  pms5003t_spec();
  delay(1000);
}
