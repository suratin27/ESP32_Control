# 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor12_new_monitor\\tutor12_new_monitor.ino"
# 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor12_new_monitor\\tutor12_new_monitor.ino"
# 2 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor12_new_monitor\\tutor12_new_monitor.ino" 2


# 5 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor12_new_monitor\\tutor12_new_monitor.ino" 2

/*---------------------------------------------------------------------

                          ค่าคงที่ของ Library

---------------------------------------------------------------------*/
# 9 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor12_new_monitor\\tutor12_new_monitor.ino"
//BOARD_TXT           คือชื่อของบอร์ด
//BOARD_VERSION       คือค่าคงที่ของบอร์ดเวอร์ชั่นใช้ในการกำหนด IO ของบอร์ด

/*---------------------------------------------------------------------

                        This part for Wifi

---------------------------------------------------------------------*/
# 15 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor12_new_monitor\\tutor12_new_monitor.ino"
long lastval =0;
uint8_t oVal = 0;
const char* ssid = "Tofahome";
const char* password = "0804863439";

void initWiFi(){
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  delay(150);
  WiFi.setHostname("ESP32 Control - RX"); //-ชื่อของบอร์ด มาจากค่าคงที่ของบอร์ด
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

void setup(){
  Serial.begin(115200);
  initWiFi();
  initESP32Monitor(1); //-ค่าคงที่ของบอร์ดเวอร์ชั่น การใช้งานคือใส่ค่าตัวนี้เป็นพารามิเตอร์ให้ฟังก์ชั่น initESP32Monitor ,initIO

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
      updateMValue(oVal,getOutput(oVal));
      oVal += 1;
    }else{
      oVal = 0;
    }
  }
  webMonTask();
}
