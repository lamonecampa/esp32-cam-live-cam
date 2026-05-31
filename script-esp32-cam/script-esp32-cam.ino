#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "FS.h"
#include "SD_MMC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

//================ WIFI =================

const char* ssid =
"SaMaRa-NET-Kav48";

const char* password =
"sayangq1";

//================ STATIC IP ============

//============== CAMERA PINS ============

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5

#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

AsyncWebServer server(80);

bool recording=false;
File videoFile;

//============== LOGGER =================

void writeLog(String msg){

File logFile=
SD_MMC.open(
"/system.log",
FILE_APPEND
);

if(!logFile){

Serial.println(
"LOG OPEN FAIL"
);

return;

}

String line;

line += "[";

line += String(
millis()/1000
);

line += "s] ";

line += msg;

line += "\n";

logFile.print(line);

logFile.close();

Serial.print(line);

}

//============== UPTIME =================

String uptime(){

unsigned long sec=
millis()/1000;

char buf[20];

sprintf(
buf,
"%02lu:%02lu:%02lu",
sec/3600,
(sec%3600)/60,
sec%60
);

return String(buf);

}

//============= CAMERA ==================

void setupCamera(){

camera_config_t config;

config.ledc_channel=
LEDC_CHANNEL_0;

config.ledc_timer=
LEDC_TIMER_0;

config.pin_d0=Y2_GPIO_NUM;
config.pin_d1=Y3_GPIO_NUM;
config.pin_d2=Y4_GPIO_NUM;
config.pin_d3=Y5_GPIO_NUM;
config.pin_d4=Y6_GPIO_NUM;
config.pin_d5=Y7_GPIO_NUM;
config.pin_d6=Y8_GPIO_NUM;
config.pin_d7=Y9_GPIO_NUM;

config.pin_xclk=
XCLK_GPIO_NUM;

config.pin_pclk=
PCLK_GPIO_NUM;

config.pin_vsync=
VSYNC_GPIO_NUM;

config.pin_href=
HREF_GPIO_NUM;

config.pin_sccb_sda=
SIOD_GPIO_NUM;

config.pin_sccb_scl=
SIOC_GPIO_NUM;

config.pin_pwdn=
PWDN_GPIO_NUM;

config.pin_reset=
RESET_GPIO_NUM;

config.xclk_freq_hz=
20000000;

config.pixel_format=
PIXFORMAT_JPEG;

if(psramFound()){

config.frame_size=
FRAMESIZE_VGA;

config.jpeg_quality=10;

config.fb_count=2;

}
else{

config.frame_size=
FRAMESIZE_QVGA;

config.jpeg_quality=12;

config.fb_count=1;

}

esp_camera_init(&config);

}

//=============== WIFI ==================

void setupWiFi(){

WiFi.mode(
WIFI_STA
);

writeLog(
"Connecting WiFi..."
);

WiFi.begin(
ssid,
password
);

while(
WiFi.status()!=WL_CONNECTED
){

delay(500);

Serial.print(".");

}

String ip=
WiFi.localIP()
.toString();

Serial.println();

Serial.println(
"CONNECTED"
);

Serial.println(ip);

writeLog(
"WiFi Connected IP="
+
ip
);

}

//=============== SD ====================

void setupSD(){

if(!SD_MMC.begin()){

Serial.println(
"SD FAIL"
);

}
else{

Serial.println(
"SD OK"
);

File f=
SD_MMC.open(
"/index.html"
);

if(f){

Serial.println(
"INDEX FOUND"
);

}
else{

Serial.println(
"INDEX MISSING"
);

}

}

}

void setupSDCheck(){

server.on(
"/sd",
HTTP_GET,
[](AsyncWebServerRequest *req){

String html;

html += "<h2>ESP32 SD TEST</h2>";

if(!SD_MMC.begin()){

req->send(
200,
"text/html",
"<h2>SD INIT FAIL</h2>"
);

return;

}

uint8_t card=
SD_MMC.cardType();

if(card==CARD_NONE){

req->send(
200,
"text/html",
"<h2>NO SD CARD</h2>"
);

return;

}

html +=
"<h3>SD OK</h3>";

html +=
"<p>CardType=";

html += String(card);

html += "</p>";

File root=
SD_MMC.open("/");

if(!root){

req->send(
200,
"text/html",
"<h2>ROOT OPEN FAIL</h2>"
);

return;

}

if(!root.isDirectory()){

req->send(
200,
"text/html",
"<h2>NOT DIRECTORY</h2>"
);

return;

}

File file=
root.openNextFile();

html+="<ul>";

while(file){

html+="<li>";

html+=String(
file.name()
);

html+=" | ";

html+=String(
file.size()
);

html+=" bytes";

html+="</li>";

file=
root.openNextFile();

}

html+="</ul>";

req->send(
200,
"text/html",
html
);

});
}

//============ STATIC FILES =============

//============ STATIC FILES =============

