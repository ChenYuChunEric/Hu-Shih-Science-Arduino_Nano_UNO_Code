/*
   Using Arduino Nano + MQ-135
   Detect NH4
   Maybe wind and temp will have affect
*/

#include <MQUnifiedsensor.h>

//Definitions
#define placa "Arduino NANO"
#define Voltage_Resolution 5
#define pin A2 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm

//Declare Sensor
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup() {
  Serial.begin(9600);

  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setA(102.2); MQ135.setB(-2.473); // Configurate the ecuation values to get NH4 concentration

  MQ135.init();

  //---------校正---------
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
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
  MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
  float NH4ppm = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  Serial.print(F("NH4= "));
  Serial.print(NH4ppm);
  Serial.println(F("ppm"));
  delay(1000); //Sampling frequency

}
