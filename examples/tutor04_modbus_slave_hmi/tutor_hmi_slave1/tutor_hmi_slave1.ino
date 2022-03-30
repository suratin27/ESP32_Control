#define ESP32_CONTROL_10RS

#include <ESP32Control.h>
#include <Arduino.h>

/*---------------------------------------------------------------------
          define Modbus Slave Serail Object
---------------------------------------------------------------------*/
TaskHandle_t t1 = NULL;
ModbusSlaveSerial iSlave;
const TickType_t xDelay20ms = pdMS_TO_TICKS(20);
//const TickType_t xDelay50ms = pdMS_TO_TICKS(50);

/*---------------------------------------------------------------------
                        This part for Web monitor
---------------------------------------------------------------------*/
void initModbusSlave(HardwareSerial* _Serial, unsigned long _baud,uint8_t _slave){
 Serial.println("-----------------------.");
 Serial.println("Init Modbus Slave.");
 iSlave.config(_Serial,_baud);
 iSlave.setSlaveId(_slave);

 iSlave.addCoil(0);  //Coil0
 iSlave.addCoil(1);  //Coil1
 iSlave.addCoil(2);  //Coil2

 iSlave.addIreg(0);  //Level
 iSlave.addIreg(1);  //Distance
 iSlave.addIreg(2);  //Flow rate

 iSlave.addHreg(0);  
 iSlave.addHreg(1);  
 iSlave.addHreg(2); 
 iSlave.addHreg(3); 
 iSlave.addHreg(4);  

 Serial.println("Done Init Modbus Slave.");
 Serial.println("-----------------------.");
}
void modbusSlaveTask(void *pvParam){
  while(1){ /*    
   if(needSuspend){
    Serial.println("Task suspended !");
    vTaskSuspend(NULL);    
   }*/

   iSlave.task();
   //toggleLed();
   vTaskDelay(xDelay20ms);   
  }    
vTaskDelete(NULL);
} 

void setup() {
  Serial.begin(9600); 
  initModbusSlave(&Serial1,9600,5);  //พารามิเตอร์ (Serial1 ,Baudrate ,Slave address)
  initIO();

  /* Modbus Slave Connection Task - Run in CPU Core1 */
  xTaskCreatePinnedToCore(modbusSlaveTask,"modbusSlaveTask",3000,(void*) NULL,2,&t1,1);

}
void loop() {
  iSlave.Hreg(0,iSlave.Ireg(0));    //นำค่าจาก Inputregistor 0 มาใส่ใน Holding registor 0
  if(iSlave.Coil(0)==true){     //เมื่อสั่งให้ coil0 ติด  output0 จะติดทำงาน
    setOutput(0);
  }else{
    resetOutput(0);
  }
  delay(50);
}