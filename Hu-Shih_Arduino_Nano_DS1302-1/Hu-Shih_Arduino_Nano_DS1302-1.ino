/*
   Using Arduino Nano + DS1302
   Detect RTC Time
   RST 8
   CLK 6
   DAT 7
*/

#include <RTClib.h>

#define RST 8
#define CLK 6
#define DAT 7
DS1302 rtc(RST, CLK, DAT);

// buffer for DateTime.tostr
char buf[20];

void setup() {
  Serial.begin(9600);
  rtc.begin();
  rtc.adjust(DateTime(__DATE__, __TIME__));
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop() {
  DateTime now = rtc.now();
  Serial.println(now.tostr(buf));
  delay(1000);
}
