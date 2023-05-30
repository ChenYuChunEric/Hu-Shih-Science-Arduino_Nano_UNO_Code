/*
   Using Arduino Nano + DHT11
   Detect Temp & Humi
*/

#include "DHT.h"

#define DHTTYPE DHT11      // DHT型號選擇DHT11
#define DHTPIN 9          // DHT11的DATA腳接在D9

DHT dht(DHTPIN, DHTTYPE);  // 初始化DHT11

void setup() {
  Serial.begin(9600);      // 啟動序列埠監控
  dht.begin();             // 啟動DHT
}

void loop() {
  float h = dht.readHumidity();      // 宣告float變數 h 並賦予濕度數值
  float t = dht.readTemperature();   // 選告float變數 t 並賦予溫度數值

  Serial.print(F("Humidity: "));     // 顯示濕度
  Serial.print(h);
  Serial.println(F("%"));

  Serial.print(F("Temperature: "));  // 顯示溫度
  Serial.print(t);
  Serial.println(F("°C"));

  Serial.println();

  delay(5000);
}
