/*
 * Using Arduino Nano + SenseAir S8
 * Detect CO2
 * Maybe wind and temp will have affect
 */

#include <Arduino.h>
#include "s8_uart.h"

#define S8_RX_PIN 2         // Rx pin which the S8 Tx pin is attached to (change if it is needed)
#define S8_TX_PIN 3         // Tx pin which the S8 Rx pin is attached to (change if it is needed)

SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);
S8_UART *sensor_S8;
S8_sensor sensor;

void setup() {

  // Configure serial port, we need it for debug
  Serial.begin(9600);

  // Wait port is open or timeout
  int i = 0;
  while (!Serial && i < 50) {
    delay(10);
    i++;
  }
  
  // First message, we are alive
  Serial.println("Init");

  // Initialize S8 sensor
  S8_serial.begin(S8_BAUDRATE);
  sensor_S8 = new S8_UART(S8_serial);

  // Check if S8 is available
  sensor_S8->get_firmware_version(sensor.firm_version);
  int len = strlen(sensor.firm_version);
  if (len == 0) {
      Serial.println("SenseAir S8 CO2 sensor not found!");
      while (1) { delay(1); };
  }
  Serial.println("Setup done!");
  Serial.flush();
}

void loop() {
  // Get CO2 measure
  sensor.co2 = sensor_S8->get_co2();
  Serial.println(sensor.co2);
  delay(3000);
}