void setupStaticFiles(){

// ROOT HTML

server.on(
"/",
HTTP_GET,
[](AsyncWebServerRequest *req){

req->send(
SD_MMC,
"/index.html",
"text/html"
);

});

// CSS

server.on(
"/style.css",
HTTP_GET,
[](AsyncWebServerRequest *req){

req->send(
SD_MMC,
"/style.css",
"text/css"
);

});

// JS

server.on(
"/app.js",
HTTP_GET,
[](AsyncWebServerRequest *req){

req->send(
SD_MMC,
"/app.js",
"application/javascript"
);

});

// TEST HTML

server.on(
"/test-html",
HTTP_GET,
[](AsyncWebServerRequest *req){

req->send(
200,
"text/plain",

SD_MMC.exists("/index.html")
?
"INDEX FOUND"
:
"INDEX MISSING"

);

});

// TEST CSS

server.on(
"/test-css",
HTTP_GET,
[](AsyncWebServerRequest *req){

req->send(
200,
"text/plain",

SD_MMC.exists("/style.css")
?
"STYLE FOUND"
:
"STYLE MISSING"

);

});

// TEST JS

server.on(
"/test-js",
HTTP_GET,
[](AsyncWebServerRequest *req){

req->send(
200,
"text/plain",

SD_MMC.exists("/app.js")
?
"APP FOUND"
:
"APP MISSING"

);

});

// LIST FILES

server.on(
"/ls",
HTTP_GET,
[](AsyncWebServerRequest *req){

String out;

File root=
SD_MMC.open("/");

File file=
root.openNextFile();

while(file){

out +=
String(file.name());

out += "\n";

file=
root.openNextFile();

}

req->send(
200,
"text/plain",
out
);

});

}
//============= STATUS ==================

void setupStatus(){

server.on(
"/status",
HTTP_GET,
[](AsyncWebServerRequest *req){

DynamicJsonDocument doc(512);

doc["cpu"]=
random(5,35);

doc["ram"]=
ESP.getFreeHeap()/1024;

doc["rssi"]=
WiFi.RSSI();

doc["uptime"]=
uptime();

doc["sd"]=
25;

String out;

serializeJson(
doc,
out
);

req->send(
200,
"application/json",
out
);

});

}

//============= SNAPSHOT ================

void setupSnapshot(){

server.on(
"/snapshot",
HTTP_GET,
[](AsyncWebServerRequest *req){

camera_fb_t *fb=
esp_camera_fb_get();

if(!fb){

req->send(
500,
"text/plain",
"capture fail"
);

return;

}

AsyncWebServerResponse *response=
req->beginResponse_P(
200,
"image/jpeg",
fb->buf,
fb->len
);

req->send(response);

esp_camera_fb_return(fb);

});

}

//============= STREAM ==================

void setupStream(){

server.on(
"/stream",
HTTP_GET,
[](AsyncWebServerRequest *request){

request->send(
200,
"text/plain",
"Use snapshot demo first"
);

});

}

//============= FILES ===================

void setupFiles(){

server.on(
"/files",
HTTP_GET,
[](AsyncWebServerRequest *req){

String json="[";

File root=
SD_MMC.open("/");

File f=
root.openNextFile();

bool first=true;

while(f){

if(!first)
json+=",";

json+="{";

json+="\"name\":\"";
json+=f.name();
json+="\",";

json+="\"size\":\"";
json+=String(
f.size()/1024
);
json+=" KB\"";

json+="}";

first=false;

f=root.openNextFile();

}

json+="]";

req->send(
200,
"application/json",
json
);

});

}

//============= AI ======================

void setupAI(){

server.on(
"/ai",
HTTP_GET,
[](AsyncWebServerRequest *req){

req->send(
200,
"application/json",
"{\"object\":\"person\",\"confidence\":92}"
);

});

}

//============= CONFIG ==================

void setupConfig(){

server.on(
"/config",
HTTP_POST,
[](AsyncWebServerRequest *req){

req->send(
200,
"text/plain",
"saved"
);

});

}

//============= RECORD ==================

void setupRecord(){

server.on(
"/recordStart",
HTTP_POST,
[](AsyncWebServerRequest *req){

recording=true;

req->send(
200,
"text/plain",
"recording"
);

});

server.on(
"/recordStop",
HTTP_POST,
[](AsyncWebServerRequest *req){

recording=false;

req->send(
200,
"text/plain",
"stopped"
);

});

}

//============== OTA ====================

void setupOTA(){

server.on(
"/ota",
HTTP_POST,

[](AsyncWebServerRequest *request){

request->send(
200,
"text/plain",
"OTA DONE"
);

ESP.restart();

},

[](AsyncWebServerRequest *request,
String filename,
size_t index,
uint8_t *data,
size_t len,
bool final){

if(!index){

Update.begin(
UPDATE_SIZE_UNKNOWN
);

}

Update.write(
data,
len
);

if(final){

Update.end(true);

}

});

}

//============== SETUP ==================

void setup(){

WRITE_PERI_REG(
RTC_CNTL_BROWN_OUT_REG,
0
);


Serial.begin(115200);

setupWiFi();

setupSD();

setupSDCheck();

setupCamera();

setupStaticFiles();

setupStatus();

setupSnapshot();

setupStream();

setupFiles();

setupAI();

setupConfig();

setupRecord();

setupOTA();

server.on(
"/ping",
HTTP_GET,
[](AsyncWebServerRequest *req){

req->send(
200,
"text/plain",
"ESP32 SERVER OK"
);

});

server.begin();

}

//=============== LOOP ==================

void loop(){

delay(10);

}
