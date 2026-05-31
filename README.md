# ESP32-CAM SD Web Logger Dashboard

ESP32-CAM Web Dashboard berbasis **ESPAsyncWebServer** dengan fitur:

* Live Camera Snapshot
* SD Card File Hosting
* Web API Monitoring
* Persistent Logging ke SD Card
* OTA Firmware Update
* REST Endpoint System Status
* Browser Log Viewer
* Static HTML / CSS / JS Hosting

Project ini dirancang untuk **ESP32-CAM AI Thinker** dan menggunakan **SD Card sebagai storage utama** untuk web interface, log system, dan file management.

---

# Features

## Camera Features

* `/snapshot` → Capture JPEG image
* Camera initialization otomatis
* PSRAM auto detection
* VGA / QVGA adaptive mode

## Web Dashboard

Static web file disimpan di SD Card:

```txt
/index.html
/style.css
/app.js
```

Endpoint:

```txt
/
```

## Monitoring API

### System Status

```txt
/status
```

JSON response:

```json
{
  "cpu":17,
  "ram":192,
  "rssi":-62,
  "uptime":"00:12:35",
  "sd":25
}
```

### Ping Test

```txt
/ping
```

Response:

```txt
ESP32 SERVER OK
```

## SD Card Diagnostics

### SD Status

```txt
/sd
```

Menampilkan:

* SD Mount Status
* Card Type
* File Listing

### List Files

```txt
/ls
```

Menampilkan seluruh file di root SD Card.

### Files API

```txt
/files
```

JSON file browser:

```json
[
 {
  "name":"index.html",
  "size":"12 KB"
 }
]
```

---

# Persistent Logging System

System log otomatis disimpan ke:

```txt
/system.log
```

## Raw Log Viewer

```txt
/log
```

## HTML Browser Log Viewer

```txt
/logs
```

Features:

* Auto Refresh
* Browser Terminal View
* Download Log
* Clear Log

## Clear Log

```txt
/clearlog
```

## Download Log

```txt
/downloadlog
```

---

# OTA Firmware Update

Endpoint:

```txt
/ota
```

POST firmware upload untuk update OTA tanpa USB flashing.

---

# AI Endpoint Demo

Demo AI API:

```txt
/ai
```

Response:

```json
{
  "object":"person",
  "confidence":92
}
```

---

# Recording API

Start recording:

```txt
POST /recordStart
```

Stop recording:

```txt
POST /recordStop
```

---

# Hardware Requirements

* ESP32-CAM AI Thinker
* MicroSD Card
* FTDI Programmer
* 5V Stable Power Supply

---

# Arduino IDE Configuration

Gunakan setting berikut:

```txt
Board:
AI Thinker ESP32-CAM

CPU:
240MHz

Flash Mode:
QIO

Flash Frequency:
80MHz

Partition Scheme:
Huge APP

PSRAM:
Enabled

Upload Speed:
115200
```

---

# Required Libraries

Install library berikut:

```txt
ESPAsyncWebServer
AsyncTCP
ArduinoJson
ESP32 Board Package
```

---

# SD Card Structure

Gunakan struktur SD berikut:

```txt
SDCARD/
│
├── index.html
├── style.css
├── app.js
└── system.log
```

---

# Installation

Clone repository:

```bash
git clone https://github.com/USERNAME/ESP32-CAM-SD-WEB-LOGGER.git
```

Open:

```txt
ESP32_CAM_SD_WEB_LOGGER_FINAL.ino
```

Compile.

Upload ke ESP32-CAM.

Open Serial Monitor:

```txt
115200 baud
```

Cari IP Address:

```txt
WiFi Connected IP=192.168.x.x
```

Open browser:

```txt
http://ESP32_IP/
```

---

# Example Routes

| Route          | Description        |
| -------------- | ------------------ |
| `/`            | Dashboard          |
| `/status`      | System Status JSON |
| `/snapshot`    | JPEG Capture       |
| `/sd`          | SD Diagnostics     |
| `/ls`          | File Listing       |
| `/files`       | Files JSON         |
| `/log`         | Raw Log            |
| `/logs`        | Browser Log Viewer |
| `/clearlog`    | Delete Log         |
| `/downloadlog` | Download Log       |
| `/ota`         | OTA Upload         |
| `/ping`        | Connectivity Test  |

---

# Troubleshooting

## SD FAIL

Check:

* FAT32 formatting
* SD card quality
* Wiring
* Power stability

## Camera Init Fail

Check:

* AI Thinker board selection
* PSRAM enabled
* Power supply

## WiFi Not Connecting

Verify:

```cpp
ssid
password
```

Router compatibility matters. Consumer WiFi equipment occasionally behaves like it negotiated standards via interpretive dance.

---

# License

MIT License

---

# Author

ESP32-CAM SD Web Logger Dashboard

Built with ESP32, AsyncWebServer, and a mild refusal to debug embedded systems using only Serial.println().
