/*
   Using Arduino Nano + SD Module
   Read & Write Data
   SS   - pin 10
   MOSI - pin 11
   MISO - pin 12
   CLK -  pin 13
   VCC -  5V
   GND -  GND
*/

#include <SPI.h>
//#include <SD.h>
#include "SdFat.h"

//CS腳位，預設是SS，以Uno來說就是腳位10，如果有多個SPI設備，請設不同的腳位
#define SD_CS_PIN SS
File myFile;
SdFat SD;

int a = 0;

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");

  //判斷SD模組初始化是否成功
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void loop() {
  //打開一個檔名為test.txt的檔案，模式為寫入(FILE_WRITE)
  //若檔案不存在，就會自動建議一個新的檔案
  myFile = SD.open("EricChen.txt", FILE_WRITE);

  // 如果成功打開，就寫入文字
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println(a);
    // 關閉檔案
    myFile.close();
    Serial.println("done.");
  } else {
    // 如果無法開啟檔案，就在監控視窗顯示訊息
    Serial.println("error opening test.txt");
  }

  //以下是讀取的部份
  //再打開test.txt
  myFile = SD.open("EricChen.txt");
  if (myFile) {
    Serial.print("開始讀取：  ");
    Serial.println("EricChen.txt");

    //一直讀取檔案內容，直到沒有為止
    while (myFile.available()) {
      Serial.write(myFile.read()); //把讀到的內容顯示在監控視窗
    }
    // 關閉檔案
    Serial.println("讀取完畢");
    Serial.println();
    myFile.close();
  } else {
    // 如果無法開啟檔案，就在監控視窗顯示訊息
    Serial.println("error opening test.txt");
  }

  delay(5000);
  a += 1;
}
