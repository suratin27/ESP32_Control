# 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor01_gpio_rlx-32u\\tutor01_gpio_rlx-32u.ino"
# 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor01_gpio_rlx-32u\\tutor01_gpio_rlx-32u.ino"

# 3 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor01_gpio_rlx-32u\\tutor01_gpio_rlx-32u.ino" 2

/*-------------------------------------------------------------------------

initIO();                     - เริ่มต้นการใช้งาน I/O โดยถ้าใช้ v1.0 ให้ใส่พารามิเตอร์ 1 แต่ถ้าใช้ 1.0RS ไม่ต้องใส่อะไร
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
const int peroid = 3000;
uint8_t OutPos = 0;
const uint8_t RX2 = 22; //- ขา RX ของ rs485
const uint8_t TX2 = 23; //- ขา TX ของ rs485

void setup(){
  Serial.begin(9600); //- Serial0 คือ USB port
  Serial2.begin(115200,0x800001c,RX2,TX2);
  initIO(); //- Init อินพุท และ เอ้าพุท
  setOutput(0);
  setOutput(1);
  setOutput(2);
  setOutput(3);
}

void loop(){
  if(millis() - lastime > peroid){
    lastime = millis();
    if(status){
      toggleSTS(3); //- toggle Status LED 3 --> ตรงบริเวณ MCU
      toggleSTS(2); //- toggle Status LED 2 --> ตรงบริเวณ MCU
      toggleSTS(1); //- toggle Status LED 1 --> ตรงบริเวณ MCU

      //toggleOutput(0);
      //toggleOutput(1);
      //toggleOutput(2);
      //toggleOutput(3);
      status = false;
    }else{
      toggleSTS(3); //- toggle Status LED 3 --> ตรงบริเวณ MCU
      toggleSTS(2); //- toggle Status LED 2 --> ตรงบริเวณ MCU
      toggleSTS(1); //- toggle Status LED 1 --> ตรงบริเวณ MCU
      status = true;
    }
  }

  toggleSTS(0); //- toggle Status LED 0 --> ตรงบริเวณ MCU

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
