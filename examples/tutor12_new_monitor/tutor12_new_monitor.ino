#define ESP32_CONTROL_10RX         //- Define ESP32 Control v1.0RX board
#define USE_WEB_MONITOR            //- Ask board to Use Web monitor function
#include <ESP32Control.h>

/*---------------------------------------------------------------------
                          ค่าคงที่ของ Library
---------------------------------------------------------------------*/
//BOARD_TXT           คือชื่อของบอร์ด
//BOARD_VERSION       คือค่าคงที่ของบอร์ดเวอร์ชั่นใช้ในการกำหนด IO ของบอร์ด

/*---------------------------------------------------------------------
                        This part for Wifi
---------------------------------------------------------------------*/
long lastval =0;
uint8_t oVal = 0;
const char* ssid = "Tofahome";
const char* password = "0804863439";

const char *ssidServ = "Solar Monitor 32U";
const char *passwordServ = "12345678";

void initWiFi(){
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(150);
  WiFi.setHostname(BOARD_TXT);                        //-ชื่อของบอร์ด มาจากค่าคงที่ของบอร์ด
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  /*  ----------> AP Mode
  WiFi.softAP(ssidServ, passwordServ);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  */
}

void setup(){
  Serial.begin(115200); 
  initWiFi();
  initESP32Monitor(BOARD_VERSION);                    //-ค่าคงที่ของบอร์ดเวอร์ชั่น การใช้งานคือใส่ค่าตัวนี้เป็นพารามิเตอร์ให้ฟังก์ชั่น initESP32Monitor ,initIO

  delay(1000);
  setMName(0,"Input0",true);
  setMName(1,"Level_Switch1",true);
  setMName(2,"Level_Switch2",true);
  
  setDName(0,"flagSts",true);
  
  setFName(0,"Humidity",true);

  addDiagnostic("Begin to Main loop");
}

void loop(){
  if(millis() - lastval > 2000){
    lastval = millis();
    if(oVal < 4){
      clearAllOutput();
      setOutput(oVal);
      clearMValue();
      updateMValue(oVal,getOutput(oVal));
      oVal += 1;
    }else{
      oVal = 0;
    }
  }
  toggleSTS(1);
  webMonTask();
}