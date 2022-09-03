#define ESP32_CONTROL_10RX    //- ต้องระบุ ชนิดของ Controller ที่ใช้ ในกรณีนี้ใช้ ESP32 Control v1.0RX
#include <ESP32Control.h>     //- Include header ของ ESP32 Control

/*-------------------------------------------------------------------------

initIO();                     - เริ่มต้นการใช้งาน I/O โดยถ้าใช้ v1.0 ให้ใส่พารามิเตอร์ 1 แต่ถ้าใช้ 1.0RS ไม่ต้องใส่อะไร
setLed();                     - เซ็ตค่า Status LED 
resetLed();                   - รีเซ็ตค่า Status LED 
toggleLed();                  - สลับสถานะ ของ Status LED 
readInput(uint8_t _i);        - อ่านค่าจากอินพุท (อินพุทที่ต้องการอ่านค่า)
setOutput(uint8_t _o);        - เซ็ตค่าเอ้าพุท (เอ้าพุทที่ต้องการ เซ็ตค่า)
resetOutput(uint8_t _o);      - รีเซ็ตค่าเอ้าพุท (เอ้าพุทที่ต้องการ รีเซ็ตค่า)
toggleOutput(uint8_t _o);     - สลับค่าเอ้าพุท (เอ้าพุทที่ต้องการ)
clearAllOutput();             - รีเซ็ตค่าเอ้าพุท ทั้งหมด
setAllOutput();               - เซ็ตค่าเอ้าพุท ทั้งหมด

-------------------------------------------------------------------------*/



unsigned long lastime = 0;
bool status = false;
const int peroid = 10000;
uint8_t OutPos = 0;
const uint8_t RX2 = 22;       //- ขา RX ของ rs485
const uint8_t TX2 = 23;       //- ขา TX ของ rs485

void setup(){
  Serial.begin(9600);         //- Serial0 คือ USB port
  Serial2.begin(115200,SERIAL_8N1,RX2,TX2); 
  initIO();                   //- Init อินพุท และ เอ้าพุท
  setAllOutput();
}

void loop(){
  if(millis() - lastime > peroid){
    lastime = millis();
    if(status){
      //clearAllOutput();
      status = false;
    }else{
      setAllOutput();
      status = true;
    }
  }

  toggleLed();

  if(readInput(0)){
    Serial.println("0");
    Serial2.println("0");
  }
  if(readInput(1)){
    Serial.println("1");
    Serial2.println("1");
  }
  if(readInput(2)){
    Serial.println("2");
    Serial2.println("2");
  }
  if(readInput(3)){
    Serial.println("3");
    Serial2.println("3");
  }
  if(readInput(4)){
    Serial.println("4");
    Serial2.println("4");
  }
  if(readInput(5)){
    Serial.println("5");
    Serial2.println("5");
  }
  if(readInput(6)){
    Serial.println("6");
    Serial2.println("6");
  }
  if(readInput(7)){
    Serial.println("7");
    Serial2.println("7");
  }
  delay(100);
}
