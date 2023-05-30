/*
   Using Arduino Nano + MQ-7
   Detect CO
   Maybe wind and temp will have affect
*/

#include <MQUnifiedsensor.h>

//Definitions
#define placa "Arduino NANO"
#define Voltage_Resolution 5
#define pin A0 //Analog input 0 of your arduino
#define type "MQ-7" //MQ7
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ7CleanAir 27.5 //RS / R0 = 27.5 ppm 

//Declare Sensor
MQUnifiedsensor MQ7(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup() {
  Serial.begin(9600);
  MQ7.setRegressionMethod(1);
  MQ7.setA(99.042); MQ7.setB(-1.518);
  MQ7.init();

  //-------校正--------
  Serial.print(F("Calibrating please wait."));
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
    Serial.print(F("."));
  }
  MQ7.setR0(calcR0 / 10);
  Serial.println();
  Serial.println(F("MQ-7  done!."));

  if (isinf(calcR0)) {
    Serial.println(F("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"));
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println(F("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"));
    while (1);
  }
  //-------校正--------
}

void loop() {
  MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
  float COppm = MQ7.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  Serial.print(F("CO= "));
  Serial.print(COppm);
  Serial.println(F(" ppm"));
  delay(1000); //Sampling frequency

}
