/*
   PMS5003T             DS1302       MQ-7          DHT11           SGP30           LCD          SD Module
   PM1.0 OM2.5 PM10     RTC Time     CO            Temp & Humi     TVOC & ECO2     Display      Read & Write Data
   RX - 4               RST 8        DATA - A0     DATA - 9        SDA - A4        SDA - A4     SS   - pin 10
   TX - 5               CLK 6                                      SCL - A5        SCL - A5     MOSI - pin 11
                        DAT 7                                                                   MISO - pin 12
                                                                                                CLK -  pin 13
*/

#include <PMS.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <MQUnifiedsensor.h>
#include "DHT.h"
#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <LiquidCrystal_PCF8574.h>
#include <SPI.h>
#include "SdFat.h"

/*----------SD Card----------*/
#define SD_CS_PIN SS
File myFile;
SdFat SD;
String filename = "EricTest2.txt";
unsigned long timetowrite = 0;
int waitingtime = 30000;
/*----------SD Card----------*/

/*----------LCD----------*/
LiquidCrystal_PCF8574 lcd(0x3f);
/*----------LCD----------*/

/*----------PMS5003T----------*/
SoftwareSerial Serial1(4, 5);
PMS pms(Serial1);
PMS::DATA data;
/*----------PMS5003T----------*/

/*----------RTC DS1302----------*/
#define RST 8
#define CLK 6
#define DAT 7
DS1302 rtc(RST, CLK, DAT);
char buf[20];
/*----------RTC DS1302----------*/

/*----------MQ-7----------*/
#define placa "Arduino NANO"
#define Voltage_Resolution 5
#define pin A0 //Analog input 0 of your arduino
#define type "MQ-7" //MQ7
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ7CleanAir 27.5 //RS / R0 = 27.5 ppm 
MQUnifiedsensor MQ7(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
/*----------MQ-7----------*/

/*----------DHT11----------*/
#define DHTTYPE DHT11     // DHT型號選擇DHT11
#define DHTPIN 9          // DHT11的DATA腳接在D9
DHT dht(DHTPIN, DHTTYPE);
float t = 0;              // 溫度
float h = 0;              // 濕度
/*----------DHT11----------*/

/*----------SGP30----------*/
Adafruit_SGP30 sgp;
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}
/*----------SGP30----------*/

void setup()
{
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);

  /*----------LCD----------*/
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  /*----------LCD----------*/

  /*----------PMS5003T----------*/
  Serial1.begin(9600);
  /*----------PMS5003T----------*/

  /*----------DHT11----------*/
  dht.begin();
  /*----------DHT11----------*/

  /*----------RTC DS1302----------*/
  rtc.begin();
  rtc.adjust(DateTime(__DATE__, __TIME__));
  if (!rtc.isrunning()) {
    Serial.println(F("RTC is NOT running!"));
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  /*----------RTC DS1302----------*/

  /*----------MQ-7----------*/
  MQ7.setRegressionMethod(1);
  MQ7.setA(99.042); MQ7.setB(-1.518);
  MQ7.init();
  Serial.print(F("Calibrating please wait."));
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
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
  /*----------MQ-7----------*/

  /*----------SGP30----------*/
  Serial.println(F("SGP30 test"));
  if (! sgp.begin()) {
    Serial.println(F("Sensor not found :("));
    while (1);
  }
  Serial.println(F("Found SGP30 serial"));
  /*----------SGP30----------*/

  /*----------SD Card----------*/
  Serial.print(F("Initializing SD card..."));

  //判斷SD模組初始化是否成功
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));
  sdcardtable();
  /*----------SD Card----------*/

}

void loop()
{
  unsigned long currentMillis = millis();
  String totaloutput = "";
  if (pms.read(data))
  {
    /*----------RTC DS1302----------*/
    DateTime now = rtc.now();
    totaloutput = now.tostr(buf);
    /*----------RTC DS1302----------*/

    /*----------DHT11----------*/
    h = dht.readHumidity();
    t = dht.readTemperature();
    totaloutput = totaloutput + "," + t + "," + h;
    /*----------DHT11----------*/

    /*----------MQ-7----------*/
    MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
    float COppm = MQ7.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
    totaloutput = totaloutput + "," + COppm;
    /*----------MQ-7----------*/

    /*----------SGP30----------*/
    sgp.setHumidity(getAbsoluteHumidity(t, h));
    if (! sgp.IAQmeasure()) {
      Serial.println(F("Measurement failed"));
      return;
    }
    if (! sgp.IAQmeasureRaw()) {
      Serial.println(F("Raw Measurement failed"));
      return;
    }
    totaloutput = totaloutput + "," + sgp.TVOC + "," + sgp.eCO2;
    /*----------SGP30----------*/

    /*----------PMS5003T----------*/
    //data.PM_AE_UG_1_0
    //data.PM_AE_UG_10_0
    totaloutput = totaloutput + "," + data.PM_AE_UG_2_5;
    /*----------PMS5003T----------*/

    /*----------LCD----------*/
    lcd.setCursor(0, 0);
    lcd.print(now.tostr(buf));
    /*----------LCD----------*/

    /*----------SD Card----------*/
    if (currentMillis - timetowrite >= waitingtime) {
      myFile = SD.open(filename, FILE_WRITE);
      if (myFile) {
        myFile.println(totaloutput);
        myFile.close();
        lcd.setCursor(0, 1);
        lcd.print(F("Recording....!"));
        digitalWrite(2, LOW);
        digitalWrite(3, HIGH);
      } else {
        Serial.println(F("error opening "));
        lcd.setCursor(0, 1);
        lcd.print(F("Something Wrong!"));
        digitalWrite(2, HIGH);
        digitalWrite(3, LOW);
      }
      myFile = SD.open(filename);
      if (myFile) {
        Serial.println(F("開始讀取"));
        while (myFile.available()) {
          Serial.write(myFile.read());
        }
        Serial.println(F("讀取完畢"));
        myFile.close();
      } else {
        Serial.println(F("error opening "));
      }
      timetowrite = currentMillis;
    }
    /*----------SD Card----------*/
  }
}

/*----------Table----------*/
void sdcardtable() {
  String table = "日期時間,溫度,濕度,CO,TVOC,CO2,PM2.5";
  myFile = SD.open(filename, FILE_WRITE);       // 打開一個檔名為test.txt的檔案，模式為寫入(FILE_WRITE)，若檔案不存在，就會自動建議一個新的檔案
  if (myFile) {                                       // 如果成功打開，就寫入文字
    myFile.println(table);                            // 寫入時間資料
    myFile.close();                                   // 關閉檔案
  } else {
    Serial.println(F("error opening "));         // 如果無法開啟檔案，就在監控視窗顯示訊息
  }
}
/*----------Table----------*/
