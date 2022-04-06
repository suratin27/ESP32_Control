#define ESP32_CONTROL_10RS
#include <ESP32Control.h>

ModbusMaster232 myMaster(1,100,3);      //- พารามิเตอร์ (Serial1,Timeout,Retry)  Timeout คือเวลาที่รอการตอบกลับจาก Slave ,Retry คือถ้าอ่านค่าไม่ได้ให้ทดลองอีกกี่ครั้ง
const int RX2   = 22;                   //- RX pin ของ rs485
const int TX2   = 23;                   //- TX pin ของ rs485 
int lastTime      = 0;
const int peroid  = 1000;

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

void setup(){
  Serial.begin(115200);
  Serial1.begin(9600,SERIAL_8N1,RX2,TX2);               //- ตั้งค่า Serial อันนี้สำคัญมาก Serial1 นี้ต้องสั่ง Begin ตรงนี้ไม่งั้นจะใช้ Modbus อ่านค่าไม่ได้
}

void loop(){
  if(millis() - lastTime > peroid){                     //- อ่านค่าทุกๆ 1 วินาที
    lastTime = millis();
    float InReg = myMaster.readInputRegistersFI(1,0);       //- อ่านค่าจาก slave1 - inputRegistor 0 แบบกลับ word
    int16_t InValue = myMaster.readInputRegistersI(1,0);    //- อ่านค่าจาก slave1 - inputRegistor 0 int16
    myMaster.writeSingleRegisterF(2,0,InReg);               //- เขียนค่าที่ได้ไปที่ slave2 - holdingRegistor 0 แบบไม่กลับ word
  }
}