#define ESP32_CONTROL_10RS

#include <ESP32Control.h>

/*
  bool _readCoils(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
  bool _readDiscreteInputs(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
  bool _readHoldingRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
  bool _readInputRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
  bool writeSingleCoil(WiFiClient &client, uint8_t slave, uint16_t addr, bool value);
  bool writeSingleRegister(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t value);
  bool writeMultipleCoils(WiFiClient &client, uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity);
  bool writeMultipleRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity);

  void readCoils(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, bool* storage);
  void readDiscreteInputs(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, bool* storage);
  void readHoldingRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, uint16_t* storage);
  void readInputRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, uint16_t* storage);
*/

ModbusTCPMaster master;                       //- ประกาศ Modbus master object
WiFiClient slave;                             //- ประกาศ Wifi client
uint8_t slaveIp[] = { 192, 168, 0, 15 };      //- Slave IP Address
uint16_t slavePort = 502;                     //- Communication port

char ssid[] = "Tofahome";
char pass[] = "0804863439";
String hostname = "ESP32 Control - RS";

uint32_t lastSentTime = 0;            //- เก็บค่าสถานะการอ่าน-เขียนค่าจาก Slave
unsigned long lasttime = 0;           //- เก็บค่าสถานะการพยายามเชื่อมต่อเมื่อ หลุดการเชื่อมต่อ
uint16_t holdingData[20];             //- ค่า Holding registor ประกาศเผื่อไว้เยอะๆก็ได้ครับ
bool coilData[65];                    //- ค่า Coil

void initWiFi(){            //- ฟังก์ชั่นการเชื่อมต่อ Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(150);
  WiFi.setHostname(hostname.c_str());
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");         //- แสดงค่า IP ของ ESP32 Control 1.0RS
  Serial.println(WiFi.localIP());       //- แสดงค่า IP ของ ESP32 Control 1.0RS
}

void setup(){
  Serial.begin(115200);
  initWiFi();
}

void loop(){
  if (!slave.connected()) {               //- ทำงานเมื่อยังไม่สามารถเชื่อมต่อกับ Slave
    if(millis() - lasttime > 2000){
      slave.stop();                       //- หยุดการเชื่อมต่อเดิม
      slave.connect(slaveIp, slavePort);  //- เริ่มการเชื่อมต่อใหม่
    }
  }

  if (slave.connected()) {
    if (millis() - lastSentTime > 500) {
      lastSentTime = millis();
      master.readInputRegisters(slave,2,0,6,holdingData);    //- อ่าน 6 ค่าจาก slave 1

      float volt = *(float*) &holdingData[0];                  //- แปลงตำแหน่ง 0-1 เป็น float
      float current = *(float*) &holdingData[2];               //- แปลงตำแหน่ง 2-3 เป็น float

      Serial.print("Volt: ");
      Serial.println(volt);
      Serial.print("Current: ");
      Serial.println(current);
      master.readDiscreteInputs(slave,31,0,25,coilData);     
    }
  }  //
}