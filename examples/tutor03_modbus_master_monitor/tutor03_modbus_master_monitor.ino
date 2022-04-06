/*-----------------------------------------------------------------------------------
  readCoilsB(uint8_t,uint16_t);                             //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการอ่านค่า)                          - คืนค่าเป็น 0,1,FF(เมื่ออ่านค่าไม่ได้)
  readCoilsU(uint8_t,uint16_t,uint16_t);                    //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการอ่านค่า ,จำนวน bit ที่ต้องการอ่านค่า)  - คืนค่าเป็น word
  readDiscreteInputsB(uint8_t,uint16_t,uint8_t);            //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการอ่านค่า)                          - คืนค่าเป็น 0,1,FF(เมื่ออ่านค่าไม่ได้)
  readDiscreteInputsU(uint8_t,uint16_t,uint16_t);           //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการอ่านค่า ,จำนวน bit ที่ต้องการอ่านค่า)  - คืนค่าเป็น word
  writeSingleCoilB(uint8_t,uint16_t, uint8_t);              //พารามิเตอร์ (slave address ,address ,bit ที่ต้องการเขียนค่า)
  readHoldingRegistersI(uint8_t,uint16_t);                  //พารามิเตอร์ (slave address ,address)                                            - คืนค่าเป็น word
  readHoldingRegistersIn(uint8_t,uint16_t,uint16_t,uint16_t*); //พารามิเตอร์ (slave address ,address ,จำนวน word ที่ต้องการ ,ตัวแปรที่ต้องการเก็บค่า)
  readHoldingRegistersF(uint8_t,uint16_t);                  //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง
  readHoldingRegistersFI(uint8_t,uint16_t);                 //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง แบบกลับ word
  readHoldingRegistersFAI(uint8_t,uint16_t);                //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง แบบกลับ ทั้ง 4 byte
  readInputRegistersF(uint8_t,uint16_t);                    //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง
  readInputRegistersFI(uint8_t,uint16_t);                   //พารามิเตอร์ (slave address ,address)              - คืนค่าเป็นจำนวนทศนิยม 2 ตำแหน่ง แบบกลับ word
  writeSingleRegisterF(uint8_t,uint16_t, float);            //พารามิเตอร์ (slave address ,address ,จำนวนที่ต้องการเขียน(float)) 
  writeSingleRegisterFI(uint8_t,uint16_t, float);           //พารามิเตอร์ (slave address ,address ,จำนวนที่ต้องการเขียน(float)) แบบกลับ word
  writeSingleRegisterFAI(uint8_t,uint16_t, float);          //พารามิเตอร์ (slave address ,address ,จำนวนที่ต้องการเขียน(float))  แบบกลับ ทั้ง 4 byte
  writeSingleRegisterI(uint8_t,uint16_t,int16_t);
-----------------------------------------------------------------------------------*/

#define ESP32_CONTROL_10RS
#include <ESP32Control.h>

#define HUMIDITY  0
#define HUMIDITY_SETPOINT 0

ModbusMaster232 myMaster(1,100,3);      //- พารามิเตอร์ (Serial1,Timeout,Retry)  Timeout คือเวลาที่รอการตอบกลับจาก Slave ,Retry คือถ้าอ่านค่าไม่ได้ให้ทดลองอีกกี่ครั้ง
const int RX2   = 22;                   //- RX pin ของ rs485
const int TX2   = 23;                   //- TX pin ของ rs485 
int lastTime      = 0;
const int peroid  = 1000;

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

void webModbusCallback(uint8_t func, uint16_t dAdd,bool iBit ,uint16_t iVal ,float fVal){
  switch (func){
  case 0x01:
    
    break;
  case 0x02:
    
    break;
  case 0x03:
    if(iVal > 0){
      updateDValue(dAdd+31,iVal);
    }else if(fVal > 0.0){
      updateFValue(dAdd+31,fVal);
    }
    break;
  case 0x04:
    if(iVal > 0){
      updateDValue(dAdd,iVal);
    }else if(fVal > 0.0){
      updateFValue(dAdd,fVal);
    }
    break;
  }
}

void setup(){
  Serial.begin(115200);
  Serial1.begin(115200,SERIAL_8N1,RX2,TX2);   //- ตั้งค่า Serial อันนี้สำคัญมาก Serial1 นี้ต้องสั่ง Begin ตรงนี้ไม่งั้นจะใช้ Modbus อ่านค่าไม่ได้
  initWiFi();
  myMaster.setCallback(webModbusCallback);    //- Set callback for Modbus master
  #ifdef ESP32_CONTROL_10RS                   //- ใช้ module version 1.0RS
    initESP32Monitor(0);
    addDiagnostic("Init Web monitor to version 1.0RS");
  #else
    initESP32Monitor(1);
    addDiagnostic("Init Web monitor to version 1.0");
  #endif
  delay(1000);

  setDName(0,"Humidity",true);              //Set D0 memory name
  setDName(31,"Humidity Setpoint",true);    //Set D31 memory name
  addDiagnostic("Begin to Main loop");
}

void loop(){
  if(millis() - lastTime > peroid){                       //- อ่านค่าทุกๆ 1 วินาที
    lastTime = millis();
    uint16_t IReg = myMaster.readInputRegistersI(1,HUMIDITY);               //- อ่านค่าจาก slave1 - Humidity
    uint16_t HReg = myMaster.readHoldingRegistersI(1,HUMIDITY_SETPOINT);    //- อ่านค่าจาก slave1 - Humidity-setpoint
  }
  webMonTask();                                           //Web monitor loop task
}
