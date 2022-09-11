#include <Arduino.h>
#line 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor01_gpio_20rxo\\tutor01_gpio_20rxo.ino"
#line 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor01_gpio_20rxo\\tutor01_gpio_20rxo.ino"
#define ESP32_CONTROL_20RXO    //- ต้องระบุ ชนิดของ Controller ที่ใช้ ในกรณีนี้ใช้ ESP32 Control v2.0RXO
#include <ESP32Control.h>     //- Include header ของ ESP32 Control

/*-------------------------------------------------------------------------

initIO(BOARD_VERSION);                     - เริ่มต้นการใช้งาน I/O โดยใส่พารามิเตอร์ BOARD_VERSION
setLed();                     - เซ็ตค่า   STS LED 
resetLed();                   - รีเซ็ตค่า  STS LED 
toggleLed();                  - สลับสถานะของ   STS LED 
readInput(uint8_t _i);        - อ่านค่าจากอินพุท (อินพุทที่ต้องการอ่านค่า)  --> ค่าพารามิเตอร์ตั้งแต่ 0 - 7
setOutput(uint8_t _o);        - เซ็ตค่าเอ้าพุท (เอ้าพุทที่ต้องการ เซ็ตค่า)    --> ค่าพารามิเตอร์ตั้งแต่ 0 - 4  
resetOutput(uint8_t _o);      - รีเซ็ตค่าเอ้าพุท (เอ้าพุทที่ต้องการ รีเซ็ตค่า)  --> ค่าพารามิเตอร์ตั้งแต่ 0 - 4
toggleOutput(uint8_t _o);     - สลับค่าเอ้าพุท (เอ้าพุทที่ต้องการ)         --> ค่าพารามิเตอร์ตั้งแต่ 0 - 4
toggleSTS(uint8_t _led);      - สลับสถานะของ LED Status --> ค่าพารามิเตอร์ตั้งแต่ 0 - 3
clearSTS();                   - เคลียร์สถานะ LED Status
-------------------------------------------------------------------------*/


unsigned long lastime = 0;
bool status = false;
const int peroid = 200;
uint8_t OutPos = 0;
const uint8_t RX2 = 22;       //- ขา RX ของ rs485
const uint8_t TX2 = 23;       //- ขา TX ของ rs485

#line 26 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor01_gpio_20rxo\\tutor01_gpio_20rxo.ino"
void setup();
#line 33 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor01_gpio_20rxo\\tutor01_gpio_20rxo.ino"
void loop();
#line 26 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor01_gpio_20rxo\\tutor01_gpio_20rxo.ino"
void setup(){
  Serial.begin(9600);         //- Serial0 คือ USB port
  Serial2.begin(115200,SERIAL_8N1,RX2,TX2); 
  initIO(BOARD_VERSION);      //- Init อินพุท และ เอ้าพุท
  setAllOutput();
}

void loop(){
  if(millis() - lastime > peroid){
    lastime = millis();
    if(status){
      toggleLed();
      status = false;
    }else{
      status = true;
    }
  }

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

