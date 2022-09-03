#include <WiFi.h>

#define ESP32_CONTROL_10RX          //- Define ESP32 Control v1.0RX board
#include <ESP32Control.h>


/*---------------------------------------------------------------------
                        This part for Wifi
---------------------------------------------------------------------*/
const char* ssid = "JURAPORN_2.4G";
const char* password = "044801060";
String hostname = "ESP32 Control - RS";

void initWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(150);
  WiFi.setHostname(hostname.c_str());
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


long lastval =0;
uint8_t oVal = 0;


void setup(){
  Serial.begin(115200); 
  initWiFi();
  #ifdef ESP32_CONTROL_10RS
    initESP32Monitor(0);
    addDiagnostic("Init Web monitor to version 1.0RS");
  #else
    initESP32Monitor(1);
    addDiagnostic("Init Web monitor to version 1.0");
  #endif

  delay(1000);
  setMName(0,"Input0",true);
  setMName(1,"Level_Switch1",true);
  setMName(2,"Level_Switch2",true);
  
  setDName(0,"flagSts",true);
  
  setFName(0,"Humidity",true);

  addDiagnostic("Begin to Main loop");
}

void loop(){
  if(millis() - lastval > 1000){
    lastval = millis();
    if(oVal < 8){
      clearAllOutput();
      setOutput(oVal);
      clearMValue();
      //Serial.println(getOutput(oVal));
      updateMValue(oVal,getOutput(oVal));
      /*
      if(oVal == 7){
        updateMValue(oVal,getOutput(oVal),true);
      }else{
        updateMValue(oVal,getOutput(oVal));
      }*/
      oVal += 1;
    }else{
      oVal = 0;
    }
  }
  webMonTask();
}