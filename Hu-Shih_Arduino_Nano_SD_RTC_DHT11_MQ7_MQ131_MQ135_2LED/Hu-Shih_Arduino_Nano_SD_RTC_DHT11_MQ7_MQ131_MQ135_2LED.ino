/*
   SD Module             DS1302       DHT11           MQ-7          MQ-131       MQ-135        LCD
   Read & Write Data     RTC Time     Temp & Humi     Detect CO     O3 + Cl2     NH4           Display Data
   SS   - 10             RST - 8      Data - 9        AO   - A0     AO   - A1    AO   - A2     A4→SDA
   MOSI - 11             CLK - 6                                                               A5→SCL
   MISO - 12             DAT - 7
   CLK  - 13
*/

#include <SPI.h>
#include "SdFat.h"
#include <RTClib.h>
#include "DHT.h"
#include <MQUnifiedsensor.h>

/*----------SD Card----------*/
#define SD_CS_PIN SS              // CS腳位，預設是SS，以Uno來說就是腳位10，如果有多個SPI設備，請設不同的腳位
File myFile;
SdFat SD;
String filename = "EricTest5.txt"; // 檔案名稱
/*----------SD Card----------*/

/*----------RTC Time----------*/
#define RST 8
#define CLK 6
#define DAT 7
DS1302 rtc(RST, CLK, DAT);
char buf[20];                    // buffer for DateTime.tostr
/*----------RTC Time----------*/

/*----------DHT11----------*/
#define DHTTYPE DHT11            // DHT型號選擇DHT11
#define DHTPIN 9                 // DHT11的DATA腳接在D9
DHT dht(DHTPIN, DHTTYPE);        // 初始化DHT11
/*----------DHT11----------*/

/*----------MQ-7----------*/
#define placa0 "Arduino NANO"
#define Voltage_Resolution0 5
#define pin0 A0 //Analog input 0 of your arduino
#define type0 "MQ-7" //MQ7
#define ADC_Bit_Resolution0 10 // For arduino UNO/MEGA/NANO
#define RatioMQ7CleanAir0 27.5 //RS / R0 = 27.5 ppm 
MQUnifiedsensor MQ7(placa0, Voltage_Resolution0, ADC_Bit_Resolution0, pin0, type0);
/*----------MQ-7----------*/

/*----------MQ-131----------*/
#define placa1 "Arduino NANO"
#define Voltage_Resolution1 5
#define pin1 A1 //Analog input 0 of your arduino
#define type1 "MQ-131" //MQ131
#define ADC_Bit_Resolution1 10 // For arduino UNO/MEGA/NANO
#define RatioMQ131CleanAir1 27.5 //RS / R0 = 27.5 ppm 
MQUnifiedsensor MQ131(placa1, Voltage_Resolution1, ADC_Bit_Resolution1, pin1, type1);
/*----------MQ-131----------*/

/*----------MQ-135----------*/
#define placa2 "Arduino NANO"
#define Voltage_Resolution2 5
#define pin2 A2 //Analog input 0 of your arduino
#define type2 "MQ-135" //MQ135
#define ADC_Bit_Resolution2 10 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir2 27.5 //RS / R0 = 27.5 ppm 
MQUnifiedsensor MQ135(placa2, Voltage_Resolution2, ADC_Bit_Resolution2, pin2, type2);
/*----------MQ-135----------*/

void setup() {

  Serial.begin(9600);

  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);

  /*----------SD Card----------*/
  Serial.print(F("Initializing SD card..."));            // 判斷SD模組初始化是否成功
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));
  sdcardtable();
  /*----------SD Card----------*/

  /*----------RTC Time----------*/
  rtc.begin();
  rtc.adjust(DateTime(__DATE__, __TIME__));
  if (!rtc.isrunning()) {
    Serial.println(F("RTC is NOT running!"));
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  /*----------RTC Time----------*/

  /*----------DHT11----------*/
  dht.begin();
  /*----------DHT11----------*/

  /*----------MQ-7----------*/
  MQ7.setRegressionMethod(1);
  MQ7.setA(99.042); MQ7.setB(-1.518);
  MQ7.init();
  Serial.println(F("Calibrating please wait."));
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ7.calibrate(RatioMQ7CleanAir0);
  }
  MQ7.setR0(calcR0 / 10);
  Serial.println(F("MQ-7  done!."));
  if (isinf(calcR0)) {
    Serial.println(F("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"));
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println(F("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"));
    while (1);
  }
  /*----------MQ-7----------*/

  /*----------MQ-131----------*/
  MQ131.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ131.setA(23.943); MQ131.setB(-1.11); // Configurate the ecuation values to get O3 concentration
  /*
    Exponential regression:
    GAS     | a      | b
    CL2     | 47.209 | -1.186
    O3      | 23.943 | -1.11
  */
  MQ131.init();
  calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ131.calibrate(RatioMQ131CleanAir1);
  }
  MQ131.setR0(calcR0 / 10);
  Serial.println(F("MQ-131  done!."));

  if (isinf(calcR0)) {
    Serial.println(F("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"));
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println(F("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"));
    while (1);
  }
  /*----------MQ-131----------*/

  /*----------MQ-135----------*/
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setA(102.2); MQ135.setB(-2.473); // Configurate the ecuation values to get NH4 concentration
  MQ135.init();
  calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir2);
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println(F("MQ-135  done!."));

  if (isinf(calcR0)) {
    Serial.println(F("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"));
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println(F("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"));
    while (1);
  }
  /*----------MQ-135----------*/

  delay(1000);
}

