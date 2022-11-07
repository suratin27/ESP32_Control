#include <Arduino.h>
#line 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor07_modbus_slave_tcp\\tutor07_modbus_slave_tcp.ino"
#line 1 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor07_modbus_slave_tcp\\tutor07_modbus_slave_tcp.ino"
#define ESP32_CONTROL_10RS
#include <ESP32Control.h>

/*-------------------------------------------------------------------------
                            Variable declarations
-------------------------------------------------------------------------*/
ModbusIP tcpSlave;                            //- Declare Modbus slave TCP Object

/*-------------------------------------------------------------------------
                              Firmware setup
-------------------------------------------------------------------------*/
#line 12 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor07_modbus_slave_tcp\\tutor07_modbus_slave_tcp.ino"
void setup();
#line 40 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor07_modbus_slave_tcp\\tutor07_modbus_slave_tcp.ino"
void loop();
#line 12 "e:\\Arduino_Lib\\libraries\\ESP32_Control\\examples\\tutor07_modbus_slave_tcp\\tutor07_modbus_slave_tcp.ino"
void setup(){
  Serial.begin(115200);
  initIO();                                   //- Init Input and Output for version 1.0RX ,2022RLX-32U
 
  WiFi.begin("JURAPORN_2.4G", "044801060");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  tcpSlave.server(502);                       //- Set server port (default port 502 for Modbus TCP)

  tcpSlave.addHreg(0, 100);                   //- Declare Holding registor 0
  tcpSlave.addHreg(1, 100);                   //- Declare Holding registor 1

  tcpSlave.addIreg(0, 100);                   //- Declare Input registor 0
  tcpSlave.addIreg(1, 100);                   //- Declare Input registor 1
}

/*-------------------------------------------------------------------------
                            Firmware main loop
-------------------------------------------------------------------------*/
void loop(){
  //toggleSTS(0);                   //- Toggle STS0 when use 2022RLX-32U
  //toggleSTS(1);                   //- Toggle STS1 when use 2022RLX-32U

  tcpSlave.Hreg(0,random(50));    //- Random data and assign to Hreg 0
  tcpSlave.Ireg(0,random());      //- Random data and assign to Ireg 0
  tcpSlave.task();
  delay(5);
}
