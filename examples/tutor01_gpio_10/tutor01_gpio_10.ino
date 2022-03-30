#define ESP32_CONTROL_10    //- ต้องระบุ ชนิดของ Controller ที่ใช้ ในกรณีนี้ใช้ ESP32 Control v1.0
#include <ESP32Control.h>     //- Include header ของ ESP32 Control

/*-------------------------------------------------------------------------

initIO(1);                     - เริ่มต้นการใช้งาน I/O ,พารามิเตอร์(board model) 0 = 1.0RS ,1 = 1.0
setLed();                     - เซ็ตค่า Status LED 
resetLed();                   - รีเซ็ตค่า Status LED 
toggleLed(uint8_t _pin);      - สลับสถานะ ของ Status LED 
readInput(uint8_t _i);        - อ่านค่าจากอินพุท (อินพุทที่ต้องการอ่านค่า)
setOutput(uint8_t _o);        - เซ็ตค่าเอ้าพุท (เอ้าพุทที่ต้องการ เซ็ตค่า)
resetOutput(uint8_t _o);      - รีเซ็ตค่าเอ้าพุท (เอ้าพุทที่ต้องการ รีเซ็ตค่า)
toggleOutput(uint8_t _o);     - สลับค่าเอ้าพุท (เอ้าพุทที่ต้องการ)
clearAllOutput();             - รีเซ็ตค่าเอ้าพุท ทั้งหมด
setAllOutput();               - เซ็ตค่าเอ้าพุท ทั้งหมด

-------------------------------------------------------------------------*/



unsigned long lastime = 0;
bool status = false;
const int peroid = 5000;
uint8_t OutPos = 0;

void setup(){
  Serial.begin(9600);         //- Serial0 คือ USB port
  initIO(1);                   //- Init อินพุท และ เอ้าพุท
  setAllOutput();
}

void loop(){
  if(millis() - lastime > peroid){
    lastime = millis();
    if(status){
      clearAllOutput();
      status = false;
    }else{
      setAllOutput();
      status = true;
    }
  }

  toggleLed();

  if(!readInput(0)){
    Serial.println("0");
    Serial2.println("0");
  }
  if(!readInput(1)){
    Serial.println("1");
    Serial2.println("1");
  }
  if(!readInput(2)){
    Serial.println("2");
    Serial2.println("2");
  }
  if(!readInput(3)){
    Serial.println("3");
    Serial2.println("3");
  }
  if(!readInput(4)){
    Serial.println("4");
    Serial2.println("4");
  }
  if(!readInput(5)){
    Serial.println("5");
    Serial2.println("5");
  }
  if(!readInput(6)){
    Serial.println("6");
    Serial2.println("6");
  }
  if(!readInput(7)){
    Serial.println("7");
    Serial2.println("7");
  }
  delay(100);
}
