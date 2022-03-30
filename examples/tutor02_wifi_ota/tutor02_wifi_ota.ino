#define ESP32_CONTROL_10RS    //- ต้องระบุ ชนิดของ Controller ที่ใช้ ในกรณีนี้ใช้ ESP32 Control v1.0RS
#include <ESP32Control.h>     //- Include header ของ ESP32 Control
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ElegantOTA.h>

const char* ssid = "..................";
const char* password = "..................";

WebServer server(80);

/*-------------------------------------------------------------------------
- ติดตั้ง Library โดยค้นหาที่ Library manager โดยใช้คำค้นว่า "ElegantOTA"
- หลังจากติดตั้งเสร็จ เปิด Serial ในตอนเริ่มต้นจะเจอ IP ของบอร์ด เช่นเจอ IP = 192.168.1.13
- ให้เข้าไปที่ 192.168.1.13/update เพื่อเข้าหน้า Wifi OTA update 
-------------------------------------------------------------------------*/


void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/plain", "Hi! I am ESP32.");
  });

  ElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  server.handleClient();
}
