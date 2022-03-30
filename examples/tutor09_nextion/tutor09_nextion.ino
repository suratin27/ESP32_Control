#define ESP32_CONTROL_10RS

#include <EasyNextionLibrary.h>
#include <ESP32Control.h>

ModbusMaster232 myMaster(1,1);
EasyNex myNex(Serial1);
int lastMills1,lastMills2;
int val;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600,SERIAL_8N1,22,23);
  initIO(0);
  //myNex.begin(9600);    //- ห้าม init ใหม่ เพราะจะทำให้อ่านค่าจาก nextion ไม่ได้
}

void loop(){
  if(millis() - lastMills2 > 1500){   //- เวลา Scan ค่าจาก Sensor ที่จะทำให้ไม่เกิดการทับกันของสัญญาณ คือประมาณ 1500 - 2000 ms
    lastMills2 = millis();
    int va = myMaster.readHoldingRegistersI(1,1);
    Serial.print("Modbus value: "); Serial.println(va);
    
    //val = myNex.readNumber("jCH1.val");
    //Serial.print("jCH1 Button: "); Serial.println(val);
  }
  myNex.NextionListen();
}

void trigger1(){
  toggleLed();
}