void loop() {
  String totaloutput = "";
  /*----------RTC Time----------*/
  DateTime now = rtc.now();
  totaloutput = now.tostr(buf);
  /*----------RTC Time----------*/

  /*----------DHT11----------*/
  float h = dht.readHumidity();                       // 宣告float變數 h 並賦予濕度數值
  float t = dht.readTemperature();                    // 選告float變數 t 並賦予溫度數值
  totaloutput = totaloutput + "," + t + "," + h;
  /*----------DHT11----------*/

  /*----------MQ-7----------*/
  MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
  float COppm = MQ7.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  totaloutput = totaloutput + "," + COppm;
  /*----------MQ-7----------*/

  /*----------MQ-131----------*/
  MQ131.setA(23.943); MQ131.setB(-1.11); // Configurate the ecuation values to get O3 concentration
  MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
  float O3ppb = MQ131.readSensorR0Rs(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  delay(500);
  MQ131.setA(47.209); MQ131.setB(-1.186); // Configurate the ecuation values to get Cl2 concentration
  MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
  float Cl2ppm = MQ131.readSensorR0Rs(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  delay(500);
  totaloutput = totaloutput + "," + O3ppb + "," + Cl2ppm;
  /*----------MQ-131----------*/

  /*----------MQ-135----------*/
  MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
  float NH4ppm = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  totaloutput = totaloutput + "," + NH4ppm;
  /*----------MQ-135----------*/

  /*----------SD Card----------*/
  myFile = SD.open(filename, FILE_WRITE);       // 打開一個檔名為test.txt的檔案，模式為寫入(FILE_WRITE)，若檔案不存在，就會自動建議一個新的檔案
  if (myFile) {                                       // 如果成功打開，就寫入文字
    Serial.println(F("Writing to test.txt..."));
    myFile.println(totaloutput);                          // 寫入時間資料
    myFile.close();                                   // 關閉檔案
    Serial.println(F("done."));
    digitalWrite(2,LOW);
    digitalWrite(3,HIGH);
  } else {
    Serial.println(F("error opening test.txt")); // 如果無法開啟檔案，就在監控視窗顯示訊息
    digitalWrite(2,HIGH);
    digitalWrite(3,LOW);
  }

  myFile = SD.open(filename);                   // 打開並讀取SD資料
  if (myFile) {
    Serial.print(F("開始讀取：  "));
    Serial.println(filename);
    while (myFile.available()) {                      // 一直讀取檔案內容，直到沒有為止
      Serial.write(myFile.read());                    // 把讀到的內容顯示在監控視窗
    }
    Serial.println(F("讀取完畢"));                        // 關閉檔案
    Serial.println();
    myFile.close();
  } else {
    Serial.println(F("error opening test.txt"));        // 如果無法開啟檔案，就在監控視窗顯示訊息
  }
  /*----------SD Card----------*/

  delay(9000);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
}

/*----------Table----------*/
void sdcardtable() {
  String table = "日期時間,溫度,濕度,CO,O3,Cl2,NH4";
  myFile = SD.open(filename, FILE_WRITE);       // 打開一個檔名為test.txt的檔案，模式為寫入(FILE_WRITE)，若檔案不存在，就會自動建議一個新的檔案
  if (myFile) {                                       // 如果成功打開，就寫入文字
    myFile.println(table);                            // 寫入時間資料
    myFile.close();                                   // 關閉檔案
  } else {
    Serial.println(F("error opening test.txt"));         // 如果無法開啟檔案，就在監控視窗顯示訊息
  }
}
/*----------Table----------*/
