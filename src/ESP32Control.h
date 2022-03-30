#ifndef ESP32CONTROL_H
#define ESP32CONTROL_H

#if defined(ESP32_CONTROL_10RS)
  #include "ModbusSlaveSerial.h"
  #include "ModbusMaster232.h"
#endif
#if defined(ESP32_CONTROL_10) || defined(ESP32_CONTROL_10RS)
  #include "ESP32ControlIO.h"
  #include "modbusTTCP.h"
  //#include "ESP32Time.h"
  #include "ESP32ControlSchedule.h"
  #include "ModbusIP_ESP8266.h"
  //const bool DEBUG = true;
#endif


#endif