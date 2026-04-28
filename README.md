# 胡適科學 Arduino Nano/UNO 感測器範例程式碼 (Hu-Shih Science)

本專案提供一系列用於環境監測的 Arduino Nano 與 UNO 範例程式碼，涵蓋多種空氣品質感測器、即時時鐘 (RTC)、LCD 顯示以及 SD 卡資料記錄功能。

## 專案簡介

本專案旨在提供穩定且易於擴展的環境監測方案。程式碼包含單一感測器的基本測試，以及將多個感測器整合在一起並記錄至 SD 卡的高級範例。

## 主要功能

- **空氣品質監測**：支援 PM2.5 (PMS5003T)、一氧化碳 (MQ-7)、二氧化碳 (SGP30/SenseAir S8)、TVOC (SGP30) 及臭氧 (MQ-131) 偵測。
- **環境數據**：即時溫濕度偵測 (DHT11)。
- **即時時鐘 (RTC)**：使用 DS1302 模組，為數據提供精確的時間戳記。
- **資料儲存 (Data Logging)**：支援將感測數據以 CSV 格式記錄至 SD 卡，方便後續分析。
- **視覺化顯示**：支援 I2C LCD 1602 顯示器，即時呈現感測數據與系統狀態。

## 支援的硬體模組

| 模組名稱 | 功能說明 |
| :--- | :--- |
| **Arduino Nano / UNO** | 主控制板 |
| **PMS5003T** | 懸浮微粒感測器 (PM1.0, PM2.5, PM10) |
| **DHT11** | 數位溫濕度感測器 |
| **SGP30** | 空氣品質感測器 (TVOC, eCO2) |
| **MQ-7** | 一氧化碳 (CO) 感測器 |
| **MQ-131** | 臭氧 (O3) 感測器 |
| **MQ-135** | 空氣品質感測器 (氨氣、苯、酒精、煙霧等) |
| **SenseAir S8** | 紅外線二氧化碳 (CO2) 感測器 |
| **DS1302** | 即時時鐘模組 (RTC) |
| **SD Card Module** | Micro SD 卡讀寫模組 (SPI 介面) |
| **LCD 1602 (I2C)** | 16x2 液晶顯示器 (搭配 PCF8574 轉接板) |

## 接線參考 (以整合版程式為例)

| 模組 | 接腳 | Arduino 接腳 |
| :--- | :--- | :--- |
| **PMS5003T** | RX / TX | D4 / D5 (SoftwareSerial) |
| **DS1302** | RST / CLK / DAT | D8 / D6 / D7 |
| **MQ-7** | Analog Out | A0 |
| **DHT11** | Data Out | D9 |
| **SGP30 / LCD** | SDA / SCL | A4 / A5 (I2C) |
| **SD Module** | CS / MOSI / MISO / CLK | D10 / D11 / D12 / D13 (SPI) |

## 必要程式庫

在編譯之前，請確保已在 Arduino IDE 中安裝以下程式庫：

- `PMS Library`
- `SoftwareSerial` (內建)
- `RTClib`
- `MQUnifiedsensor`
- `DHT sensor library`
- `Adafruit SGP30 Sensor`
- `LiquidCrystal_PCF8574`
- `SdFat`

## 資料夾結構

- `Hu-Shih_Arduino_Nano_[SensorName]`: 個別感測器的測試程式碼。
- `Hu-Shih_Arduino_Nano_SD_RTC_...`: 多感測器整合與 SD 卡紀錄程式碼。

## 使用說明

1. 開啟 Arduino IDE 並載入所需的 `.ino` 檔案。
2. 根據接線表連接硬體。
3. 確認開發板選擇正確 (Arduino Nano 或 UNO)。
4. 編譯並上傳程式碼。
5. 開啟序列埠監控器 (9600 baud) 查看數據輸出。
