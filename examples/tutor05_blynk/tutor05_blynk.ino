/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define ESP32_CONTROL_10RS    //- กำหนดว่าเราใช้ ESP32 Control v1.0RS

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID ".........................."
#define BLYNK_DEVICE_NAME ".........................."
#define BLYNK_AUTH_TOKEN ".........................."

#include <ESP32Control.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>     //- ต้องติดตั้ง Blynk 2.0 ก่อนครับ

char ssid[] = "JURAPORN_2.4G";
char pass[] = "044801060";
char auth[] = BLYNK_AUTH_TOKEN;

BLYNK_WRITE(V0){      //- Switch 0
 int pinValue = param.asInt();
 if(pinValue){
   setOutput(0);
   Serial.println("Ox0 is set to HIGH");
 }else{
   resetOutput(0);
   Serial.println("Ox0 is reset to LOW");
 }
}
BLYNK_WRITE(V1){      //- Switch 1
 int pinValue = param.asInt();
 if(pinValue){
   setOutput(1);
   Serial.println("Ox1 is set to HIGH");
 }else{
   resetOutput(1);
   Serial.println("Ox1 is reset to LOW");
 }
}

void setup(){
  Serial.begin(9600);   // Debug console
  initIO();             // Start ESP32 Control IO monitor
  Blynk.begin(auth, ssid, pass);
}

void loop(){
  Blynk.run();
}
