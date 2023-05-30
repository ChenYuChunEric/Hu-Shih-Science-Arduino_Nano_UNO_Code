/*
   Using Arduino Nano + MQ-131
   Detect O3 + Cl2
   Maybe wind and temp will have affect
*/

#include <MQUnifiedsensor.h>

//Definitions
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A1 //Analog input 0 of your arduino
#define type "MQ-131" //MQ131
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ131CleanAir 15 //RS / R0 = 15 ppm

//Declare Sensor
MQUnifiedsensor MQ131(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);


void setup() {
  Serial.begin(9600);

  //Set math model to calculate the PPM concentration and the value of constants
  MQ131.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ131.setA(23.943); MQ131.setB(-1.11); // Configurate the ecuation values to get O3 concentration

  /*
    Exponential regression:
    GAS     | a      | b
    CL2     | 47.209 | -1.186
    O3      | 23.943 | -1.11
  */

  MQ131.init();

  //---------校正---------
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ131.calibrate(RatioMQ131CleanAir);
    Serial.print(".");
  }
  MQ131.setR0(calcR0 / 10);
  Serial.println("  done!.");

  if (isinf(calcR0)) {
    Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply");
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply");
    while (1);
  }
  //---------校正---------
}

void loop() {
  MQ131.setA(23.943); MQ131.setB(-1.11); // Configurate the ecuation values to get O3 concentration
  MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
  float O3ppb = MQ131.readSensorR0Rs(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

  delay(500);

  MQ131.setA(47.209); MQ131.setB(-1.186); // Configurate the ecuation values to get Cl2 concentration
  MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
  float Cl2ppm = MQ131.readSensorR0Rs(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

  delay(500);

  Serial.print(F("O3 = "));
  Serial.print(O3ppb);
  Serial.println(F(" ppb"));

  Serial.print(F("Cl2 = "));
  Serial.print(Cl2ppm);
  Serial.println(F(" ppm"));

}
