# 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor13_modbusio_monitor\\tutor13_modbusio_monitor.ino"
# 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor13_modbusio_monitor\\tutor13_modbusio_monitor.ino"
# 2 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor13_modbusio_monitor\\tutor13_modbusio_monitor.ino" 2


# 5 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor13_modbusio_monitor\\tutor13_modbusio_monitor.ino" 2

/*---------------------------------------------------------------------

                          ค่าคงที่ของ Library

---------------------------------------------------------------------*/
# 9 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor13_modbusio_monitor\\tutor13_modbusio_monitor.ino"
//BOARD_TXT           คือชื่อของบอร์ด
//BOARD_VERSION       คือค่าคงที่ของบอร์ดเวอร์ชั่นใช้ในการกำหนด IO ของบอร์ด

/*---------------------------------------------------------------------

                        This part for Wifi

---------------------------------------------------------------------*/
# 15 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor13_modbusio_monitor\\tutor13_modbusio_monitor.ino"
long lastval =0;
uint8_t oVal = 0;
const char* ssid = "Tofahome";
const char* password = "0804863439";

/*---------------------------------------------------------------------

                        Modbus TCP Master part

---------------------------------------------------------------------*/
# 23 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor13_modbusio_monitor\\tutor13_modbusio_monitor.ino"
ModbusTCPMaster master; //- ประกาศ Modbus master object
WiFiClient slave; //- ประกาศ Wifi client
uint8_t slaveIp[] = { 192, 168, 0, 15 }; //- Slave IP Address
uint16_t slavePort = 502; //- Communication port
unsigned long slaveMillis;
bool inputBuffer[8];
uint16_t iregBuffer[8];

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
  setMName(10,"MV0_Auto",true);
  setMName(11,"MV1_Auto",true);
  setMName(12,"MV2_Auto",true);
  setMName(13,"MV3_Auto",true);

  setDName(0,"flagSts",true);

  setFName(0,"Humidity",true);

  addDiagnostic("Begin to Main loop");
}

void loop(){
  if (!slave.connected()) { //- ทำงานเมื่อยังไม่สามารถเชื่อมต่อกับ Slave
    if(millis() - slaveMillis > 2000){
      slaveMillis = millis();
      slave.stop(); //- หยุดการเชื่อมต่อเดิม
      addDiagnostic("Try to connect to Modbus slave.");
      slave.connect(slaveIp, slavePort); //- เริ่มการเชื่อมต่อใหม่
    }
  }

  if(millis() - lastval > 1000){
    lastval = millis();
    if(slave.connected()) { //- ใช้ในการอ่านค่าจาก Slave เมื่อเชื่อมต่อกับ Slave แล้ว
      master.readInputRegisters(slave, 1, 0, 4, iregBuffer);
      //float volt = *(float*) &iregBuffer[0];                  //- แปลงตำแหน่ง 0-1 เป็น float
      //float current = *(float*) &iregBuffer[2];               //- แปลงตำแหน่ง 2-3 เป็น float
      delay(50);
      master.readDiscreteInputs(slave, 1, 0, 1, inputBuffer);
      //updateMValue(10,inputBuffer[0],true);          //- Update Modbus Input module value to Web monitor
      //updateMValue(11,inputBuffer[1],true);
      //updateMValue(12,inputBuffer[2],true);
      //updateMValue(13,inputBuffer[3],true);
    }
  }
  webMonTask();
}
